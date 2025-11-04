#include "AnimationSystem.h"
#include "Systems/Graphics.h"

namespace NightSnack {

	void AnimationSystem::Init() {
		// Initialise the map of animation clips with all the stored pointer textures from Graphics System
		// Using each texture, extract the animation clips from the single texture image
		// Etc, texture banana has 2 animation clips:
		// "banana_0", "banana_1"

		const auto& texturesMap = m_coord.GetSystem<GraphicsSystem>()->GetTexturesMap();

		// Iterate through all the textures and extract the animations if needed
		for (const auto& [textureName, texturePtr] : texturesMap) {
			Texture* tex = texturePtr.get();
			int cols = tex->GetColumns();
			int rows = tex->GetRows();

			// Skip static textures (1x1 means no animation)
			if (cols == 1 && rows == 1) {
				continue;
			}

			//std::cout << "[INIT] Processing texture: " << textureName << " has " << cols
			//	<< " columns, " << rows << " rows\n";

			// Separate the texture into its different animation clips
			// Etc 1 texture image has 2 animation clips from left to right
			float frameWidth = 1.f / static_cast<float>(cols);
			float frameHeight = 1.f / static_cast<float>(rows);

			// Each row corresponds to one animation clip
			for (int r{}; r < rows; ++r) {
				AnimationClip clip;
				clip.texture = tex;

				std::string clipName = textureName + "_" + std::to_string(r);
				//std::cout << "Creating AnimationClip: " << clipName << std::endl;

				for (int c{}; c < cols; ++c) {
					AnimationFrame frame;
					frame.U0 = c * frameWidth;
					frame.U1 = (c + 1) * frameWidth;
					// Invert V coords because texture coords start from bottom left
					// We want the coords from our sprite sheet to start from top left
					frame.V0 = 1.f - (r + 1) * frameHeight;
					frame.V1 = 1.f - r * frameHeight;
					frame.timeDelay = 20.f; 

					clip.frames.emplace_back(frame);
				}

				// Store this clip under this texture's animation set
				m_animationClips[textureName][clipName] = clip;
			}
		}
	}

	void AnimationSystem::Update(int steps) {
		// Iterate through all entities that have Animator component
		for (Entity e : m_entities) {
			auto& animator = m_coord.ModifyComponent<Animator>(e);
			auto& ren = m_coord.ModifyComponent<Renderable>(e);

			// Skip the updating of this sprite sheet
			// User either did not properly initialized Texture[cols, rows] or
			// User added an Animator component to a static texture
			Texture* tex = m_coord.GetSystem<GraphicsSystem>()->GetTexture(animator.spriteSheetName);
			if (tex->GetColumns() == 1 && tex->GetRows() == 1) {
				std::cerr << "[AnimationSystem] Current Entity texture is treated as static. Initialize Texture[cols, rows] or remove Entity's Animator Component!\n";
				continue;
			} 

			// Skip if this Animator is not playing
			if (!animator.isPlaying) {
				continue;
			}

			// If Archetypes.json did not state the current animation clip, set the default clip
			if (animator.currentAnimationClip.empty()) {
				// Set the first Animation Clip to be the default clip
				animator.currentAnimationClip = m_animationClips[animator.spriteSheetName].begin()->first;
				//std::cout << "Empty current clip changed to: " << animator.currentAnimationClip << std::endl;
			} 

			// Here, there is a current Animation Clip being played
			// Update the time/steps and switch to the next frame of the Animation Clip if needed to
			
			// Current animation clip and frame
			auto& clip = m_animationClips[animator.spriteSheetName][animator.currentAnimationClip];
			auto& currentFrame = clip.frames[animator.currentFrameIndex];

			animator.currentTime += steps * animator.playbackSpeed;

			// Move to next animation frame if needed
			if (animator.currentTime >= currentFrame.timeDelay) {
				animator.currentTime = 0.f;
				++animator.currentFrameIndex;

				// Reached the end of the animation clip
				if (animator.currentFrameIndex >= static_cast<int>(clip.frames.size())) {
					if (animator.isLooping) {
						animator.currentFrameIndex = 0; // Reset to the start again
					} else {
						// Make it stuck at this frame if it is not looping
						animator.isPlaying = false;
						animator.currentFrameIndex = static_cast<int>(clip.frames.size()) - 1;
					}
				}
			}

			// Fetch the new frame UV
			const AnimationFrame& frame = clip.frames[animator.currentFrameIndex];

			// Update the UV coordinates, to be retrieved on the Graphics System side
			if (m_coord.HasComponent<RigidBody>(e)) {
				auto& rb = m_coord.GetComponent<RigidBody>(e);
				
				if (rb.vel.x < 0.f) {
					animator.lastFacingRight = false;
				} else if (rb.vel.x > 0.f) {
					animator.lastFacingRight = true;
				}

				if (!animator.lastFacingRight) {
					// Entity is currently facing the left direction
					// Set the UV coordinates as default state, facing to the left
					animator.uvTopLeft = { frame.U0, frame.V0 };
					animator.uvBottomRight = { frame.U1, frame.V1 };
				} else {
					// Entity is currently facing the right direction
					// Face to the right
					animator.uvTopLeft = { frame.U1, frame.V0 };
					animator.uvBottomRight = { frame.U0, frame.V1 };
				}

			} else { 
				// No RigidBody component, set the UV coordinates as default state
				// Default is looking to the left
				animator.uvTopLeft = { frame.U0, frame.V0 };
				animator.uvBottomRight = { frame.U1, frame.V1 };
			}
		}
	}
} // NightSnack
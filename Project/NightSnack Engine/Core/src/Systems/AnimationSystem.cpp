/******************************************************************************
 * File:        AnimationSystem.cpp
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created Animation System
 *
 * Description:
 * This file contains the definitions of the Animation System.
 * It initializes the AnimationClips and AnimationFrames of every texture loaded, based on its number of columns and rows.
 * It manages the updating of UV coordinates of the current AnimationClip, which is used by the Graphics System.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "AnimationSystem.h"
#include "Systems/Graphics.h"
#include "Factory/EntityFactory.h"

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

			CreateAnimationClips(textureName, tex, cols, rows);
		}
	}

	void AnimationSystem::Update() {
		if (m_coord.IsPaused()) return;

		// Iterate through all entities that have Animator component
		for (Entity e : m_entities) {
			auto& animator = m_coord.ModifyComponent<Animator>(e);
			auto& ren = m_coord.GetComponent<Renderable>(e);

			// Skip the updating of this sprite sheet
			// User either did not properly initialized Texture[cols, rows] or
			// User added an Animator component to a static texture
			////std::cout << "Renderable name: " << ren.materialName << std::endl;
			Material* mat = m_coord.GetSystem<GraphicsSystem>()->GetMaterial(ren.materialName);
			if (!mat) { continue; }
			Texture* tex = mat->GetTexture();
			if (!tex) {
				////std::cout << "Texture does not exist\n";
				continue;
			}
			////std::cout << "Spritesheet name: " << animator.spriteSheetName << std::endl;
			if (tex->GetColumns() == 1 && tex->GetRows() == 1) {
				std::cerr << "[AnimationSystem] Current Entity texture is treated as static. Ensure Texture name has [_cols_rows] or remove Entity's Animator Component!\n";
				continue;
			}

			// Skip if this Animator is not playing
			if (!animator.isPlaying) {
				continue;
			}

			// Get the spritesheet name based on the texture it is using
			if (animator.spriteSheetName.empty() || animator.spriteSheetName != tex->GetTextureName()) {
				////std::cout << "Spritesheet name: " << animator.spriteSheetName << std::endl;
				animator.spriteSheetName = tex->GetTextureName();
				// The texture has been changed for this Entity, but the animator is still using the old spritesheet
				animator.currentAnimationClip.clear(); // Reset the current AnimationClip
			}

			if (m_animationClips.find(animator.spriteSheetName) == m_animationClips.end()) {
				UpdateAnimationClipsMap();
			}

			// If Archetypes.json did not state the current animation clip, set the default clip
			if (animator.currentAnimationClip.empty()) {
				// Set the first Animation Clip to be the default clip
				animator.currentAnimationClip = m_animationClips[animator.spriteSheetName].begin()->first;
				////std::cout << "Empty current clip changed to: " << animator.currentAnimationClip << std::endl;
			}

			// Here, there is a current Animation Clip being played
			// Update the time/steps and switch to the next frame of the Animation Clip if needed to

			// Current animation clip and frame
			auto& clip = m_animationClips[animator.spriteSheetName][animator.currentAnimationClip];
			auto& currentFrame = clip.frames[animator.currentFrameIndex];

			animator.currentTime += m_coord.steps * animator.playbackSpeed;

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
				// Has RigidBody component, set the UV coordinates according to where it moves
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

	void AnimationSystem::UpdateAnimationClipsMap() {
		// Initialize AnimationClips for any newly loaded textures

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

			// Only create AnimationClips for newly loaded textures
			if (m_animationClips.find(textureName) == m_animationClips.end()) {
				CreateAnimationClips(textureName, tex, cols, rows);
			}
		}
	}

	void AnimationSystem::CreateAnimationClips(std::string textureName, Texture* tex, int cols, int rows) {
		////std::cout << "[INIT] Processing texture: " << textureName << " has " << cols
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
			////std::cout << "Creating AnimationClip: " << clipName << std::endl;

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

		////std::cout << "Created AnimationClip for: " << textureName << std::endl;
	}

	std::vector<std::string> AnimationSystem::GetSpritesheetAnimationClipsNames(Entity e) {
		auto& animator = m_coord.GetComponent<Animator>(e);
		std::vector<std::string> ret;

		// This entity does not have a spritesheet/texture attached to it
		if (animator.spriteSheetName.empty()) {
			return {};
		}

		// Iterate through all AnimationClips in this spritesheet
		for (const auto& [animClipName, animClip] : m_animationClips[animator.spriteSheetName]) {
			// Collect the AnimationClip names
			ret.emplace_back(animClipName);
		}

		return ret;
	}

	void AnimationSystem::SetCurrentAnimationClip(const std::string& animationClipName, Entity e) {

		auto& animator = m_coord.ModifyComponent<Animator>(e);

		// Check whether the Entity has a spritesheet attached to it
		if (animator.spriteSheetName.empty()) {
			std::cerr << "[AnimationSystem] Error: This Entity does not have a Spritesheet!\n";
			return;
		}

		if (m_animationClips[animator.spriteSheetName].find(animationClipName) == m_animationClips[animator.spriteSheetName].end()) {
			std::cerr << "[AnimationSystem] Error: This AnimationClip name does not exist!\n";
			return;
		}

		animator.currentAnimationClip = animationClipName;
		// Restart by default whenever change Animation Clips
		animator.currentTime = 0.f;
		animator.currentFrameIndex = 0;
	}

	void AnimationSystem::ChangeAnimation() {
		// Currently only change animation clip for the "AnimatedRubric" prefab
		for (Entity e : m_entities) {

			std::string prefabName = m_coord.GetSystem<EntityFactory>()->GetEntityPrefabName(e);
			if (prefabName != "AnimatedRubric") { continue; }

			auto& animator = m_coord.ModifyComponent<Animator>(e);

			std::string currAnimClipName = animator.currentAnimationClip;
			std::vector<std::string> animClipNames = GetSpritesheetAnimationClipsNames(e);

			if (animClipNames.empty()) { return; }

			// If there is no current Animation Clip
			if (currAnimClipName.empty()) {
				currAnimClipName = animClipNames[0]; // Assign the first clip by default
				break;
			}

			for (const auto& name : animClipNames) {
				// Assign a random clip to change
				if (name != currAnimClipName) {
					SetCurrentAnimationClip(name, e);
					break;
				}
			}
		}
	}

} // NightSnack
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

			CreateAnimationClips(textureName, cols, rows);
		}
	}

	void AnimationSystem::Update() {
		//if (m_coord.IsPaused()) return;
		
		if (m_animationClips.empty()) {
			return;
		}

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
				//std::cerr << "[AnimationSystem] Current Entity texture is treated as static. Ensure Texture name has [_cols_rows] or remove Entity's Animator Component!\n";

				// If this entity previously had an animated spritesheet, but user changed it to have a static texture, 
				// clear the previous spritesheet name and current animation clip
				if (!animator.spriteSheetName.empty()) {
					animator.spriteSheetName.clear();
					animator.currentAnimationClip.clear();
					// Reset the uv values as well so that the texture is rendered properly
					animator.uvTopLeft = { 0.f, 0.f };
					animator.uvBottomRight = { 1.f, 1.f };
				}

				// Texture still exists but it is static, at least make the texture be facing the correct direction
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
						animator.uvTopLeft = { 1.f, 0.f };
						animator.uvBottomRight = { 0.f, 1.f };
					} else {
						// Entity is currently facing the right direction
						// Face to the right
						animator.uvTopLeft = { 0.f, 0.f };
						animator.uvBottomRight = { 1.f, 1.f };
					}

				} else {
					// No RigidBody component, set the UV coordinates as default state
					// Default is looking to the left
					animator.uvTopLeft = { 1.f, 0.f };
					animator.uvBottomRight = { 0.f, 1.f };
				}

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
				CreateAnimationClips(textureName, cols, rows);
			}
		}
	}

	void AnimationSystem::CreateAnimationClips(std::string textureName, int cols, int rows) {
		////std::cout << "[INIT] Processing texture: " << textureName << " has " << cols
		//	<< " columns, " << rows << " rows\n";

		//std::string textureStemName = std::filesystem::path(textureName).stem().string();

		// Separate the texture into its different animation clips
		// Etc 1 texture image has 2 animation clips from left to right
		float frameWidth = 1.f / static_cast<float>(cols);
		float frameHeight = 1.f / static_cast<float>(rows);

		// Each row corresponds to one animation clip
		for (int r{}; r < rows; ++r) {
			AnimationClip clip;
			clip.name = "Clip_" + std::to_string(r); // default clip name that can be renamed by the user

			std::string clipID = textureName + "_" + std::to_string(r);
			////std::cout << "Creating AnimationClip: " << clipName << std::endl;

			for (int c{}; c < cols; ++c) {
				AnimationFrame frame;
				frame.U0 = c * frameWidth;
				frame.U1 = (c + 1) * frameWidth;
				// Invert V coords because texture coords start from bottom left
				// We want the coords from our sprite sheet to start from top left
				frame.V0 = 1.f - (r + 1) * frameHeight;
				frame.V1 = 1.f - r * frameHeight;
				frame.timeDelay = 20; // by default

				clip.frames.emplace_back(frame);
			}

			// Store this clip under this texture's animation set
			m_animationClips[textureName][clipID] = clip;
		}

		LoadAnimationAsset(textureName);
		//std::cout << "Created AnimationClip for: " << textureName << std::endl;
	}

	void AnimationSystem::ChangeAnimation() {
		// Currently only change animation clip for the "AnimatedRubric" prefab
		// For testing purposes
		static int temp = 0;
		for (Entity e : m_entities) {

			std::string prefabName = m_coord.GetSystem<EntityFactory>()->GetEntityPrefabName(e);
			if (prefabName != "AnimatedRubric") { continue; }

			if (temp == 0) {
				StopAnimation(e); // Stops bluespell animation
			} else if (temp == 1) {
				PlayAnimation(e, true); // Play bluespell animation from the start
			} else if (temp == 2) {
				SetCurrentAnimationClipByName(e, "dead_wrongname"); // nothing happens, wrong clip name
			} else if (temp == 3) {
				SetCurrentAnimationClipByName(e, "dead"); // Change clip to dead animation, continues playing
			} else if (temp == 4) {
				SetCurrentAnimationClipByName(e, "bluespell"); // Change clip to bluespell animation, continues playing
			}
		}
		++temp;
	}

	void AnimationSystem::SaveAnimationAsset(const std::string& spritesheet) {
		auto it = m_animationClips.find(spritesheet);
		if (it == m_animationClips.end()) {
			// This spritesheet does not even exist somehow, return it!
			return;
		}

		json j;
		j["spritesheet"] = spritesheet; 

		json clips;
		// Serialize each Animation Clip
		for (const auto& [clipID, clip] : it->second) {
			json clipJson;
			clipJson["name"] = clip.name;

			json frames = json::array();
			// Serialize each Animation Frame
			for (const auto& f : clip.frames) {
				// Only have timeDelay to save 
				frames.push_back({ {"timeDelay", f.timeDelay} });
			}
			clipJson["frames"] = frames;

			clips[clipID] = clipJson;
		}

		j["clips"] = clips;

		std::filesystem::path path = std::filesystem::path(RESOURCE_PATH) / "GameData/Animations/" / (spritesheet + ".json");
		// Ensure directory exists before writing to it
		std::filesystem::create_directories(path.parent_path());

		// Write it to disk
		std::ofstream ofs(path);
		ofs << std::setw(4) << j;

		//std::cout << "[AnimationSystem] Saved Animation Asset: " << path << std::endl;
	}

	void AnimationSystem::LoadAnimationAsset(const std::string& spritesheet) {

		std::filesystem::path path = std::filesystem::path(RESOURCE_PATH) / "GameData/Animations/" / (spritesheet + ".json");

		if (!std::filesystem::exists(path)) {
			return;
		}

		// Load data from the file
		std::ifstream ifs(path);
		json j;
		ifs >> j;

		// Get the Animation Clips from this spritesheet
		auto& runtimeClips = m_animationClips[spritesheet];

		// Iterate through the saved Animation Clips
		for (auto& [clipID, clipJson] : j["clips"].items()) {
			// Skip clips that do not exist in the current runtime data
			if (runtimeClips.find(clipID) == runtimeClips.end())
				continue;

			auto& clip = runtimeClips[clipID];

			// Restore clip name
			if (clipJson.contains("name"))
				clip.name = clipJson["name"].get<std::string>();

			// Restore frame data
			auto& frames = clipJson["frames"];
			size_t count = std::min(frames.size(), clip.frames.size());
			for (size_t i = 0; i < count; ++i) {
				clip.frames[i].timeDelay = frames[i]["timeDelay"].get<int>();
			}
		}

		//std::cout << "[AnimationSystem] Loaded Animation Asset: " << path << std::endl;
	}

	std::string AnimationSystem::FindAnimationClipIDByName(const std::string& spritesheet, const std::string& name) {
		auto it = m_animationClips.find(spritesheet);
		if (it == m_animationClips.end()) {
			return "";
		}

		for (const auto& [clipID, clip] : it->second) {
			if (clip.name == name) {
				return clipID;
			}
		}

		return "";
	}

	void AnimationSystem::PlayAnimation(Entity e, bool restart) {
		if (!m_coord.HasComponent<Animator>(e)) {
			return;
		}
		auto& animator = m_coord.ModifyComponent<Animator>(e);
		animator.isPlaying = true;

		// To restart or to continue playing from where it stopped?
		if (restart) {
			animator.currentTime = 0.f;
			animator.currentFrameIndex = 0;
		}
	}

	void AnimationSystem::StopAnimation(Entity e) {
		if (!m_coord.HasComponent<Animator>(e)) {
			return;
		}
		auto& animator = m_coord.ModifyComponent<Animator>(e);
		animator.isPlaying = false;
	}
	
	bool AnimationSystem::IsPlayingAnimation(Entity e) const {
		if (!m_coord.HasComponent<Animator>(e)) {
			return false;
		}
		return m_coord.GetComponent<Animator>(e).isPlaying;
	}

	bool AnimationSystem::IsLoopingAnimation(Entity e) const {
		if (!m_coord.HasComponent<Animator>(e)) {
			return false;
		}
		return m_coord.GetComponent<Animator>(e).isLooping;
	}

	void AnimationSystem::SetLoopingAnimation(Entity e, bool loop) {
		if (!m_coord.HasComponent<Animator>(e)) {
			return;
		}
		m_coord.ModifyComponent<Animator>(e).isLooping = loop;
	}

	void AnimationSystem::SetCurrentAnimationClipByID(const std::string& animationClipName, Entity e) {

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

	bool AnimationSystem::SetCurrentAnimationClipByName(Entity e, const std::string& clipName) {
		// This entity has no Animator component
		if (!m_coord.HasComponent<Animator>(e)) {
			return false;
		}

		auto& animator = m_coord.ModifyComponent<Animator>(e);

		// This entity has no texture
		if (animator.spriteSheetName.empty()) {
			return false;
		}

		auto spritesheetIt = m_animationClips.find(animator.spriteSheetName);

		// This spritesheet is invalid
		if (spritesheetIt == m_animationClips.end()) {
			return false;
		}

		// Find the Animation Clip ID by the display name (editable)
		std::string clipID = FindAnimationClipIDByName(spritesheetIt->first, clipName);
		if (clipID.empty()) {
			return false;
		}

		SetCurrentAnimationClipByID(clipID, e);

		return true;
	}

	std::string AnimationSystem::GetCurrentAnimationClipName(Entity e) const {
		if (!m_coord.HasComponent<Animator>(e)) {
			return "";
		}

		const auto& animator = m_coord.GetComponent<Animator>(e);
		if (animator.spriteSheetName.empty()) { // No spritesheet
			return "";
		}

		auto spritesheetIt = m_animationClips.find(animator.spriteSheetName);
		if (spritesheetIt == m_animationClips.end()) { // Invalid spritesheet
			return "";
		}

		auto clipIt = spritesheetIt->second.find(animator.currentAnimationClip);
		if (clipIt == spritesheetIt->second.end()) { // Invalid animation clip
			return "";
		}

		// Display name
		return clipIt->second.name; 
	}

	std::vector<std::string> AnimationSystem::GetSpritesheetAnimationClipsNames(Entity e) {
		if (!m_coord.HasComponent<Animator>(e)) {
			return {};
		}

		auto& animator = m_coord.GetComponent<Animator>(e);
		std::vector<std::string> ret;

		// This entity does not have a spritesheet/texture attached to it
		if (animator.spriteSheetName.empty()) {
			return {};
		}

		// Iterate through all AnimationClips in this spritesheet
		for (const auto& [animClipName, animClip] : m_animationClips[animator.spriteSheetName]) {
			// Collect the AnimationClip names
			// Not the clip ID, but the user editable name
			ret.emplace_back(animClip.name);
		}

		return ret;
	}

	void AnimationSystem::SetAnimationPlaybackSpeed(Entity e, float speed) {
		if (!m_coord.HasComponent<Animator>(e)) {
			return;
		}
		// Playback speed should not be negative
		m_coord.ModifyComponent<Animator>(e).playbackSpeed = std::max(0.f, speed);
	}

	float AnimationSystem::GetAnimationPlaybackSpeed(Entity e) const {
		if (!m_coord.HasComponent<Animator>(e)) {
			return 1.f;
		}
		return m_coord.GetComponent<Animator>(e).playbackSpeed;
	}

} // NightSnack
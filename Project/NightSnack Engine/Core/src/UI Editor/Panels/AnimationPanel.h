/******************************************************************************
 * File:        AnimationPanel.h
 * Project:     NightSnack
 * Author(s):
 *
 * Description:
 *
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once
#include "AllPanels.h"
#include "Graphics/Texture.h"
#include "Systems/AnimationSystem.h"
#include "Factory/EntityFactory.h"
#include <stb_image.h>

namespace fs = std::filesystem;

namespace NightSnack
{

	/**
	* @brief Animation panel for managing animations
	*/
	class AnimationPanel {
	public:
		/**
		* @brief constructor
		* @param coord reference to the ECS coordinator
		*/
		AnimationPanel(Coordinator& coord, std::shared_ptr<Entity> selectedEntity) :
			m_coord(coord),
			m_selectedEntity(selectedEntity)
		{

		}

		/**
		* @brief draw the animation panel
		*/
		void DrawAnimationPanel() {
			// Top controls
			static int selectedClip = 0;
			static int selectedFrame = -1;
			static int lastSelectedFrame = -1;
			static int lastSelectedClip = -1;
			static int timeDelay = 20;
			static Entity lastSelectedEntity = INVALID_ENTITY;

			static bool isPlaying = false;
			static int playbackTimerSteps = 0;
			static int previewFrame = 0;

			// Check whether an Entity is selected or not
			// If not selected, do not display anything, prompt user to select an Entity
			// If selected, display all the Animation Clips that belong to that Entity (based on its texture) 
			if (*m_selectedEntity == INVALID_ENTITY) {
				ImGui::TextDisabled("Select an Entity to edit its Animation Clips");
				return;
			}

			if (!m_coord.HasComponent<Animator>(*m_selectedEntity)) {
				ImGui::TextDisabled("Selected Entity has no Animator");
				return;
			}

			if (!m_coord.HasComponent<Renderable>(*m_selectedEntity)) {
				ImGui::TextDisabled("Selected Entity has no Renderable");
				return;
			}




			static std::string lastAnimatorClip;
			static Entity lastEntity = INVALID_ENTITY;

			auto& animator = m_coord.GetComponent<Animator>(*m_selectedEntity);

			bool animatorClipChanged = (lastEntity != *m_selectedEntity) || (lastAnimatorClip != animator.currentAnimationClip);

			std::vector<std::string> clipNames = m_coord.GetSystem<AnimationSystem>()->GetSpritesheetAnimationClipsNames(*m_selectedEntity);

			if (animatorClipChanged) {
				for (int i{ 0 }; i < static_cast<int>(clipNames.size()); ++i) {
					if (clipNames[i] == animator.currentAnimationClip) {
						selectedClip = i;
						break;
					}
				}

				// Reset preview related things
				selectedFrame = -1;
				previewFrame = 0;
				isPlaying = false;
				lastAnimatorClip = animator.currentAnimationClip;
				lastEntity = *m_selectedEntity;
			}

			// Reset to prevent error if number of Animation Clips vary between spritesheets
			if (selectedClip >= static_cast<int>(clipNames.size())) {
				selectedClip = 0;
			}

			if (!clipNames.empty())
			{
				ImGui::Text("Animation Clips");
				ImGui::SameLine();
				ImGui::Combo(
					"##AnimationClipsCombo", // ## makes it not visible, this is an ID only
					&selectedClip,
					[](void* data, int idx, const char** out_text)
					{
						auto& clips = *static_cast<std::vector<std::string>*>(data);
						*out_text = clips[idx].c_str();
						return true;
					},
					&clipNames,
					static_cast<int>(clipNames.size())
				);

				ImGui::SameLine();
				if (ImGui::Button("Play")) {
					isPlaying = true;
					previewFrame = (selectedFrame >= 0) ? selectedFrame : 0;
					playbackTimerSteps = 0;
				}
				ImGui::SameLine();
				if (ImGui::Button("Pause")) {
					isPlaying = false;
				}

				ImGui::Separator();

				// Timeline
				ImGui::Text("Timeline");
				ImGui::Separator();

				float fullWidth = ImGui::GetContentRegionAvail().x;

				ImGui::Columns(2, "TimelineWithPreview", false);
				ImGui::SetColumnWidth(0, fullWidth * 0.65f);

				ImVec2 childSize(0, 220);

				ImGui::BeginChild("Timeline", childSize, true);

				auto& allClips = m_coord.GetSystem<AnimationSystem>()->m_animationClips;
				std::string spritesheetName = m_coord.GetComponent<Animator>(*m_selectedEntity).spriteSheetName;

				auto spritesheetIt = allClips.find(spritesheetName);
				if (spritesheetIt == allClips.end()) {
					return;
				}
				auto clipIt = spritesheetIt->second.find(clipNames[selectedClip]);
				if (clipIt == spritesheetIt->second.end()) {
					return;
				}

				const int frameCount = static_cast<int>(clipIt->second.frames.size());

				// Reset to prevent error if the number of frames vary between Animation Clips
				if (selectedFrame >= frameCount) {
					selectedFrame = -1;
				}

				// Draw the frame number in 1 row
				for (int i{ 0 }; i < frameCount; ++i) {
					ImGui::PushID(i);

					int durationSteps = clipIt->second.frames[i].timeDelay;

					bool clicked = ImGui::Selectable(
						("Frame " + std::to_string(i)).c_str(),
						selectedFrame == i,
						0,
						ImVec2(80, 50)
					);

					if (clicked) {
						selectedFrame = i;
					}

					ImVec2 rectMin = ImGui::GetItemRectMin();

					ImGui::GetWindowDrawList()->AddText(
						ImVec2(rectMin.x + 2, rectMin.y + 22),
						IM_COL32(160, 160, 160, 255),
						(std::to_string(durationSteps) + " steps").c_str()
					);

					ImGui::SameLine();
					ImGui::PopID();
				}
				ImGui::NewLine();

				// Draw each frame of the current Animation clip in 1 rpw
				for (int i{ 0 }; i < frameCount; ++i) {
					ImTextureID texID = GetOrLoadTexture(spritesheetName);
					if (!texID) {
						continue;
					}

					// Need to slice the texture image again using the number of cols and rows this spritesheet has
					// cols use frameCount
					// rows use clipNames.size()
					// selectedClip represents which is the current clip in the whole spritesheet
					const int cols = frameCount;
					const int rows = static_cast<int>(clipNames.size());

					float u0 = static_cast<float>(i) / cols;
					float v0 = static_cast<float>(selectedClip) / rows;
					float u1 = static_cast<float>(i + 1) / cols;
					float v1 = static_cast<float>(selectedClip + 1) / rows;

					ImGui::Image(texID, ImVec2(80, 64), ImVec2(u0, 1.f - v0), ImVec2(u1, 1.f - v1));
					ImGui::SameLine();
				}

				ImGui::EndChild();
				ImGui::NextColumn();

				ImGui::BeginChild("AnimationPreview", childSize, true);

				// Update the preview Animation overtime
				if (isPlaying && frameCount > 0)
				{
					std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
					static std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();
					static float totalTime = 0.f;
					std::chrono::duration<float> frameTime = currentTime - lastTime;
					lastTime = currentTime;

					float dt = frameTime.count();

					// Clamp very large frame times (debugger pauses, alt-tab, etc.)
					if (dt > 0.25f) { // MAX_FRAME_TIME is 0.25f
						dt = 0.25f;
					}
					totalTime += dt;
					// Compute number of steps
					int steps = 0;
					while (totalTime >= FixedDT) {
						steps += 1;
						totalTime -= FixedDT;
					}

					int frameDuration = clipIt->second.frames[previewFrame].timeDelay;
					playbackTimerSteps += steps;

					if (playbackTimerSteps >= frameDuration)
					{
						// Go to the next frame
						playbackTimerSteps -= frameDuration;
						previewFrame++;

						// Loop animation
						if (previewFrame >= frameCount)
							previewFrame = 0;
					}
				}

				ImGui::Text("Preview");
				ImGui::Separator();
				ImTextureID texID = GetOrLoadTexture(spritesheetName);
				if (texID && frameCount > 0)
				{
					const int cols = frameCount;
					const int rows = static_cast<int>(clipNames.size());

					int frameToShow = isPlaying ? previewFrame : selectedFrame;

					if (frameToShow < 0)
						frameToShow = 0;

					float u0 = static_cast<float>(frameToShow) / cols;
					float v0 = static_cast<float>(selectedClip) / rows;
					float u1 = static_cast<float>(frameToShow + 1) / cols;
					float v1 = static_cast<float>(selectedClip + 1) / rows;

					// Center the preview
					float previewSize = 96.0f;
					float availX = ImGui::GetContentRegionAvail().x;
					ImGui::SetCursorPosX((availX - previewSize) * 0.5f);

					ImGui::Image(
						texID,
						ImVec2(previewSize, previewSize),
						ImVec2(u0, 1.f - v0),
						ImVec2(u1, 1.f - v1)
					);

					ImGui::Spacing();
					ImGui::Text("Frame: %d", frameToShow);
				} else
				{
					ImGui::TextDisabled("No preview available");
				}

				ImGui::EndChild();
				ImGui::Columns(1);

				// Frame Inspector
				ImGui::Separator();
				ImGui::Text("Frame Properties");

				if (selectedFrame >= 0 && selectedFrame < static_cast<int>(clipIt->second.frames.size())) {

					// Frame selection has changed, update the slider data accordingly
					if (selectedFrame != lastSelectedFrame) {
						timeDelay = clipIt->second.frames[selectedFrame].timeDelay;
						lastSelectedFrame = selectedFrame;
					}

					// Clip selection has changed, update the slider data amd pause the preview animation
					if (selectedClip != lastSelectedClip ||
						clipNames[selectedClip] != clipNames[lastSelectedClip] ||
						lastSelectedEntity != *m_selectedEntity) {

						timeDelay = clipIt->second.frames[selectedFrame].timeDelay;
						lastSelectedClip = selectedClip;
						lastSelectedFrame = selectedFrame;
						lastSelectedEntity = *m_selectedEntity;
						isPlaying = false;
					}

					ImGui::Text("Selected Frame: %d", selectedFrame);
					ImGui::SliderInt("Frame Duration (steps)", &timeDelay, 1, 200);

					if (ImGui::Button("Apply")) {
						clipIt->second.frames[selectedFrame].timeDelay = timeDelay;
					}

					ImGui::SameLine();
					if (ImGui::Button("Save Animation")) {
						// Serialize animation asset to disk
						m_coord.GetSystem<AnimationSystem>()->SaveAnimationAsset(animator.spriteSheetName);
					}
				} else {
					lastSelectedFrame = -1;
					ImGui::TextDisabled("Select a frame to edit.");
				}
			} else {
				ImGui::TextDisabled("No Animation Clips available");
			}
		}

		/**
		* @brief draw the assets and animation panel
		*/
		void Draw()
		{
			ImGui::Begin("Animation");

			if (ImGui::BeginTabBar("AnimationTab")) {

				if (ImGui::BeginTabItem("Timeline")) {
					DrawAnimationPanel();
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
			ImGui::End();
		}

	private:
		Coordinator& m_coord;
		std::shared_ptr<Entity> m_selectedEntity;

		std::unordered_map<std::string, GLuint> m_loadedTextures;
		std::unordered_map<std::string, std::shared_ptr<Texture>> m_textureObjects;

		/**
		* @brief gets or loads a texture from cache
		*/
		ImTextureID GetOrLoadTexture(const std::string& path)
		{
			if (m_loadedTextures.find(path) != m_loadedTextures.end())
			{
				return m_loadedTextures[path];
			}

			std::shared_ptr<Texture> tex = std::make_shared<Texture>(path, m_coord.GetSystem<ResourceManager>());
			m_textureObjects[path] = tex;
			m_loadedTextures[path] = (ImTextureID)(intptr_t)tex->GetID();

			return m_loadedTextures[path];
		}

	};
}
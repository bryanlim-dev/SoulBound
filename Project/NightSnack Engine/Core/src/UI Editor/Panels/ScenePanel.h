/******************************************************************************
 * File:        ScenePanel.h
 * Project:     NightSnack
 * Author(s):	Lim Wei Jie (Primary, 95%) - Created and implemented scene panel
 *              rendering and editor camera
 *              Bryan Lim Jun Jie (Secondary, 5%) - Added creation of entity
 *              from dragging assets into scene panel
 *
 *
 * Description:
 * Scene panel showing the rendered viewport. Handles rendering the scene,
 * including the editor camera.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once
#include "AllPanels.h"

namespace NightSnack
{
	struct DefaultEditorCamera
	{
		Vec3 pos{ 0.f, 0.f, 2.f };
		float rot{ 1.0f };
		Vec2 scale{ 1.0f, 1.0f };
		float zoom{ 1.0f };
	};

	enum class SceneTool { PanZoom, Select, MultiSelect, Paint };

	class ScenePanel {
	public:

		/************************************************************************/
		/*!
		\brief
			Constructor for ScenePanel that sets up editor camera entity and framebuffer.
		*/
		/************************************************************************/
		ScenePanel(Coordinator& coord, std::shared_ptr<Entity> selectedEntity, Tilemap& tilemap)
			: m_coord(coord), m_selectedEntity(selectedEntity), m_tilemap(tilemap)
		{
			// Create editor camera entity
			CreateEditorCamera();

			// Create framebuffer and color/depth attachments
			CreateFramebuffer(1600, 900);
		}

		/************************************************************************/
		/*!
		\brief
			Destructor for ScenePanel that cleans up framebuffer and texture.
		*/
		/************************************************************************/
		~ScenePanel()
		{
			// Clean up framebuffer
			CleanupFramebuffer();
			if (m_sceneTexture)
			{
				glDeleteTextures(1, &m_sceneTexture);
			}
		}

		/************************************************************************/
		/*!
		\brief
			Draws the Scene panel, rendering the scene to a framebuffer and displaying it.
		*/
		/************************************************************************/
		void Draw()
		{
			// Scene Panel
			ImGui::Begin("Scene");

			// Get mouse world position variables
			auto inputSystem = m_coord.GetSystem<InputSystem>();
			float mouseWorldX = inputSystem->GetMousePosX();
			float mouseWorldY = inputSystem->GetMousePosY();

			// Get available size in panel
			ImVec2 panelSize = ImGui::GetContentRegionAvail();

			// Set target aspect ratio
			constexpr float targetAspect = 16.0f / 9.0f;

			// Calculate the largest size that fits in the panel while maintaining aspect ratio
			float panelAspect = panelSize.x / panelSize.y;
			ImVec2 imageSize;
			if (panelAspect > targetAspect)
			{
				imageSize.y = panelSize.y;
				imageSize.x = panelSize.y * targetAspect;
			}
			else
			{
				imageSize.x = panelSize.x;
				imageSize.y = panelSize.x / targetAspect;
			}

			// Resizing of framebuffer
			ResizeFramebuffer(imageSize);

			// Render scene to framebuffer using editor camera
			RenderSceneToFramebuffer();

			// Display the framebuffer texture in the panel
			if (m_colorTex && imageSize.x > 0 && imageSize.y > 0)
			{
				// Center the image in the panel
				panelSize = ImGui::GetContentRegionAvail();
				ImVec2 cursorPos = ImGui::GetCursorPos();
				ImVec2 offset = ImVec2((panelSize.x - imageSize.x) * 0.5f, (panelSize.y - imageSize.y) * 0.5f);
				ImGui::SetCursorPos(ImVec2(cursorPos.x + offset.x, cursorPos.y + offset.y));
				// Draw the scene texture
				ImGui::Image((ImTextureID)m_colorTex, imageSize, ImVec2(0, 1), ImVec2(1, 0));
				ImVec2 imageTopLeft = ImGui::GetItemRectMin();

				auto& cam = m_coord.GetComponent<Camera>(m_editorCameraEntity);
				auto& camT = m_coord.GetComponent<Transform>(m_editorCameraEntity);

				float halfWidth = (cam.width * 0.5f) / cam.zoom;
				float halfHeight = (cam.height * 0.5f) / cam.zoom;

				float worldLeft = camT.pos.x - halfWidth;
				float worldRight = camT.pos.x + halfWidth;
				float worldBottom = camT.pos.y - halfHeight;
				float worldTop = camT.pos.y + halfHeight;

				// Initialize game boundary box
				if (!m_boundaryInitialized)
				{
					m_worldLeft = worldLeft;
					m_worldRight = worldRight;
					m_worldBottom = worldBottom;
					m_worldTop = worldTop;
					m_boundaryInitialized = true;
				}

				ImVec2 boundaryTopLeft = ScenePanel::WorldToScreen(m_worldLeft, m_worldTop, imageTopLeft, imageSize, worldLeft, worldRight, worldBottom, worldTop);
				ImVec2 boundaryTopRight = ScenePanel::WorldToScreen(m_worldRight, m_worldTop, imageTopLeft, imageSize, worldLeft, worldRight, worldBottom, worldTop);
				ImVec2 boundaryBottomRight = ScenePanel::WorldToScreen(m_worldRight, m_worldBottom, imageTopLeft, imageSize, worldLeft, worldRight, worldBottom, worldTop);
				ImVec2 boundaryBottomLeft = ScenePanel::WorldToScreen(m_worldLeft, m_worldBottom, imageTopLeft, imageSize, worldLeft, worldRight, worldBottom, worldTop);

				ImDrawList* drawList = ImGui::GetWindowDrawList();
				drawList->AddLine(boundaryTopLeft, boundaryTopRight, IM_COL32(255, 255, 255, 255), 2.0f);
				drawList->AddLine(boundaryTopRight, boundaryBottomRight, IM_COL32(255, 255, 255, 255), 2.0f);
				drawList->AddLine(boundaryBottomRight, boundaryBottomLeft, IM_COL32(255, 255, 255, 255), 2.0f);
				drawList->AddLine(boundaryBottomLeft, boundaryTopLeft, IM_COL32(255, 255, 255, 255), 2.0f);

				if (m_tilemap.tileOverlayEnabled)
				{
					DrawTilemap(imageTopLeft, imageSize);
				}

				CalculateMouseWorldPosition(imageTopLeft, imageSize, mouseWorldX, mouseWorldY);

				// Displays mouse world position
				DrawMouseInfo(mouseWorldX, mouseWorldY, imageTopLeft);

				if (ImGui::IsItemHovered() && ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
				{
					ImGuiIO& io = ImGui::GetIO();
					auto& camMod = m_coord.ModifyComponent<Camera>(m_editorCameraEntity);

					// Camera Zoom
					float scroll = io.MouseWheel;
					if (scroll != 0.0f)
					{
						camMod.zoom = std::clamp(camMod.zoom * (scroll > 0 ? 1.1f : 0.9f), 0.05f, 10.0f);
						m_editorCameraData.zoom = camMod.zoom;
					}

					// Camera Pan
					if (ImGui::IsMouseDragging(ImGuiMouseButton_Right))
					{
						ImVec2 dragDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
						auto& camTransform = m_coord.ModifyComponent<Transform>(m_editorCameraEntity);
						constexpr float panSpeed = 1.65f;
						camTransform.pos.x -= dragDelta.x * (panSpeed / camMod.zoom);
						camTransform.pos.y += dragDelta.y * (panSpeed / camMod.zoom);
						m_editorCameraData.pos = camTransform.pos;
						ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);
					}

					// Multi-Selection rectangle
					/*if (currentTool == SceneTool::MultiSelect)
					{
						ImVec2 imageTopLeft = ImGui::GetItemRectMin();
						ImVec2 imageSize = ImGui::GetItemRectSize();

						// Start selection
						if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
						{
							m_isMultiSelecting = true;
							m_selectStart = ImGui::GetMousePos();
							m_selectEnd = m_selectStart;
						}
						// Update selection rectangle
						if (m_isMultiSelecting && ImGui::IsMouseDown(ImGuiMouseButton_Left))
						{
							m_selectEnd = ImGui::GetMousePos();
							ImDrawList* drawList = ImGui::GetWindowDrawList();
							drawList->AddRect(m_selectStart, m_selectEnd, IM_COL32(0, 255, 0, 128), 0.0f, 0, 2.0f);
						}
						// Finish selection
						if (m_isMultiSelecting && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
						{
							m_multiSelectedEntities.clear();
							ImVec2 min = ImVec2(std::min(m_selectStart.x, m_selectEnd.x), std::min(m_selectStart.y, m_selectEnd.y));
							ImVec2 max = ImVec2(std::max(m_selectStart.x, m_selectEnd.x), std::max(m_selectStart.y, m_selectEnd.y));
							for (Entity e : m_coord.GetLivingEntities())
							{
								if (!m_coord.HasComponent<Transform>(e)) { continue; }
								auto& t = m_coord.GetComponent<Transform>(e);
								ImVec2 screenPos = WorldToScreen(t.pos.x, t.pos.y, imageTopLeft, imageSize);
								if (screenPos.x >= min.x && screenPos.x <= max.x && screenPos.y >= min.y && screenPos.y <= max.y)
								{
									m_multiSelectedEntities.insert(e);
								}
							}
							m_isMultiSelecting = false;
							currentTool = SceneTool::Select;
						}
					}

					// Delete selected entities
					if (!m_multiSelectedEntities.empty() && ImGui::IsKeyPressed(ImGuiKey_Delete))
					{
						for (Entity e : m_multiSelectedEntities)
						{
							m_coord.DestroyEntity(e);
						}
						m_multiSelectedEntities.clear();
					}*/
					if (m_multiSelectedEntities.empty() && m_selectedEntity && *m_selectedEntity != INVALID_ENTITY && ImGui::IsKeyPressed(ImGuiKey_Delete))
					{
						m_coord.DestroyEntity(*m_selectedEntity);
						*m_selectedEntity = INVALID_ENTITY;
					}

					// Entity Picking
					if (!m_isDragging && currentTool == SceneTool::Select)
					{
						HandleEntityPicking(mouseWorldX, mouseWorldY);

						if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !m_isDragging && m_hoveredEntity != INVALID_ENTITY) {
							*m_selectedEntity = m_hoveredEntity;
							auto& t = m_coord.GetComponent<Transform>(m_hoveredEntity);
							m_dragOffset.x = t.pos.x - mouseWorldX;
							m_dragOffset.y = t.pos.y - mouseWorldY;
							m_isDragging = true;

							if (m_coord.HasComponent<RigidBody>(m_hoveredEntity))
							{
								auto& rb = m_coord.ModifyComponent<RigidBody>(m_hoveredEntity);
								m_gravityBackup[m_hoveredEntity] = rb.useGravity;
								rb.useGravity = false;
							}
						}
					}

					// Entity Dragging
					if (m_isDragging && m_hoveredEntity != INVALID_ENTITY)
					{
						if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
						{
							HandleEntityDragging(mouseWorldX, mouseWorldY);
						}
						else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
						{
							m_isDragging = false;
							if (m_coord.HasComponent<RigidBody>(m_hoveredEntity))
							{
								auto& rb = m_coord.ModifyComponent<RigidBody>(m_hoveredEntity);
								rb.useGravity = m_gravityBackup[m_hoveredEntity];
								m_gravityBackup.erase(m_hoveredEntity);
							}
						}
					}

					// Tile Painting
					if (currentTool == SceneTool::Paint && ImGui::IsMouseDown(ImGuiMouseButton_Left))
					{
						// Calculate grid position
						float grid = m_tilemap.gridSize;
						int gridX = static_cast<int>(std::floor(mouseWorldX / grid));
						int gridY = static_cast<int>(std::floor(mouseWorldY / grid));

						if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
						{
							m_isPainting = true;
							m_paintPath.clear();
							auto tilePos = std::make_pair(gridX, gridY);

							// Determine whether to create or delete tile based on existing tile at grid
							PaintAction action = (m_paintedTiles.find(tilePos) == m_paintedTiles.end()) ? PaintAction::Create : PaintAction::Delete;
							m_paintAction = action;
							m_paintPath.push_back(tilePos);
							PaintTile(gridX, gridY, action);
						}
						else if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && m_isPainting)
						{
							// Continue painting only if new tile position
							auto tilePos = std::make_pair(gridX, gridY);
							if (std::find(m_paintPath.begin(), m_paintPath.end(), tilePos) == m_paintPath.end())
							{
								m_paintPath.push_back(tilePos);
								PaintTile(gridX, gridY, m_paintAction);
							}
						}
						if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
						{
							// Stop painting
							m_isPainting = false;
							m_paintPath.clear();
						}
					}
				}

				// drag and drop image
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PREFAB")) {
						const char* droppedPath = static_cast<const char*>(payload->Data);

						// Dropped path: ../assets\GameData\Prefabs\Ghost.json
						std::cout << "Dropped path: " << droppedPath << "\n";

						// Get the Prefab name from the path, which is 'Ghost' in this case
						std::filesystem::path filePath(droppedPath);

						std::cout << "Creating entity from prefab: " << filePath.stem().string() << "\n";

						Transform trans{};

						trans.pos.x = mouseWorldX + m_dragOffset.x;
						trans.pos.y = mouseWorldY + m_dragOffset.y;
						trans.pos.z = 0.f;
						// default layer...
						trans.layer = LayerType::GAMELAYER;

						trans.scale = Vec2{ 50.f, 50.f };	// default scale...

						m_coord.GetSystem<EntityFactory>()->Create(filePath.stem().string(), trans);

						std::cout << "Transform: " << trans.pos.x << ", " << trans.pos.y << std::endl;

					}
				}

				ImGui::EndDragDropTarget();
			}
			// Draw tool bar for mouse tool selection
			DrawToolBar();

			ImGui::End();
		}

		/************************************************************************/
		/*!
		\brief
			Resets and deletes the scene texture.
		*/
		/************************************************************************/
		void ResetTexture();

		void CreateEditorCamera();

		/************************************************************************/
		/*!
		\brief
			Destroys the editor camera entity.
		*/
		/************************************************************************/
		void DestroyEditorCamera();

	private:
		GLuint m_sceneTexture = 0;
		Coordinator& m_coord;

		// Selected entity in the scene
		std::shared_ptr<Entity> m_selectedEntity;
		std::set<Entity> m_multiSelectedEntities;
		bool m_isMultiSelecting = false;
		ImVec2 m_selectStart, m_selectEnd;

		Entity m_backgroundEntity;      // Current background entity ID

		// Camera entity and saved camera data
		Entity m_editorCameraEntity = INVALID_ENTITY;
		DefaultEditorCamera m_editorCameraData;

		// Dragging and picking state
		Entity m_hoveredEntity = INVALID_ENTITY;
		Vec3   m_dragOffset = { 0, 0, 0 };
		bool   m_isDragging = false;
		std::unordered_map<Entity, bool> m_gravityBackup;

		// Framebuffer objects and attachments
		GLuint m_fbo = 0, m_colorTex = 0, m_rbo = 0;
		int m_width = 1600, m_height = 900;
		float m_worldLeft = 0.0f;
		float m_worldRight = 0.0f;
		float m_worldBottom = 0.0f;
		float m_worldTop = 0.0f;
		bool m_boundaryInitialized = false;

		// Current scene tool
		SceneTool currentTool = SceneTool::Select;

		// Tilemap for object snapping and tile painting
		Tilemap& m_tilemap;
		std::map<std::pair<int, int>, std::pair<int, Entity>> m_paintedTiles;

		bool m_isPainting = false;
		std::vector<std::pair<int, int>> m_paintPath;
		enum class PaintAction { None, Create, Delete };
		PaintAction m_paintAction = PaintAction::None;

		/************************************************************************/
		/*!
		\brief
			Creates or resizes the scene texture.

		\param[in] width
			- Width of the texture.

		\param[in] height
			- Height of the texture.
		*/
		/************************************************************************/
		void CreateOrResizeTexture(int width, int height);

		/************************************************************************/
		/*!
		\brief
			Updates the scene texture from the current framebuffer.

		\param[in] width
			- Width of the texture.

		\param[in] height
			- Height of the texture.
		*/
		/************************************************************************/
		void UpdateTextureFromFramebuffer(int width, int height) const;

		/************************************************************************/
		/*!
		\brief
			Creates the framebuffer and its attachments.

		\param[in] width
			- Width of the framebuffer.

		\param[in] height
			- Height of the framebuffer.
		*/
		/************************************************************************/
		void CreateFramebuffer(int width, int height);

		/************************************************************************/
		/*!
		\brief
			Resizes the framebuffer if current size differs from previous.

		\param[in] imageSize
			- New size of the framebuffer.
		*/
		/************************************************************************/
		void ResizeFramebuffer(const ImVec2& imageSize);

		/************************************************************************/
		/*!
		\brief
			Cleans up the framebuffer and its attachments.
		*/
		/************************************************************************/
		void CleanupFramebuffer();

		/************************************************************************/
		/*!
		\brief
			Renders the scene to the framebuffer.
		*/
		/************************************************************************/
		void RenderSceneToFramebuffer();

		/************************************************************************/
		/*!
		\brief
			Calculates the world position of the mouse based on the image's
			position and size.

		\param[in] imageTopLeft
			- Top-left corner of the image in world space.

		\param[in] imageSize
			- Size of the image in world space.

		\param[out] mouseWorldX
			- Output mouse X coordinate in world space.

		\param[out] mouseWorldY
			- Output mouse Y coordinate in world space.
		*/
		/************************************************************************/
		void CalculateMouseWorldPosition(const ImVec2& imageTopLeft, const ImVec2& imageSize, float& mouseWorldX, float& mouseWorldY);

		/************************************************************************/
		/*!
		\brief
			Calculates the world position of the mouse based on the image's
			position and size.

		\param[in] wx
			- World X coordinate.

		\param[in] wy
			- World Y coordinate.

		\param[in] imageTopLeft
			- Top-left corner of the image in world space.

		\param[in] imageSize
			- Size of the image in world space.

		\param[in] worldLeft
			- Left boundary of the world space.

		\param[in] worldRight
			- Right boundary of the world space.

		\param[in] worldBottom
			- Bottom boundary of the world space.

		\param[in] worldTop
			- Top boundary of the world space.
		*/
		/************************************************************************/
		static ImVec2 WorldToScreen(float wx, float wy, const ImVec2& imageTopLeft, const ImVec2& imageSize,
			float worldLeft, float worldRight, float worldBottom, float worldTop);

		/************************************************************************/
		/*!
		\brief
			Draws the outline of the selected entity.
		*/
		/************************************************************************/
		void DrawEntityOutline();

		/************************************************************************/
		/*!
		\brief
			Handles the picking of entities in the scene.

		\param[in] mouseWorldX
			- Mouse X coordinate in world space.

		\param[in] mouseWorldY
			- Mouse Y coordinate in world space.
		*/
		/************************************************************************/
		void HandleEntityPicking(float mouseWorldX, float mouseWorldY);

		/************************************************************************/
		/*!
		\brief
			Handles the dragging of entities in the scene.

		\param[in] mouseWorldX
			- Mouse X coordinate in world space.

		\param[in] mouseWorldY
			- Mouse Y coordinate in world space.
		*/
		/************************************************************************/
		void HandleEntityDragging(float mouseWorldX, float mouseWorldY);

		/************************************************************************/
		/*!
		\brief
			Creates an entity in the scene from the given asset path at the
			specified mouse position.

		\param[in] assetPath
			- Path to the image asset.

		\param[in] mouseX
			- mouse X coordinate in world space.

		\param[in] mouseY
			- mouse Y coordinate in world space.
		*/
		/************************************************************************/
		void CreateEntityFromAsset(const std::string& assetPath, float mouseX, float mouseY);

		/************************************************************************/
		/*!
		\brief
			Draws the tool bar for selecting mouse tools.
		*/
		/************************************************************************/
		void DrawToolBar();

		/************************************************************************/
		/*!
		\brief
			Draws scene information such as mouse position and background entity info.

		\param[in] mouseX
			- Mouse X coordinate in world space.

		\param[in] mouseY
			- Mouse Y coordinate in world space.

		\param[in] imageTopLeft
			- Top-left corner of the image in world space.
		*/
		/************************************************************************/
		void DrawMouseInfo(float mouseX, float mouseY, ImVec2 imageTopLeft);

		/************************************************************************/
		/*!
		\brief
			Draws a tilemap overlay on the scene.

		\param[in] imageTopLeft
			- Top-left corner of the image in world space.

		\param[in] imageSize
			- Size of the image in world space.
		*/
		/************************************************************************/
		void DrawTilemap(const ImVec2& imageTopLeft, const ImVec2& imageSize);

		/************************************************************************/
		/*!
		\brief
			Paints a tile at the specified grid coordinates.

		\param[in] mouseX
			- Mouse X coordinate in world space.

		\param[in] mouseY
			- Mouse Y coordinate in world space.
		*/
		/************************************************************************/
		void PaintTile(int gridX, int gridY, PaintAction action);

	};

	//-----------------------------------------------//
	//--------------- Public Methods ----------------//
	//-----------------------------------------------//

	// Reset and delete the scene texture.
	inline void ScenePanel::ResetTexture()
	{
		if (m_sceneTexture)
		{
			glDeleteTextures(1, &m_sceneTexture);
			m_sceneTexture = 0;
		}
	}

	// Create the editor camera entity.
	inline void ScenePanel::CreateEditorCamera()
	{
		// Create a dedicated editor camera entity for Scene
		auto factory = m_coord.GetSystem<EntityFactory>();
		if (factory)
		{
			Transform editorTransform;
			editorTransform.pos = m_editorCameraData.pos;
			editorTransform.rot = m_editorCameraData.rot;
			editorTransform.scale = m_editorCameraData.scale;
			editorTransform.layer = LayerType::GAMEUI;
			m_editorCameraEntity = factory->Create("Cam", editorTransform);

			if (m_editorCameraEntity != INVALID_ENTITY)
			{
				if (!m_coord.HasComponent<Camera>(m_editorCameraEntity))
				{
					m_coord.AddComponent<Camera>(m_editorCameraEntity, Camera{});
				}
				auto& cam = m_coord.ModifyComponent<Camera>(m_editorCameraEntity);
				cam.zoom = m_editorCameraData.zoom;
				cam.isMainCam = false;
			}
		}
	}

	// Destroy the editor camera entity.
	inline void ScenePanel::DestroyEditorCamera()
	{
		// Remove editor camera entity
		if (m_editorCameraEntity != INVALID_ENTITY &&
			m_coord.GetLivingEntities().count(m_editorCameraEntity))
		{
			m_coord.DestroyEntity(m_editorCameraEntity);
			m_editorCameraEntity = INVALID_ENTITY;
		}
	}


	//-----------------------------------------------//
	//--------------- Private Methods ---------------//
	//-----------------------------------------------//

	// Create or resize the scene texture.
	inline void ScenePanel::CreateOrResizeTexture(int width, int height)
	{
		if (m_sceneTexture) glDeleteTextures(1, &m_sceneTexture);
		glGenTextures(1, &m_sceneTexture);
		glBindTexture(GL_TEXTURE_2D, m_sceneTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	// Update the scene texture from the current framebuffer.
	inline void ScenePanel::UpdateTextureFromFramebuffer(int width, int height) const
	{
		glBindTexture(GL_TEXTURE_2D, m_sceneTexture);
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width, height, 0);
	}

	// Create the framebuffer and its attachments.
	inline void ScenePanel::CreateFramebuffer(int width, int height)
	{
		m_width = width;
		m_height = height;

		glGenFramebuffers(1, &m_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

		glGenTextures(1, &m_colorTex);
		glBindTexture(GL_TEXTURE_2D, m_colorTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTex, 0);

		glGenRenderbuffers(1, &m_rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			printf("[ScenePanel] Framebuffer not complete!\n");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// Resize the framebuffer if current size differs from previous.
	inline void ScenePanel::ResizeFramebuffer(const ImVec2& imageSize)
	{
		if ((int)imageSize.x != m_width || (int)imageSize.y != m_height)
		{
			CleanupFramebuffer();
			CreateFramebuffer((int)imageSize.x, (int)imageSize.y);
		}
	}

	// Clean up the framebuffer and its attachments.
	inline void ScenePanel::CleanupFramebuffer()
	{
		if (m_fbo) glDeleteFramebuffers(1, &m_fbo);
		if (m_colorTex) glDeleteTextures(1, &m_colorTex);
		if (m_rbo) glDeleteRenderbuffers(1, &m_rbo);
		m_fbo = m_colorTex = m_rbo = 0;
	}

	// Render the scene to the framebuffer.
	inline void ScenePanel::RenderSceneToFramebuffer()
	{
		// Render scene to framebuffer using editor camera
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glViewport(0, 0, m_width, m_height);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render with editor camera
		auto graphics = m_coord.GetSystem<GraphicsSystem>();
		if (graphics) {
			graphics->RenderWithCamera(m_editorCameraEntity);
		}
		auto textSystem = m_coord.GetSystem<TextSystem>();
		if (textSystem) {
			textSystem->Draw(m_editorCameraEntity, false);
		}

		// Draw hovered entity outline
		DrawEntityOutline();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// Calculate mouse world position for the editor camera.
	inline void ScenePanel::CalculateMouseWorldPosition(const ImVec2& imageTopLeft, const ImVec2& imageSize, float& mouseWorldX, float& mouseWorldY)
	{
		ImVec2 mousePosInPanel = ImGui::GetMousePos();
		ImVec2 mousePosInImage = ImVec2(mousePosInPanel.x - imageTopLeft.x, mousePosInPanel.y - imageTopLeft.y);

		float ndcX = (mousePosInImage.x / imageSize.x) * 2.0f - 1.0f;
		float ndcY = 1.0f - (mousePosInImage.y / imageSize.y) * 2.0f;

		auto& camInfo = m_coord.GetComponent<Camera>(m_editorCameraEntity);
		auto& camInfoTransform = m_coord.GetComponent<Transform>(m_editorCameraEntity);
		float halfWidth = (camInfo.width * 0.5f) / camInfo.zoom;
		float halfHeight = (camInfo.height * 0.5f) / camInfo.zoom;

		mouseWorldX = camInfoTransform.pos.x + ndcX * halfWidth;
		mouseWorldY = camInfoTransform.pos.y + ndcY * halfHeight;
	}

	// Convert world coordinates to screen coordinates.
	inline ImVec2 ScenePanel::WorldToScreen(float wx, float wy, const ImVec2& imageTopLeft, const ImVec2& imageSize,
		float worldLeft, float worldRight, float worldBottom, float worldTop)
	{
		float u = (wx - worldLeft) / (worldRight - worldLeft);
		float v = 1.0f - (wy - worldBottom) / (worldTop - worldBottom);
		return ImVec2(imageTopLeft.x + u * imageSize.x, imageTopLeft.y + v * imageSize.y);
	}

	// Draw the outline of the selected entity.
	inline void ScenePanel::DrawEntityOutline()
	{
		auto& camInfo = m_coord.GetComponent<Camera>(m_editorCameraEntity);
		if (m_hoveredEntity != INVALID_ENTITY)
		{
			auto graphics = m_coord.GetSystem<GraphicsSystem>();
			if (graphics && m_coord.HasComponent<Transform>(m_hoveredEntity) && m_coord.HasComponent<Renderable>(m_hoveredEntity))
			{
				// Get components
				auto& t = m_coord.GetComponent<Transform>(m_hoveredEntity);
				auto& r = m_coord.GetComponent<Renderable>(m_hoveredEntity);

				// Draw wireframe outline
				Shader* shader = graphics->GetShader("debug");
				Mesh* mesh = graphics->GetMesh(r.meshName);
				if (shader && mesh)
				{
					// Calculate MVP matrix
					auto& camTransform = m_coord.GetComponent<Transform>(m_editorCameraEntity);
					glm::mat4 proj = graphics->GetCameraProjectionMatrix(camInfo.width, camInfo.height, camInfo.zoom);
					glm::mat4 view = glm::translate(glm::mat4(1.f), glm::vec3(-camTransform.pos.x, -camTransform.pos.y, -camTransform.pos.z));
					glm::mat4 model = graphics->GetModelToWorldMatrix(t.pos, t.rot, t.scale);

					// Draw mesh in wireframe
					shader->Use();
					shader->SetMat4("uDebugMVP", proj * view * model);
					shader->SetVec3("uEntityColor", glm::vec3(0.f, 1.f, 0.f)); // Green

					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					mesh->Draw();
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}
			}
		}
	}

	// Handle the picking of entities in the scene.
	inline void ScenePanel::HandleEntityPicking(float mouseWorldX, float mouseWorldY)
	{
		Entity topEntity = INVALID_ENTITY;
		float topZ = -1e9f;
		for (Entity e : m_coord.GetLivingEntities())
		{
			if (!m_coord.HasComponent<Transform>(e) || !m_coord.HasComponent<Renderable>(e))
				continue;

			auto& t = m_coord.GetComponent<Transform>(e);
			float halfW = t.scale.x * 0.5f;
			float halfH = t.scale.y * 0.5f;
			if (mouseWorldX >= t.pos.x - halfW && mouseWorldX <= t.pos.x + halfW &&
				mouseWorldY >= t.pos.y - halfH && mouseWorldY <= t.pos.y + halfH)
			{
				float z = t.pos.z;
				if (z > topZ)
				{
					topEntity = e;
					topZ = z;
				}
			}
		}
		m_hoveredEntity = topEntity;
	}

	// Handle the dragging of entities in the scene.
	inline void ScenePanel::HandleEntityDragging(float mouseWorldX, float mouseWorldY)
	{
		if (m_isDragging && m_hoveredEntity != INVALID_ENTITY)
		{
			// Update entity position while dragging
			if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
			{
				auto& t = m_coord.ModifyComponent<Transform>(m_hoveredEntity);
				float grid = m_tilemap.gridSize;
				float newX = mouseWorldX + m_dragOffset.x;
				float newY = mouseWorldY + m_dragOffset.y;

				// Snap to grid cell center if tilemap overlay is enabled
				if (m_tilemap.tileOverlayEnabled)
				{
					newX = std::floor(newX / grid) * grid + grid * 0.5f;
					newY = std::floor(newY / grid) * grid + grid * 0.5f;
				}

				t.pos.x = newX;
				t.pos.y = newY;
			}
			else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				m_isDragging = false;

				// Restore gravity if entity has RigidBody
				if (m_coord.HasComponent<RigidBody>(m_hoveredEntity))
				{
					auto& rb = m_coord.ModifyComponent<RigidBody>(m_hoveredEntity);
					rb.useGravity = m_gravityBackup[m_hoveredEntity];
					m_gravityBackup.erase(m_hoveredEntity);
				}
			}
		}
	}

	// Create an entity in the scene from the given asset path at the specified mouse position.
	inline void ScenePanel::CreateEntityFromAsset(const std::string& assetPath, float mouseX, float mouseY)
	{
		// load image through ResourceManager
		auto resourceMgr = m_coord.GetSystem<ResourceManager>();
		if (!resourceMgr)
		{
			std::cerr << "[Scene] ResourceManager not available!\n";
			return;
		}

		size_t imageKey = 0;
		LOAD_RESULT result = resourceMgr->LoadImage(assetPath, imageKey);

		// Check if loading succeeded
		if (result == LOAD_FAIL_BADPATH)
		{
			std::cerr << "[Scene] Failed to load image: " << assetPath << "\n";
			return;
		}

		std::cout << "[Scene] Loaded image with key: " << imageKey << "\n";

		// remove old background if exists
		if (m_backgroundEntity != INVALID_ENTITY)
		{
			m_coord.DestroyEntity(m_backgroundEntity);
			std::cout << "[Scene] Removed old background entity\n";
		}

		// create new entity
		m_backgroundEntity = m_coord.CreateEntity();
		std::cout << "[Scene] Created entity: " << m_backgroundEntity << "\n";

		// add Transform component (position, rotation, scale)
		// Position at mouse location in world space
		Transform transform;
		transform.pos = Vec3{ mouseX, mouseY, -10.0f }; // z = -10 puts it behind
		transform.rot = 0.0f;
		transform.scale = Vec2{ 800.0f, 600.0f }; // Default size (adjust as needed)
		m_coord.AddComponent(m_backgroundEntity, transform);

		// STEP 5: Add Renderable component (makes it visible)
		Renderable renderable;
		renderable.meshName = "quad";               // Use quad mesh
		renderable.materialName = "background_mat"; // Your material name
		renderable.isWireframe = false;
		renderable.wasModified = true;              // Flag for graphics system
		m_coord.AddComponent(m_backgroundEntity, renderable);


		std::cout << "[Scene] Background created at (" << mouseX << ", " << mouseY << ")\n";
	}

	// Draw the tool bar for selecting mouse tools.
	inline void ScenePanel::DrawToolBar()
	{
		ImVec2 imageTopLeft = ImGui::GetItemRectMin();
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		// Tool bar overlay position
		ImVec2 toolBarPos = ImVec2(imageTopLeft.x + 12, imageTopLeft.y + 12);
		float buttonWidth[4] = { 60.0f, 60.0f, 90.0f, 60.0f };
		const char* toolNames[4] = { "View", "Select", "MultiSelect", "Paint" };
		SceneTool toolEnums[4] = { SceneTool::PanZoom, SceneTool::Select, SceneTool::MultiSelect, SceneTool::Paint };

		// Draw background
		float totalWidth = buttonWidth[0] + buttonWidth[1] + buttonWidth[2] + buttonWidth[3] + 30.0f;
		drawList->AddRectFilled(
			toolBarPos,
			ImVec2(toolBarPos.x + totalWidth, toolBarPos.y + 32),
			IM_COL32(32, 32, 32, 180), 4.0f);

		// Draw and handle buttons
		float x = toolBarPos.x + 8;
		for (int i = 0; i < 4; ++i)
		{
			ImVec2 btnPos = ImVec2(x, toolBarPos.y + 4);
			ImGui::SetCursorScreenPos(btnPos);
			ImGui::PushID(i);
			if (ImGui::InvisibleButton(toolNames[i], ImVec2(buttonWidth[i], 24.0f)))
			{
				currentTool = toolEnums[i];
			}

			// Highlight selected tool
			ImU32 color = (currentTool == toolEnums[i]) ? IM_COL32(100, 180, 255, 255) : IM_COL32(200, 200, 200, 255);
			drawList->AddRectFilled(btnPos, ImVec2(btnPos.x + buttonWidth[i], btnPos.y + 24.0f), color, 4.0f);
			drawList->AddText(ImVec2(btnPos.x + 8, btnPos.y + 4), IM_COL32(32, 32, 32, 255), toolNames[i]);
			ImGui::PopID();
			x += buttonWidth[i] + 6.0f;
		}
	}

	// Draw scene information such as mouse position and background entity info.
	inline void ScenePanel::DrawMouseInfo(float mouseX, float mouseY, ImVec2 imageTopLeft)
	{
		// Display mouse info at top left corner
		ImVec2 imageBottomLeft = ImVec2(imageTopLeft.x + 12, imageTopLeft.y + ImGui::GetItemRectSize().y - 32);

		char mouseInfo[64];
		snprintf(mouseInfo, sizeof(mouseInfo), "Mouse World: (%.1f, %.1f)", mouseX, mouseY);

		// Draw the info with a background for readability
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->AddRectFilled(
			ImVec2(imageBottomLeft.x - 4, imageBottomLeft.y - 4),
			ImVec2(imageBottomLeft.x + 200, imageBottomLeft.y + 22),
			IM_COL32(32, 32, 32, 180), 4.0f);

		drawList->AddText(imageBottomLeft, IM_COL32(255, 255, 255, 255), mouseInfo);

	}

	// Draw the tilemap overlay on the scene panel.
	inline void ScenePanel::DrawTilemap(const ImVec2& imageTopLeft, const ImVec2& imageSize)
	{
		auto& cam = m_coord.GetComponent<Camera>(m_editorCameraEntity);
		auto& camTransform = m_coord.GetComponent<Transform>(m_editorCameraEntity);

		float halfWidth = (cam.width * 0.5f) / cam.zoom;
		float halfHeight = (cam.height * 0.5f) / cam.zoom;

		float worldLeft = camTransform.pos.x - halfWidth;
		float worldRight = camTransform.pos.x + halfWidth;
		float worldBottom = camTransform.pos.y - halfHeight;
		float worldTop = camTransform.pos.y + halfHeight;

		m_tilemap.DrawTilemapOverlay(imageTopLeft, imageSize, worldLeft, worldRight, worldBottom, worldTop);
	}

	// Create/Delete a tile at the specified grid coordinates
	inline void ScenePanel::PaintTile(int gridX, int gridY, PaintAction action)
	{
		auto tilePos = std::make_pair(gridX, gridY);
		int currentTile = m_tilemap.selectedTile;
		auto factory = m_coord.GetSystem<EntityFactory>();
		if (!factory) { return; }

		auto it = m_paintedTiles.find(tilePos);

		// Deletes the tile at the specified position if action is Delete
		if (action == PaintAction::Delete)
		{
			if (it != m_paintedTiles.end())
			{
				Entity toDelete = it->second.second;
				if (toDelete != INVALID_ENTITY)
				{
					m_coord.DestroyEntity(toDelete);
				}
				m_paintedTiles.erase(it);
			}
			return;
		}

		// Creates or replaces the tile at the specified position if action is Create
		if (it != m_paintedTiles.end()) {
			// If the same tile is being painted, do nothing
			if (it->second.first == currentTile) { return; }

			// Replace tile if different
			Entity toDelete = it->second.second;
			if (toDelete != INVALID_ENTITY)
			{
				m_coord.DestroyEntity(toDelete);
			}
			m_paintedTiles.erase(it);
		}

		// Snap position to grid
		float grid = m_tilemap.gridSize;
		Vec3 pos{ gridX * grid + grid * 0.5f, gridY * grid + grid * 0.5f, 0.0f };
		Vec2 scale{ grid, grid };

		// Create tile entity
		Entity tileEntity = m_coord.CreateEntity();

		// Add components to tile entity
		Transform t;
		t.pos = pos;
		t.scale = scale;
		t.rot = 0.0f;
		t.layer = LayerType::GAMELAYER;
		m_coord.AddComponent(tileEntity, t);

		Renderable r;
		r.meshName = "square";
		r.materialName = "tile_mat_" + std::to_string(tileEntity);
		r.isWireframe = false;
		r.wasModified = true;
		m_coord.AddComponent(tileEntity, r);

		RigidBody rb;
		rb.invMass = 1.f;
		rb.useGravity = false;
		m_coord.AddComponent(tileEntity, rb);

		Collider c;
		c.shape = ShapeType::AABB;
		c.type = ColliderType::Ground;
		c.width = grid;
		c.height = grid;
		m_coord.AddComponent(tileEntity, c);

		// Update material texture of tile
		auto graphics = m_coord.GetSystem<GraphicsSystem>();
		if (graphics)
		{
			graphics->UpdateTextureOfMaterial(m_tilemap.tileTexturePath, tileEntity);
		}

		// Save tile as prefab if not already existing
		std::string prefabName = "Tile_" + std::to_string(currentTile);
		factory->SaveAsPrefab(tileEntity, prefabName, true);

		// Record painted tile
		m_paintedTiles[tilePos] = std::make_pair(currentTile, tileEntity);
	}

} // NightSnack

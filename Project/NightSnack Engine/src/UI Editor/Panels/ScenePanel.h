/******************************************************************************
 * File:        ScenePanel.h
 * Project:     NightSnack
 * Author(s):	Lim Wei Jie (Primary, 95%)
 *              Bryan Lim Jun Jie (Secondary, 5%)
 *
 * Description:
 * Scene panel showing the rendered viewport.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once
#include "AllPanels.h"

namespace NightSnack
{
    class ScenePanel {
    public:
        ScenePanel(Coordinator& coord) : m_coord(coord) 
        {
			// Create a dedicated editor camera entity for Scene Panel
            m_editorCameraEntity = m_coord.CreateEntity();

            Transform editorTransform;
            editorTransform.pos = { 0.f, 0.f, 0.f };
            editorTransform.rot = 0.f;
            editorTransform.scale = { 1.f, 1.f };
            m_coord.AddComponent(m_editorCameraEntity, editorTransform);

            Camera editorCamera;
            editorCamera.isMainCam = false;
            editorCamera.zoom = 1.f;
            editorCamera.width = 1600.f;
            editorCamera.height = 900.f;
            m_coord.AddComponent(m_editorCameraEntity, editorCamera);

            // Create framebuffer and color/depth attachments
            CreateFramebuffer(1600, 900);
        }

        ~ScenePanel()
        {
            CleanupFramebuffer();
            if (m_sceneTexture)
            {
                glDeleteTextures(1, &m_sceneTexture);
            }
        }


        void Draw()
        {
            // Scene Panel
            ImGui::Begin("Scene");

            // ----------------------------------------------------------------
            // GET MOUSE POSITION - For positioning dropped assets
            // ----------------------------------------------------------------
            auto inputSystem = m_coord.GetSystem<InputSystem>();
            float mouseWorldX = inputSystem->GetMousePosX();
            float mouseWorldY = inputSystem->GetMousePosY();

            ImVec2 panelSize = ImGui::GetContentRegionAvail();

            // Set your target aspect ratio (e.g., 16:9)
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

            // Resize framebuffer if needed
            if ((int)imageSize.x != m_width || (int)imageSize.y != m_height) {
                CleanupFramebuffer();
                CreateFramebuffer((int)imageSize.x, (int)imageSize.y);
            }

            // Render scene to framebuffer using editor camera
            glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
            glViewport(0, 0, m_width, m_height);
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            auto graphics = m_coord.GetSystem<GraphicsSystem>();
            if (graphics) {
                graphics->RenderWithCamera(m_editorCameraEntity);
            }

            auto& camInfo = m_coord.GetComponent<Camera>(m_editorCameraEntity);
            if (m_hoveredEntity != INVALID_ENTITY)
            {
				// Draw outline for hovered entity
                auto graphics = m_coord.GetSystem<GraphicsSystem>();
                if (graphics && m_coord.HasComponent<Transform>(m_hoveredEntity) && m_coord.HasComponent<Renderable>(m_hoveredEntity))
                {
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
                        glm::mat4 model = graphics->GetModelToWorldMatrix(t.pos, t.rot, t.scale, r.renderLayer);

                        shader->Use();
                        shader->SetMat4("uDebugMVP", proj * view * model);
                        shader->SetVec3("uEntityColor", glm::vec3(0.f, 1.f, 0.f)); // Green

                        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                        mesh->Draw();
                        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    }
                }
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // Center the image in the panel
            ImVec2 cursorPos = ImGui::GetCursorPos();
            ImVec2 offset = ImVec2((panelSize.x - imageSize.x) * 0.5f, (panelSize.y - imageSize.y) * 0.5f);
            ImGui::SetCursorPos(ImVec2(cursorPos.x + offset.x, cursorPos.y + offset.y));

            if (m_colorTex && imageSize.x > 0 && imageSize.y > 0)
            {
                ImGui::Image((ImTextureID)m_colorTex, imageSize, ImVec2(0, 1), ImVec2(1, 0));

                // Calculate mouse world position for the editor camera
                ImVec2 imageTopLeft = ImGui::GetItemRectMin();
                ImVec2 mousePosInPanel = ImGui::GetMousePos();
                ImVec2 mousePosInImage = ImVec2(mousePosInPanel.x - imageTopLeft.x, mousePosInPanel.y - imageTopLeft.y);

				// Convert to Normalized Coordinates
                float ndcX = (mousePosInImage.x / imageSize.x) * 2.0f - 1.0f;
                float ndcY = 1.0f - (mousePosInImage.y / imageSize.y) * 2.0f;

				// Convert NDC to world coordinates
                auto& cam = m_coord.GetComponent<Camera>(m_editorCameraEntity);
                auto& camTransform = m_coord.GetComponent<Transform>(m_editorCameraEntity);
                float halfWidth = (cam.width * 0.5f) / cam.zoom;
                float halfHeight = (cam.height * 0.5f) / cam.zoom;

                // Calculate world position
                float mouseWorldX = camTransform.pos.x + ndcX * halfWidth;
                float mouseWorldY = camTransform.pos.y + ndcY * halfHeight;

				// Mouse Interaction within Scene Panel
                if (ImGui::IsItemHovered() && ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
                {
                    ImGuiIO& io = ImGui::GetIO();
                    auto& cam = m_coord.ModifyComponent<Camera>(m_editorCameraEntity);
                    // Camera Zoom
                    float scroll = io.MouseWheel;
                    if (scroll != 0.0f)
                    {
                        // Clamp scaling for smooth zoom
                        cam.zoom = std::clamp(cam.zoom * (scroll > 0 ? 1.1f : 0.9f), 0.05f, 10.0f);
                    }

                    // Camera Pan (Right Mouse Drag)
                    if (ImGui::IsMouseDragging(ImGuiMouseButton_Right))
                    {
                        ImVec2 dragDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
                        auto& camTransform = m_coord.ModifyComponent<Transform>(m_editorCameraEntity);

                        // Pan speed
						constexpr float panSpeed = 1.65f;
                        camTransform.pos.x -= dragDelta.x * (panSpeed / cam.zoom);
                        camTransform.pos.y += dragDelta.y * (panSpeed / cam.zoom);
						// Reset drag delta to avoid compounding movement
                        ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);
                    }


                    // Detect left mouse press and pick entity
                    if (!m_isDragging)
                    {
						// Find topmost entity under mouse
                        Entity topEntity = INVALID_ENTITY;
                        int topLayer = -1000;
                        float topZ = -1e9f;

						// Iterate through all entities to find which one is under the mouse
                        for (Entity e : m_coord.GetLivingEntities())
                        {
							// Only consider entities with Transform and Renderable components
                            if (!m_coord.HasComponent<Transform>(e) || !m_coord.HasComponent<Renderable>(e))
                                continue;

                            auto& t = m_coord.GetComponent<Transform>(e);
                            auto& r = m_coord.GetComponent<Renderable>(e);

							// AABB check
                            float halfW = t.scale.x * 0.5f;
                            float halfH = t.scale.y * 0.5f;
                            if (mouseWorldX >= t.pos.x - halfW && mouseWorldX <= t.pos.x + halfW &&
                                mouseWorldY >= t.pos.y - halfH && mouseWorldY <= t.pos.y + halfH)
                            {
								// Check if this entity is on top based on render layer and z-position
                                int layer = static_cast<int>(r.renderLayer);
                                float z = t.pos.z;
                                if (layer > topLayer || (layer == topLayer && z > topZ))
                                {
                                    topEntity = e;
                                    topLayer = layer;
                                    topZ = z;
                                }
                            }
                        }
						// Set hovered entity
                        m_hoveredEntity = topEntity;

						// Start dragging on left mouse click
                        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !m_isDragging && m_hoveredEntity != INVALID_ENTITY)
                        {
							// Calculate drag offset
                            auto& t = m_coord.GetComponent<Transform>(m_hoveredEntity);
                            m_dragOffset.x = t.pos.x - mouseWorldX;
                            m_dragOffset.y = t.pos.y - mouseWorldY;
                            m_isDragging = true;
                        }
                    }

					// Handle dragging of entity
                    if (m_isDragging && m_hoveredEntity != INVALID_ENTITY)
                    {
						// Update entity position while dragging
                        if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
                        {
                            auto& t = m_coord.ModifyComponent<Transform>(m_hoveredEntity);
                            t.pos.x = mouseWorldX + m_dragOffset.x;
                            t.pos.y = mouseWorldY + m_dragOffset.y;
                        }
                        else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                        {
                            m_isDragging = false;
                        }
                    }
                }

                // ================================================================
                // DRAG & DROP TARGET - Accept images dropped from Assets panel
                // THIS IS WHERE THE MAGIC HAPPENS!
                // ================================================================
                if (ImGui::BeginDragDropTarget())
                {
                    // ----------------------------------------------------------------
                    // VISUAL FEEDBACK - Green overlay when hovering with asset
                    // ----------------------------------------------------------------
                    ImGui::GetWindowDrawList()->AddRectFilled(
                        imageTopLeft,
                        ImVec2(imageTopLeft.x + imageSize.x, imageTopLeft.y + imageSize.y),
                        IM_COL32(100, 200, 100, 50) // Semi-transparent green
                    );

                    // ----------------------------------------------------------------
                    // ACCEPT PAYLOAD - Receive dropped image
                    // ----------------------------------------------------------------
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_IMAGE"))
                    {
                        // Extract file path from payload
                        const char* droppedPath = (const char*)payload->Data;
                        std::string assetPath(droppedPath);

                        std::cout << "[Scene] Dropped asset: " << assetPath << "\n";
                        std::cout << "[Scene] Mouse position: (" << mouseWorldX << ", " << mouseWorldY << ")\n";

                        // ----------------------------------------------------------------
                        // CREATE ENTITY FROM DROPPED ASSET
                        // This spawns the image in your game world
                        // ----------------------------------------------------------------
                        CreateEntityFromAsset(assetPath, mouseWorldX, mouseWorldY);
                    }

                    ImGui::EndDragDropTarget();
                }

                
                // ----------------------------------------------------------------
                // TOOLTIP - Show instructions when hovering
                // ----------------------------------------------------------------
                if (ImGui::IsItemHovered())
                {
                    if (m_backgroundEntity == INVALID_ENTITY)
                    {
                        ImGui::BeginTooltip();
                        ImGui::Text("Drag an image from Assets to add background");
                        ImGui::EndTooltip();
                    }
                }
            }
            // ----------------------------------------------------------------
            // INFO PANEL - Show current scene info
            // ----------------------------------------------------------------
            DrawSceneInfo(mouseWorldX, mouseWorldY);

            ImGui::End();
        }

        void ResetTexture()
        {
            if (m_sceneTexture)
            {
                glDeleteTextures(1, &m_sceneTexture);
                m_sceneTexture = 0;
            }
        }

    private:
        GLuint m_sceneTexture = 0;
        Coordinator& m_coord;
        Entity m_backgroundEntity;      // Current background entity ID
		Entity m_editorCameraEntity;

        Entity m_hoveredEntity = INVALID_ENTITY;
        Vec3   m_dragOffset = { 0, 0, 0 };
        bool   m_isDragging = false;

        Transform m_editorTransform;
        Camera m_editorCamera;

        GLuint m_fbo = 0, m_colorTex = 0, m_rbo = 0;
        int m_width = 1600, m_height = 900;

        // Create or resize the texture used for rendering the scene
        void CreateOrResizeTexture(int width, int height)
        {
            if (m_sceneTexture) glDeleteTextures(1, &m_sceneTexture);
            glGenTextures(1, &m_sceneTexture);
            glBindTexture(GL_TEXTURE_2D, m_sceneTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }

        // Update the texture with the current framebuffer content
        void UpdateTextureFromFramebuffer(int width, int height) const
        {
            glBindTexture(GL_TEXTURE_2D, m_sceneTexture);
            glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width, height, 0);
        }

		// Create framebuffer with color and depth attachments
        void CreateFramebuffer(int width, int height) {
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

		// Cleanup framebuffer resources
        void CleanupFramebuffer() {
            if (m_fbo) glDeleteFramebuffers(1, &m_fbo);
            if (m_colorTex) glDeleteTextures(1, &m_colorTex);
            if (m_rbo) glDeleteRenderbuffers(1, &m_rbo);
            m_fbo = m_colorTex = m_rbo = 0;
        }

        // ====================================================================
       // CREATE ENTITY FROM ASSET
       // Loads image and creates game entity at mouse position
       // ====================================================================
        void CreateEntityFromAsset(const std::string& assetPath, float mouseX, float mouseY)
        {
            // STEP 1: Load image through ResourceManager
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

            // STEP 2: Remove old background if exists
            // (You can remove this if you want multiple backgrounds)
            if (m_backgroundEntity != INVALID_ENTITY)
            {
                m_coord.DestroyEntity(m_backgroundEntity);
                std::cout << "[Scene] Removed old background entity\n";
            }

            // STEP 3: Create new entity
            m_backgroundEntity = m_coord.CreateEntity();
            std::cout << "[Scene] Created entity: " << m_backgroundEntity << "\n";

            // STEP 4: Add Transform component (position, rotation, scale)
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
            renderable.renderLayer = RenderLayer::BACKGROUND; // Render behind
            renderable.isWireframe = false;
            renderable.wasModified = true;              // Flag for graphics system
            m_coord.AddComponent(m_backgroundEntity, renderable);


            std::cout << "[Scene] Background created at (" << mouseX << ", " << mouseY << ")\n";
        }

        // Shows current scene information at bottom of panel
        void DrawSceneInfo(float mouseX, float mouseY)
        {
            ImGui::Separator();

            // Display mouse position
            ImGui::Text("Mouse World Position: (%.1f, %.1f)", mouseX, mouseY);

            // Display background entity info
            if (m_backgroundEntity != INVALID_ENTITY)
            {
                ImGui::Text("Background Entity: %u", m_backgroundEntity);

                // Show transform if exists
                if (m_coord.HasComponent<Transform>(m_backgroundEntity))
                {
                    auto& transform = m_coord.GetComponent<Transform>(m_backgroundEntity);
                    ImGui::Text("Position: (%.1f, %.1f, %.1f)",
                        transform.pos.x, transform.pos.y, transform.pos.z);
                    ImGui::Text("Scale: (%.1f, %.1f)",
                        transform.scale.x, transform.scale.y);
                }

                //// Button to remove background
                //if (ImGui::Button("Remove Background"))
                //{
                //    m_coord.DestroyEntity(m_backgroundEntity);
                //    m_backgroundEntity = INVALID_ENTITY;
                //    std::cout << "[Scene] Background removed\n";
                //}
            }
            //else
            //{
            //    // Show hint when no background
            //    ImGui::TextColored(ImVec4(1, 1, 0, 1),
            //        "Drag an image from Assets panel to add background");
            //}
        }
    };

} // NightSnack

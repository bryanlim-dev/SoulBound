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
        ScenePanel(Coordinator& coord) : 
            m_coord(coord) 
        {}

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

            // Set the OpenGL viewport to the aspect-corrected size
            glViewport(0, 0, (int)imageSize.x, (int)imageSize.y);

            // Only recreate/resize if needed
            static ImVec2 lastSize = ImVec2(0, 0);
            if (imageSize.x != lastSize.x || imageSize.y != lastSize.y)
            {
                CreateOrResizeTexture((int)imageSize.x, (int)imageSize.y);
                lastSize = imageSize;
            }

            if (!m_sceneTexture)
            {
                CreateOrResizeTexture((int)imageSize.x, (int)imageSize.y);
            }
            UpdateTextureFromFramebuffer((int)imageSize.x, (int)imageSize.y);

            // Center the image in the panel
            ImVec2 cursorPos = ImGui::GetCursorPos();
            ImVec2 offset = ImVec2((panelSize.x - imageSize.x) * 0.5f, (panelSize.y - imageSize.y) * 0.5f);
            ImGui::SetCursorPos(ImVec2(cursorPos.x + offset.x, cursorPos.y + offset.y));

            if (m_sceneTexture && imageSize.x > 0 && imageSize.y > 0)
            {
                // Store cursor position for drop zone
                ImVec2 imageTopLeft = ImGui::GetCursorScreenPos();

                ImGui::Image((ImTextureID)m_sceneTexture, imageSize, ImVec2(0, 1), ImVec2(1, 0));

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

                // Button to remove background
                if (ImGui::Button("Remove Background"))
                {
                    m_coord.DestroyEntity(m_backgroundEntity);
                    m_backgroundEntity = INVALID_ENTITY;
                    std::cout << "[Scene] Background removed\n";
                }
            }
            else
            {
                // Show hint when no background
                ImGui::TextColored(ImVec4(1, 1, 0, 1),
                    "Drag an image from Assets panel to add background");
            }
        }
    };

} // NightSnack

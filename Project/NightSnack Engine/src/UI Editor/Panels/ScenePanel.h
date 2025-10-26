/******************************************************************************
 * File:        ScenePanel.h
 * Project:     NightSnack
 * Author(s):	
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
        ScenePanel(Coordinator& coord) : m_coord(coord) {}

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
                ImGui::Image((ImTextureID)m_sceneTexture, imageSize, ImVec2(0, 1), ImVec2(1, 0));
            }
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
    };

} // NightSnack

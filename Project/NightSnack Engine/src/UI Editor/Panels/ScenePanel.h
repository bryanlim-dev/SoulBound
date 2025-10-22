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
            ImVec2 avail = ImGui::GetContentRegionAvail();

			// Get window dimensions from Coordinator
            int windowWidth = m_coord.GetWindowWidth();
            int windowHeight = m_coord.GetWindowHeight();

            // Only recreate/resize if needed
            static int lastWidth = 0, lastHeight = 0;
            if (windowWidth != lastWidth || windowHeight != lastHeight) {
                CreateOrResizeTexture(windowWidth, windowHeight);
                lastWidth = windowWidth;
                lastHeight = windowHeight;
            }

            // Always copy the full window framebuffer
            if (!m_sceneTexture)
            {
                CreateOrResizeTexture(windowWidth, windowHeight);
            }
            UpdateTextureFromFramebuffer(windowWidth, windowHeight);

            // Calculate aspect-correct size
            float windowAspect = (float)windowWidth / (float)windowHeight;
            float panelAspect = avail.x / avail.y;
            ImVec2 imageSize;
            if (panelAspect > windowAspect)
            {
                imageSize.y = avail.y;
                imageSize.x = avail.y * windowAspect;
            }
            else
            {
                imageSize.x = avail.x;
                imageSize.y = avail.x / windowAspect;
            }

            // Center the image in the panel
            ImVec2 cursorPos = ImGui::GetCursorPos();
            ImVec2 offset = ImVec2((avail.x - imageSize.x) * 0.5f, (avail.y - imageSize.y) * 0.5f);
            ImGui::SetCursorPos(ImVec2(cursorPos.x + offset.x, cursorPos.y + offset.y));

            ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(m_sceneTexture)), imageSize, ImVec2(0, 1), ImVec2(1, 0));
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

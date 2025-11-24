/******************************************************************************
 * File:        Tilemap.h
 * Project:     NightSnack
 * Author(s):	Wei Jie Lim (Primary, 100%) - Created and implemented tilemap window
 *
 * Description:
 * Tilemap window showing the grid layout for the current scene. Allows for
 * editing of tile properties and placement of entities on the grid.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once
#include "AllPanels.h"

namespace NightSnack
{
    class Tilemap {
    public:

        // Tilemap overlay settings
        bool tileOverlayEnabled = false;
        float gridSize = 24.0f;
        ImU32 gridColor = IM_COL32(200, 200, 200, 100);

        // Tile editor settings
        bool showTileEditor = false;
        std::string tileTexturePath;
        int tileRows = 8;
        int tileCols = 8;
        int selectedTile = -1; // index of selected tile
        ImTextureID tileTextureID{};


        /************************************************************************/
        /*!
        \brief
		    Default Constructor for Tilemap.
        */
        /************************************************************************/
        Tilemap() = default;

        /************************************************************************/
        /*!
        \brief
			Draws the settings for the tile overlay in the scene.
        */
        /************************************************************************/
        void DrawTilemapSettings();

        /************************************************************************/
        /*!
        \brief
            Draws the settings for the tile overlay in the scene.

		\param[in] imageTopLeft
		    - Top-left position of the scene image in the panel.

		\param[in] imageSize
		    - Size of the scene image in the panel.

		\param[in] worldLeft
		    - Left boundary of the world coordinates.

		\param[in] worldRight
		    - Right boundary of the world coordinates.

		\param[in] worldBottom
		    - Bottom boundary of the world coordinates.

		\param[in] worldTop
		    - Top boundary of the world coordinates.
        */
        /************************************************************************/
        void DrawTilemapOverlay(const ImVec2& imageTopLeft, const ImVec2& imageSize,
            float worldLeft, float worldRight, float worldBottom, float worldTop) const;

        /************************************************************************/
        /*!
        \brief
		    Draws the tile editor panel for selecting and placing tiles.

        \param[in] coord
			- Reference to the ECS Coordinator.

		\param[in] selectedEntity
			- Currently selected entity in the scene.
        */
        /************************************************************************/
        void DrawTileEditor(Coordinator& coord, Entity selectedEntity);

        /************************************************************************/
        /*!
        \brief
			Returns the currently selected tile index.

        \param[in] coord
            - Reference to the ECS Coordinator.

        \param[in] selectedEntity
            - Currently selected entity in the scene.
        */
        /************************************************************************/
        int GetSelectedTile() const { return selectedTile; }

        /************************************************************************/
        /*!
        \brief
			Helper function to convert world to screen coordinates in image.
        */
        /************************************************************************/
        static ImVec2 WorldToScreen(float wx, float wy, float worldLeft, float worldRight,
            float worldBottom, float worldTop, ImVec2 imageTopLeft, ImVec2 imageSize);

    private:
        // Editor view control
        float editorZoom = 1.0f;
        ImVec2 editorPan = ImVec2(0, 0);

        int tileTextureWidth = 0;
        int tileTextureHeight = 0;
    };

    inline void Tilemap::DrawTilemapSettings()
    {
        ImGui::Checkbox("Tile Overlay", &tileOverlayEnabled);
        ImGui::SliderFloat("Grid Size", &gridSize, 8.0f, 128.0f, "%.0f");
        if (ImGui::Button("Open Tile Editor")) showTileEditor = true;
    }

    inline void Tilemap::DrawTilemapOverlay(const ImVec2& imageTopLeft, const ImVec2& imageSize,
        float worldLeft, float worldRight, float worldBottom, float worldTop) const

    {
        if (!tileOverlayEnabled) return;

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        for (float x = std::floor(worldLeft / gridSize) * gridSize; x <= worldRight; x += gridSize)
        {
            ImVec2 p1 = WorldToScreen(x, worldBottom, worldLeft, worldRight, worldBottom, worldTop, imageTopLeft, imageSize);
            ImVec2 p2 = WorldToScreen(x, worldTop, worldLeft, worldRight, worldBottom, worldTop, imageTopLeft, imageSize);
            draw_list->AddLine(p1, p2, gridColor);
        }
        for (float y = std::floor(worldBottom / gridSize) * gridSize; y <= worldTop; y += gridSize)
        {
            ImVec2 p1 = WorldToScreen(worldLeft, y, worldLeft, worldRight, worldBottom, worldTop, imageTopLeft, imageSize);
            ImVec2 p2 = WorldToScreen(worldRight, y, worldLeft, worldRight, worldBottom, worldTop, imageTopLeft, imageSize);
            draw_list->AddLine(p1, p2, gridColor);
        }
    }

    inline void Tilemap::DrawTileEditor(Coordinator& coord, Entity selectedEntity)
    {
		(void)selectedEntity; // Unused for now
        if (!showTileEditor) return;

        ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_Once);
        ImGui::Begin("Tile Editor", &showTileEditor);
        static ImVec2 textureCenter = ImVec2(0, 0);

        // --- Drag-and-Drop Texture Support ---
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_IMAGE"))
            {
                const char* droppedPath = static_cast<const char*>(payload->Data);
                tileTexturePath = droppedPath;

                // Load texture using ResourceManager
                auto resourceMgr = coord.GetSystem<ResourceManager>();
                if (resourceMgr)
                {
                    size_t imageKey = 0;
                    resourceMgr->LoadImage(tileTexturePath, imageKey);
                    int width, height, channels;
                    unsigned char* pixels = resourceMgr->GetImage(imageKey, width, height, channels);
                    if (pixels) {
                        GLuint texID = 0;
                        glGenTextures(1, &texID);
                        glBindTexture(GL_TEXTURE_2D, texID);
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, channels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, pixels);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        tileTextureID = (ImTextureID)texID;
                        tileTextureWidth = width;
                        tileTextureHeight = height;

                        // Snap top-left of texture to nearest grid intersection
                        ImVec2 mousePos = ImGui::GetMousePos();
                        float scaledGridSize = gridSize * editorZoom;
                        float snappedX = std::floor((mousePos.x - editorPan.x) / scaledGridSize) * scaledGridSize;
                        float snappedY = std::floor((mousePos.y - editorPan.y) / scaledGridSize) * scaledGridSize;
                        textureCenter = ImVec2(snappedX, snappedY);
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }

        // --- Show drag instruction if no texture ---
        if (!tileTextureID)
            ImGui::Text("Drag a texture from the Assets panel here.");

        // --- Pan and Zoom Controls ---
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 mousePos = ImGui::GetMousePos();

        // Store where to center the texture
        if (textureCenter.x == 0 && textureCenter.y == 0)
            textureCenter = ImVec2(windowPos.x + windowSize.x * 0.5f, windowPos.y + windowSize.y * 0.5f);

        // Zoom with mouse wheel
        if (ImGui::IsWindowHovered())
        {
            float wheel = ImGui::GetIO().MouseWheel;
            if (wheel != 0.0f)
            {
                float prevZoom = editorZoom;
                editorZoom = std::clamp(editorZoom * (wheel > 0 ? 1.1f : 0.9f), 0.2f, 10.0f);

                ImVec2 mouseRel;
                mouseRel.x = mousePos.x - windowPos.x - editorPan.x;
                mouseRel.y = mousePos.y - windowPos.y - editorPan.y;
                float zoomFactor = (editorZoom / prevZoom - 1.0f);
                editorPan.x -= mouseRel.x * zoomFactor;
                editorPan.y -= mouseRel.y * zoomFactor;
            }
        }

        // Pan with right mouse drag
        if (ImGui::IsWindowHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Right))
        {
            ImVec2 dragDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
            editorPan.x += dragDelta.x;
            editorPan.y += dragDelta.y;
            ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);
        }

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        // --- Infinite Grid Drawing ---
        ImVec2 gridOrigin = tileTextureID ? textureCenter : ImVec2(windowPos.x + windowSize.x * 0.5f, windowPos.y + windowSize.y * 0.5f);
        gridOrigin.x += editorPan.x;
        gridOrigin.y += editorPan.y;

        float scaledGridSize = gridSize * editorZoom;
        float left = windowPos.x;
        float right = windowPos.x + windowSize.x;
        float top = windowPos.y;
        float bottom = windowPos.y + windowSize.y;
        float firstX = gridOrigin.x + std::floor((left - gridOrigin.x) / scaledGridSize) * scaledGridSize;
        float firstY = gridOrigin.y + std::floor((top - gridOrigin.y) / scaledGridSize) * scaledGridSize;

        // Draw vertical grid lines
        for (float x = firstX; x <= right; x += scaledGridSize)
            draw_list->AddLine(ImVec2(x, top), ImVec2(x, bottom), gridColor);
        // Draw horizontal grid lines
        for (float y = firstY; y <= bottom; y += scaledGridSize)
            draw_list->AddLine(ImVec2(left, y), ImVec2(right, y), gridColor);

        // --- Draw Texture at Actual Pixel Size ---
        if (tileTextureID)
        {
            float texWidth = tileTextureWidth * editorZoom;
            float texHeight = tileTextureHeight * editorZoom;
            ImVec2 texPos = ImVec2(textureCenter.x + editorPan.x, textureCenter.y + editorPan.y);

            // Flip the UVs to correct vertical orientation
            draw_list->AddImage(tileTextureID, texPos, ImVec2(texPos.x + texWidth, texPos.y + texHeight), ImVec2(0, 1), ImVec2(1, 0));

            // Overlay grid on texture
            int cols = static_cast<int>(tileTextureWidth / gridSize);
            int rows = static_cast<int>(tileTextureHeight / gridSize);
            for (int x = 0; x <= cols; ++x)
            {
                float gx = texPos.x + x * scaledGridSize;
                draw_list->AddLine(ImVec2(gx, texPos.y), ImVec2(gx, texPos.y + texHeight), IM_COL32(255, 255, 0, 180));
            }
            for (int y = 0; y <= rows; ++y)
            {
                float gy = texPos.y + y * scaledGridSize;
                draw_list->AddLine(ImVec2(texPos.x, gy), ImVec2(texPos.x + texWidth, gy), IM_COL32(255, 255, 0, 180));
            }
        }

        // --- Tile Selection ---
        if (tileTextureID)
        {
            float cellWidth = gridSize * editorZoom;
            float cellHeight = gridSize * editorZoom;
            draw_list = ImGui::GetWindowDrawList();

            for (int row = 0; row < tileRows; ++row)
            {
                for (int col = 0; col < tileCols; ++col)
                {
                    ImVec2 cellMin = ImVec2(textureCenter.x + editorPan.x + col * cellWidth,
                        textureCenter.y + editorPan.y + row * cellHeight);
                    ImVec2 cellMax = ImVec2(cellMin.x + cellWidth, cellMin.y + cellHeight);

                    // Highlight selected tile
                    int tileIdx = row * tileCols + col;
                    if (selectedTile == tileIdx)
                        draw_list->AddRect(cellMin, cellMax, IM_COL32(0, 255, 0, 255), 0.0f, 0, 2.0f);

                    // Selection logic
                    if (ImGui::IsMouseHoveringRect(cellMin, cellMax) && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                        selectedTile = tileIdx;
                }
            }
        }

        ImGui::End();
    }

    inline ImVec2 Tilemap::WorldToScreen(float wx, float wy, float worldLeft, float worldRight,
        float worldBottom, float worldTop, ImVec2 imageTopLeft, ImVec2 imageSize)
    {
        float u = (wx - worldLeft) / (worldRight - worldLeft);
        float v = 1.0f - (wy - worldBottom) / (worldTop - worldBottom);
        return ImVec2(imageTopLeft.x + u * imageSize.x, imageTopLeft.y + v * imageSize.y);
    }


} // NightSnack

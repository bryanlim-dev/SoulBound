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
        float gridSize = 32.0f;
        ImU32 gridColor = IM_COL32(200, 200, 200, 100);

        // Tile editor settings
        bool showTileEditor = false;
        std::string tileTexturePath;
        int tileRows = 8;
        int tileCols = 8;
        int selectedTile = -1; // index of selected tile
        ImTextureID tileTextureID;


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
            float worldLeft, float worldRight, float worldBottom, float worldTop);

        /************************************************************************/
        /*!
        \brief
		    Draws the tile editor panel for selecting and placing tiles.
        */
        /************************************************************************/
        void DrawTileEditor();

        /************************************************************************/
        /*!
        \brief
			Helper function to convert world to screen coordinates in image.
        */
        /************************************************************************/
        static ImVec2 WorldToScreen(float wx, float wy, float worldLeft, float worldRight,
            float worldBottom, float worldTop, ImVec2 imageTopLeft, ImVec2 imageSize);

    };

    inline void Tilemap::DrawTilemapSettings()
    {
        ImGui::Checkbox("Tile Overlay", &tileOverlayEnabled);
        ImGui::SliderFloat("Grid Size", &gridSize, 8.0f, 128.0f, "%.0f");
        if (ImGui::Button("Open Tile Editor")) showTileEditor = true;
    }

    inline void Tilemap::DrawTilemapOverlay(const ImVec2& imageTopLeft, const ImVec2& imageSize,
        float worldLeft, float worldRight, float worldBottom, float worldTop)

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

    inline void Tilemap::DrawTileEditor()
    {
        if (!showTileEditor) return;
        ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_Once); // Set a reasonable default size
        ImGui::Begin("Tile Editor", &showTileEditor);

        // Editable parameters
        char texPathBuffer[256];
        strncpy_s(texPathBuffer, tileTexturePath.c_str(), sizeof(texPathBuffer));
        texPathBuffer[sizeof(texPathBuffer) - 1] = 0;
        if (ImGui::InputText("Texture Path", texPathBuffer, sizeof(texPathBuffer))) {
            tileTexturePath = texPathBuffer;
            // TODO: Load texture and set tileTextureID here
        }
        ImGui::InputInt("Rows", &tileRows);
        ImGui::InputInt("Cols", &tileCols);

        ImVec2 texSize(256, 256); // Replace with actual texture size after loading
        if (tileTextureID)
        {
            ImGui::Text("Select a tile:");
            ImVec2 gridOrigin = ImGui::GetCursorScreenPos();
            ImGui::Image(tileTextureID, texSize);

            // Draw Tilemap overlay
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            float cellW = texSize.x / tileCols;
            float cellH = texSize.y / tileRows;
            for (int r = 0; r <= tileRows; ++r)
            {
                float y = gridOrigin.y + r * cellH;
                draw_list->AddLine(ImVec2(gridOrigin.x, y), ImVec2(gridOrigin.x + texSize.x, y), IM_COL32(255, 255, 255, 80));
            }
            for (int c = 0; c <= tileCols; ++c)
            {
                float x = gridOrigin.x + c * cellW;
                draw_list->AddLine(ImVec2(x, gridOrigin.y), ImVec2(x, gridOrigin.y + texSize.y), IM_COL32(255, 255, 255, 80));
            }

            // Tile selection logic
            ImVec2 mouse = ImGui::GetMousePos();
            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                int col = (int)((mouse.x - gridOrigin.x) / cellW);
                int row = (int)((mouse.y - gridOrigin.y) / cellH);
                if (col >= 0 && col < tileCols && row >= 0 && row < tileRows)
                    selectedTile = row * tileCols + col;
            }

            // Highlight selected tile
            if (selectedTile >= 0)
            {
                int selRow = selectedTile / tileCols;
                int selCol = selectedTile % tileCols;
                ImVec2 min = ImVec2(gridOrigin.x + selCol * cellW, gridOrigin.y + selRow * cellH);
                ImVec2 max = ImVec2(min.x + cellW, min.y + cellH);
                draw_list->AddRect(min, max, IM_COL32(255, 0, 0, 255), 0.0f, 0, 2.0f);
            }

            // Save selected tile as PNG (pseudo-code)
            if (selectedTile >= 0 && ImGui::Button("Save Selected Tile"))
            {
                // TODO: Extract tile from texture and save as PNG
            }
        }
        else
        {
            ImGui::Text("Texture not loaded.");
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

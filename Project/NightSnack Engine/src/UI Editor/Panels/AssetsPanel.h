/******************************************************************************
 * File:        AssetsPanel.h
 * Project:     NightSnack
 * Author(s):	
 *
 * Description:
 * Assets panel displaying and handling all game assets.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once
#include "AllPanels.h"

namespace NightSnack
{
    class AssetsPanel {
    public:
        AssetsPanel(Coordinator& coord) : m_coord(coord) {}

        void Draw()
        {
			// Set initial window size and style
            ImGui::SetNextWindowSize(ImVec2(600, 300), ImGuiCond_Always);

			// Options Window
            ImGui::Begin("Assets");

            if (ImGui::BeginTabBar("AssetsTabs"))
            {
                // Code here

                ImGui::EndTabBar();
            }

            ImGui::End();
        }

    private:
        Coordinator& m_coord;
    };
}

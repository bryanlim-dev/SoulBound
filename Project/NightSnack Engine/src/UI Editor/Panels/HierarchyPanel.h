/******************************************************************************
 * File:        HierarchyPanel.h
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan (Primary, 100%)
 *
 * Description:
 * Base class for UI panels (Currently not implemented).
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once
#include <imgui.h>
#include <string>
#include <cstdint>
#include "Core/ECS/Coordinator.h"

namespace NightSnack
{
    class HierarchyPanel {
	public:
        HierarchyPanel(Coordinator& coord) : m_coord(coord) {}

        void Draw() 
        {
            ImGui::SetNextWindowSize(ImVec2(260, 300), ImGuiCond_Always);

            ImGuiStyle& style = ImGui::GetStyle();
            style.Colors[ImGuiCol_WindowBg]      = ImVec4(0.15f, 0.18f, 0.22f, 1.0f); // Panel background
            style.Colors[ImGuiCol_TitleBg]       = ImVec4(0.10f, 0.12f, 0.15f, 1.0f); // Title bar (inactive)
            style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.25f, 0.30f, 1.0f); // Title bar (active)

            ImGui::Begin("Hierarchy");

            if (ImGui::BeginTabBar("HierarchyTabs")) 
            {
                // Scene tab
                if (ImGui::BeginTabItem("Scene"))
                {
                    // Scene dropdown
                    if (ImGui::TreeNode("Scene"))
                    {
                        const auto& entities = m_coord.GetLivingEntities();
                        for (Entity e : entities)
                        {
                            ImGui::Selectable(std::to_string(e).c_str());
                        }
                        ImGui::TreePop();
                    }
                    ImGui::EndTabItem();
                }

                // Prefabs tab
                if (ImGui::BeginTabItem("Prefabs"))
                {
                    // TODO: Replace with your prefab logic
                    ImGui::Text("Prefab list & instances coming soon");
                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }

            ImGui::End();
        }

    private:
        Coordinator& m_coord;
    };
}

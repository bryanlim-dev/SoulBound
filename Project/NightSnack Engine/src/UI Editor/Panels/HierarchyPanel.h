/******************************************************************************
 * File:        HierarchyPanel.h
 * Project:     NightSnack
 * Author(s):	
 *
 * Description:
 * Hierarchy panel showing all entities in the scene.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once
#include "AllPanels.h"

namespace NightSnack
{
    class HierarchyPanel {
    public:
        HierarchyPanel(Coordinator& coord, std::shared_ptr<Entity> selectedEntity, std::unordered_map<Entity, std::string>& uiNames)
            : m_coord(coord), m_selectedEntity(selectedEntity), m_uiNames(uiNames) {}

        void Draw()
        {
			// Set initial window size and style
            //ImGui::SetNextWindowSize(ImVec2(260, 300), ImGuiCond_Once);

            ImGuiStyle& style = ImGui::GetStyle();
            style.Colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.0f);
            style.Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.12f, 0.15f, 1.0f);
            style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.25f, 0.30f, 1.0f);

			// Hierarchy Window
            ImGui::Begin("Hierarchy");

            if (ImGui::BeginTabBar("HierarchyTabs"))
            {
				// Scene Tab
                if (ImGui::BeginTabItem("Scene"))
                {
                    // Scene Dropdown list
                    if (ImGui::TreeNodeEx("Scene", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        const auto& entities = m_coord.GetLivingEntities();
                        auto factory = m_coord.GetSystem<NightSnack::EntityFactory>();
                        for (Entity e : entities)
                        {
                            if (m_uiNames.find(e) == m_uiNames.end()) {
                                m_uiNames[e] = factory ? factory->GetEntityPrefabName(e) : "Entity " + std::to_string(e);
                            }

                            std::string label = m_uiNames[e] + "##" + std::to_string(e);

                            bool selected = (m_selectedEntity && e == *m_selectedEntity);
                            if (ImGui::Selectable(label.c_str(), selected)) {
                                *m_selectedEntity = e;
                            }
                        }
                        ImGui::TreePop();

                        // Add Prefab button at the bottom
                        if (ImGui::Button("Add Prefab"))
                        {
                            Entity newEntity = m_coord.CreateEntity();
                            
                            Transform defaultTransform{
                                Vec3(0.0f, 0.0f, 0.0f),
                                0.0f,
                                Vec2(200.f, 200.f)
                            };

                            Renderable defaultRenderable{
                                "circle",
                                "ColorMat",
                                RenderLayer::DEFAULT,
                                false,
                                true
                            };

                            m_coord.AddComponent<Transform>(newEntity, defaultTransform);
                            m_coord.AddComponent<Renderable>(newEntity, defaultRenderable);
                            m_coord.AddComponent<Color>(newEntity, Color{ 1.f, 0.f, 0.f, 1.f });

                            *m_selectedEntity = newEntity;
                        }
                    }
                    ImGui::EndTabItem();
                }

				// Prefabs Tab
                if (ImGui::BeginTabItem("Prefabs"))
                {
                    ImGui::Text("Prefab list & instances coming soon");
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }

            ImGui::End();
        }

    private:
        Coordinator& m_coord;
        std::shared_ptr<Entity> m_selectedEntity;
        std::unordered_map<Entity, std::string>& m_uiNames;
    };
}

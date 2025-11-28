/******************************************************************************
 * File:        HierarchyPanel.h
 * Project:     NightSnack
 * Author(s):	Wei Jie Lim (Primary, 100%) - Created and implemented hierarchy panel
 *
 * Description:
 * Hierarchy panel showing all entities in the scene. Able to select, add 
 * and delete entities.
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

        /************************************************************************/
        /*!
        \brief
		    Constructor for HierarchyPanel.

		\param[in] coord
		    - Reference to Coordinator.

		\param[in] selectedEntity
		    - Shared pointer to currently selected entity.

        \param[in] uiNames
			- Shared pointer to map of entity to UI names.
        */
        /************************************************************************/
        HierarchyPanel(Coordinator& coord, std::shared_ptr<Entity> selectedEntity, std::shared_ptr<std::unordered_map<Entity, std::string>> uiNames)
            : m_coord(coord), m_selectedEntity(selectedEntity), m_uiNames(uiNames) {}

        /************************************************************************/
        /*!
        \brief
		    Draws hierarchy panel showing all entities in the scene.
        */
        /************************************************************************/
        void Draw()
        {
			// Set initial window size and style
            //ImGui::SetNextWindowSize(ImVec2(260, 300), ImGuiCond_Once);

			// Hierarchy Window
            ImGui::Begin("Hierarchy");

            if (ImGui::BeginTabBar("HierarchyTabs"))
            {
				// Scene Tab
                if (ImGui::BeginTabItem("Scene"))
                {
                    // Highlight Objects with Dropdown
                    ImVec2 min = ImGui::GetCursorScreenPos();
                    ImVec2 max = ImVec2(min.x + ImGui::GetContentRegionAvail().x, min.y + ImGui::GetTextLineHeight());
                    ImU32 bgColor = IM_COL32(80, 180, 255, 60); // Light blue, semi-transparent
                    ImGui::GetWindowDrawList()->AddRectFilled(min, max, bgColor);


                    // Scene Dropdown list
                    if (ImGui::TreeNodeEx("Scene", ImGuiTreeNodeFlags_DefaultOpen))
                    {
						ImGui::Indent(30.f);
                        const auto& entities = m_coord.GetLivingEntities();
                        auto factory = m_coord.GetSystem<NightSnack::EntityFactory>();

						// List all entities
                        for (Entity e : entities)
                        {
							// Assign default UI name if not present
                            if (m_uiNames->find(e) == m_uiNames->end()) {
                                (*m_uiNames)[e] = factory ? factory->GetEntityPrefabName(e) : "Entity " + std::to_string(e);
                            }

							// Create unique label for ImGui Selectable
                            std::string label = (*m_uiNames)[e] + "##" + std::to_string(e);

							// Check if this entity is selected
                            bool selected = (m_selectedEntity && e == *m_selectedEntity);
                            if (ImGui::Selectable(label.c_str(), selected)) {
                                *m_selectedEntity = e;
                            }
                        }
						ImGui::Unindent(30.f);
                        ImGui::TreePop();
                    }

                    // Add Prefab button at the bottom of list
                    if (ImGui::Button("Add Prefab"))
                    {
                        auto factory = m_coord.GetSystem<NightSnack::EntityFactory>();
                        Transform defaultTransform
                        {
                            Vec3(0.f, 145.f, 1.f),
                            0.0f,
                            Vec2(100.f, 100.f)
                        };

                        factory->Create(std::string{ "Ghost" }, defaultTransform);
                    }

                    // Delete button to remove selected entity
                    if (m_selectedEntity && *m_selectedEntity != INVALID_ENTITY)
                    {
                        ImGui::SameLine();
                        if (ImGui::Button("Delete"))
                        {
                            m_coord.DestroyEntity(*m_selectedEntity);
                            m_uiNames->erase(*m_selectedEntity);
                            *m_selectedEntity = INVALID_ENTITY;
                        }
                    }
                    if (m_selectedEntity && *m_selectedEntity != INVALID_ENTITY && ImGui::IsKeyPressed(ImGuiKey_Delete))
                    {
                        if (m_coord.HasComponent<Transform>(*m_selectedEntity))
                        {
                            auto factory = m_coord.GetSystem<EntityFactory>();
                            auto prefabName = factory->GetEntityPrefabName(*m_selectedEntity);
                            auto transform = m_coord.GetComponent<Transform>(*m_selectedEntity);
                            NightSnack::UndoSystem::Instance().Push(
                                NightSnack::EntityDeleteSnapshot{ *m_selectedEntity, prefabName, transform }
                            );
                        }
                        m_coord.DestroyEntity(*m_selectedEntity);
                        m_uiNames->erase(*m_selectedEntity);
                        *m_selectedEntity = INVALID_ENTITY;
                    }

                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }

            ImGui::End();
        }

    private:
        Coordinator& m_coord;
        std::shared_ptr<Entity> m_selectedEntity;
        std::shared_ptr<std::unordered_map<Entity, std::string>> m_uiNames;
    };
}

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
                    // Scene Dropdown list
                    if (ImGui::TreeNodeEx("Scene", ImGuiTreeNodeFlags_DefaultOpen))
                    {
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
                        ImGui::TreePop();

                        // Add Prefab button at the bottom of list
                        if (ImGui::Button("Add Prefab"))
                        {
                            Entity newEntity = m_coord.CreateEntity();
                            
                            Transform defaultTransform{
                                Vec3(0.f, 145.f, 1.f),
                                0.0f,
                                Vec2(100.f, 100.f)
                            };

                            Renderable defaultRenderable{
                                "square",
                                "ColorMat",
                                false,
                                true
                            };

                            m_coord.AddComponent<Transform>(newEntity, defaultTransform);
                            m_coord.AddComponent<Renderable>(newEntity, defaultRenderable);
                            m_coord.AddComponent<Color>(newEntity, Color{ 1.f, 0.f, 0.f, 1.f });

                            *m_selectedEntity = newEntity;
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
        std::shared_ptr<std::unordered_map<Entity, std::string>> m_uiNames;
    };
}

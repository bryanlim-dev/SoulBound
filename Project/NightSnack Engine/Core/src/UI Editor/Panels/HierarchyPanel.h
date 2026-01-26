/******************************************************************************
 * File:        HierarchyPanel.h
 * Project:     NightSnack
 * Author(s):	Wei Jie Lim (Primary, 100%) - Created and implemented hierarchy panel
 *
 * Description:
 * Hierarchy panel showing all entities in the scene. Able to select, add 
 * and delete entities.
 *
 * Copyright � 2025 DigiPen Institute of Technology Singapore.
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

                    auto factory = m_coord.GetSystem<EntityFactory>();

                    // Add Prefab button at the bottom of list
                    if (ImGui::Button("Create"))
                    {
                        factory->Create();
                    }

                    // Delete button to remove selected entity
                    if (m_selectedEntity && *m_selectedEntity != INVALID_ENTITY)
                    {
                        ImGui::SameLine();
                        if (ImGui::Button("Delete"))
                        {
                            DeleteEntity();
                        }
                    }
                    if (m_selectedEntity && *m_selectedEntity != INVALID_ENTITY && ImGui::IsKeyPressed(ImGuiKey_Delete))
                    {
                        DeleteEntity();
                    }

                    // Scene Dropdown list
                    if (ImGui::TreeNodeEx("Scene", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        const auto& entities = m_coord.GetLivingEntities();

						// List all entities
                        for (Entity e : entities)
                        {
							// Hide SceneCam from hierarchy
                            if (m_uiNames->count(e) && (*m_uiNames)[e] == "SceneCam") continue;

							// Assign default UI name if not present
                            if (m_uiNames->find(e) == m_uiNames->end()) {
                                (*m_uiNames)[e] = factory ? factory->GetEntityPrefabName(e) : "Entity " + std::to_string(e);
                            }

                            // Draw Hierarchy Tree
                            if (!m_coord.HasComponent<Hierarchy>(e)) continue;
                            auto& h = m_coord.GetComponent<Hierarchy>(e);
                            if (h.parent == INVALID_ENTITY)
                            {
                                DrawEntityTree(e);
                            }
                        }
						ImGui::Unindent(30.f);
                        ImGui::TreePop();
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

        /************************************************************************/
        /*!
        \brief
			Deletes the selected entity from the scene and records the 
            action for undo.
        */
        /************************************************************************/
        void DeleteEntity()
        {
            auto factory = m_coord.GetSystem<EntityFactory>();

			// Record entity deletion for undo
            auto prefabName = factory->GetEntityPrefabName(*m_selectedEntity);
            auto transform = m_coord.GetComponent<Transform>(*m_selectedEntity);
            NightSnack::UndoSystem::Instance().Push(
                NightSnack::EntityDeleteSnapshot{ *m_selectedEntity, prefabName, transform }
            );

			// Remove entity from ECS
            factory->RemoveEntity(*m_selectedEntity);
            m_uiNames->erase(*m_selectedEntity);
            *m_selectedEntity = INVALID_ENTITY;
        }

        /************************************************************************/
        /*!
        \brief
		    Draws the entity tree recursively for parent-child relationships.

        \param[in] entity
		    - The entity to draw the tree for.
        */
        /************************************************************************/
        void DrawEntityTree(Entity e)
        {
            auto& uiNames = *m_uiNames;
            auto factory = m_coord.GetSystem<EntityFactory>();

            if (uiNames.find(e) == uiNames.end()) {
                uiNames[e] = factory ? factory->GetEntityPrefabName(e) : "Entity " + std::to_string(e);
            }

            auto& h = m_coord.GetComponent<Hierarchy>(e);
            bool hasChildren = !h.children.empty();
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
            if (!hasChildren) flags |= ImGuiTreeNodeFlags_Leaf;

            // Selection logic
            bool selected = (m_selectedEntity && e == *m_selectedEntity);
            if (selected) flags |= ImGuiTreeNodeFlags_Selected;

            std::string label = uiNames[e] + "##" + std::to_string(e);
            if (m_coord.HasComponent<Camera>(e)) label = "CAMERA";
            bool open = ImGui::TreeNodeEx(label.c_str(), flags);

            // Handle selection
            if (ImGui::IsItemClicked())
            {
                *m_selectedEntity = e;
            }

            // Drag source
            if (ImGui::BeginDragDropSource())
            {
                ImGui::SetDragDropPayload("ENTITY_DRAG", &e, sizeof(Entity));
                ImGui::Text("Move %s", uiNames[e].c_str());
                ImGui::EndDragDropSource();
            }

            // Drop target
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_DRAG"))
                {
                    Entity dragged = *(const Entity*)payload->Data;
                    if (dragged != e && m_coord.HasComponent<Hierarchy>(dragged) && m_coord.HasComponent<Hierarchy>(e))
                    {
                        m_coord.ParentEntity(dragged, e);
                    }
                }
                ImGui::EndDragDropTarget();
            }

            if (open)
            {
                for (Entity child : h.children)
                {
                    DrawEntityTree(child);
                }
                ImGui::TreePop();
            }
        }
    };
}

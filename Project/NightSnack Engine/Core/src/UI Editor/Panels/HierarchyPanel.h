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
#include "GameLogic/SceneManager.h"

namespace NightSnack
{
    class ImguiSystem;

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

            std::vector<Entity> camTriggerDeleteQueue;

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
                        Entity newEntity = factory->Create();

                        NightSnack::UndoSystem::Instance().Push(
                            NightSnack::EntityCreateSnapshot{ newEntity }
                        );
                    }

					// Add Collider button to Create collider entity
                    ImGui::SameLine();
                    if (ImGui::Button("Create Collider")) {

                        //Vec3 campos = m_coord.GetComponent<Transform>(m_imgui.GetScenePanel().GetEditorCamera()).pos;
                        Transform t;
                        //t.pos = Vec3{ campos.x, campos.y, 0.f };
                        t.scale = Vec2{ 100.f, 100.f };
                        t.pos.z = 1.f;
                        Collider c;
                        c.width = c.height = 100.f;
                        c.radius = 50.f;
                        Entity newCollider = factory->CreateCollider(c, t);
                        
                        NightSnack::UndoSystem::Instance().Push(
                            NightSnack::EntityCreateSnapshot{ newCollider }
                        );
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

                // CamTriggers Tab
                if (ImGui::BeginTabItem("CamTriggers"))
                {
                    auto sceneManager = m_coord.GetSystem<NightSnack::Scene::SceneManager>();
                    auto keyframesCopy = sceneManager->GetCameraKeyframes();
                    static int selectedKeyframe = 0;

                    ImGui::Text("Keyframes for CamTrigger:");
                    for (int i = 0; i < keyframesCopy.size(); ++i)
                    {
                        float maxX = keyframesCopy[i].second["maxX"];
                        float maxY = keyframesCopy[i].second["maxY"];
                        float minX = keyframesCopy[i].second["minX"];
                        float minY = keyframesCopy[i].second["minY"];
                        
                        char label[128];
                        snprintf(label, sizeof(label), "Keyframe %d \n[minX: %.1f, maxX: %.1f] \n[minY: %.1f, maxY: %.1f]", i, minX, maxX, minY, maxY);
                        if (ImGui::Selectable(label, selectedKeyframe == i))
                        {
                            selectedKeyframe = i;
                        }
                    }
                    if (ImGui::Button("Create CamTrigger"))
                    {
                        Entity sceneCam = m_coord.GetSystem<GraphicsSystem>()->GetCamera();
                        Vec3 camPos = m_coord.GetComponent<Transform>(sceneCam).pos;
                        Vec2 scale = Vec2{ 100.f, 100.f };
                        json bounds = json::object();
                        if (!keyframesCopy.empty() && (selectedKeyframe >= 0 && selectedKeyframe < keyframesCopy.size()))
                        {
                            bounds = keyframesCopy[selectedKeyframe].second;
                        }
                        else
                        {
                            bounds["minX"] = 0.f;
                            bounds["maxX"] = 0.f;
                            bounds["minY"] = 0.f;
                            bounds["maxY"] = 0.f;
							bounds["zoom"] = 1.f;
                            bounds["pos"] = { {"x", camPos.x}, {"y", camPos.y}, {"z", camPos.z} };
                            bounds["scale"] = { {"x", scale.x}, {"y", scale.y} };
                        }
                        sceneManager->CreateCamTrigger(camPos, scale, bounds);

                        // Record for undo
                        auto keyframes = sceneManager->GetCameraKeyframes();
                        if (!keyframes.empty())
                        {
                            NightSnack::UndoSystem::Instance().Push(
                                NightSnack::CamTriggerCreateSnapshot{ keyframes.back().first }
                            );
                        }
                    }
                    keyframesCopy = sceneManager->GetCameraKeyframes();
                    ImGui::Separator();
                    ImGui::Text("Existing CamTriggers:");
                    for (int i = 0; i < keyframesCopy.size(); ++i)
                    {
                        ImGui::PushID(i);
                        Entity camTriggerEntity = keyframesCopy[i].first;
                        json& bounds = keyframesCopy[i].second;
                        float minX = bounds["minX"];
                        float maxX = bounds["maxX"];
                        float minY = bounds["minY"];
                        float maxY = bounds["maxY"];
                        float zoom = bounds["zoom"];

                        ImGui::Text("CamTrigger %d: \nminX=%.1f, maxX=%.1f, \nminY=%.1f, maxY=%.1f, \nzoom=%.2f", i, minX, maxX, minY, maxY, zoom);
                        Transform& t = m_coord.ModifyComponent<Transform>(camTriggerEntity);
                        Collider& c = m_coord.ModifyComponent<Collider>(camTriggerEntity);

                        // Position
                        float pos[3] = { t.pos.x, t.pos.y, t.pos.z };
                        if (ImGui::InputFloat3("Position", pos)) {
                            t.pos.x = pos[0];
                            t.pos.y = pos[1];
                            t.pos.z = pos[2];
                            bounds["pos"]["x"] = pos[0];
                            bounds["pos"]["y"] = pos[1];
                            bounds["pos"]["z"] = pos[2];
                        }

                        // Scale
                        float scale[2] = { t.scale.x, t.scale.y };
                        if (ImGui::InputFloat2("Scale", scale)) {
                            t.scale.x = scale[0];
                            t.scale.y = scale[1];
                            c.width = scale[0];
                            c.height = scale[1];
                            bounds["scale"]["x"] = scale[0];
                            bounds["scale"]["y"] = scale[1];
                        }

						// Bounds
                        if (ImGui::InputFloat("minX", &minX)) bounds["minX"] = minX;
                        if (ImGui::InputFloat("maxX", &maxX)) bounds["maxX"] = maxX;
                        if (ImGui::InputFloat("minY", &minY)) bounds["minY"] = minY;
                        if (ImGui::InputFloat("maxY", &maxY)) bounds["maxY"] = maxY;
                        if (ImGui::InputFloat("zoom", &zoom)) bounds["zoom"] = zoom;

                        if (ImGui::Button("Delete CamTrigger"))
                        {
                            camTriggerDeleteQueue.push_back(camTriggerEntity);
                        }
                        ImGui::PopID();
                        ImGui::Separator();
                    }
					sceneManager->UpdateCameraKeyframes(keyframesCopy);
                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }

            ImGui::End();

            // Process CamTrigger deletions after ImGui::End()
            if (!camTriggerDeleteQueue.empty())
            {
                auto sceneManager = m_coord.GetSystem<NightSnack::Scene::SceneManager>();
                for (Entity e : camTriggerDeleteQueue)
                {
                    // Capture state before deleting for undo stack
                    Vec3 pos{};
                    Vec2 scale{};
                    json bounds{};

                    if (m_coord.HasComponent<Transform>(e))
                    {
                        auto& t = m_coord.GetComponent<Transform>(e);
                        pos = t.pos;
                        scale = Vec2{ t.scale.x, t.scale.y };
                    }
                    for (auto& [ke, kb] : sceneManager->GetCameraKeyframes())
                    {
                        if (ke == e) { bounds = kb; break; }
                    }

                    NightSnack::UndoSystem::Instance().Push(
                        NightSnack::CamTriggerDeleteSnapshot{ e, pos, scale, bounds }
                    );
                    sceneManager->DeleteCamTrigger(e);
                }
            }
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
            auto prefabName = factory->GetEntityPrefabName(*m_selectedEntity);
            auto transform = m_coord.GetComponent<Transform>(*m_selectedEntity);

            if (m_coord.HasComponent<Transform>(*m_selectedEntity) && m_coord.GetComponent<Transform>(*m_selectedEntity).name == "CamTrigger") {
                auto sceneManager = m_coord.GetSystem<NightSnack::Scene::SceneManager>();
                if (sceneManager)
                {
                    NightSnack::UndoSystem::Instance().Push(
                        NightSnack::EntityDeleteSnapshot{ *m_selectedEntity, prefabName, transform,
                        m_coord.HasComponent<Collider>(*m_selectedEntity)
                            ? std::optional<NightSnack::Collider>(m_coord.GetComponent<Collider>(*m_selectedEntity))
                            : std::nullopt }
                    );
                    sceneManager->DeleteCamTrigger(*m_selectedEntity);
                }
            }
            else {
                // Record entity deletion for undo
                NightSnack::UndoSystem::Instance().Push(
                    NightSnack::EntityDeleteSnapshot{ *m_selectedEntity, prefabName, transform,
                        m_coord.HasComponent<Collider>(*m_selectedEntity)
                            ? std::optional<NightSnack::Collider>(m_coord.GetComponent<Collider>(*m_selectedEntity))
                            : std::nullopt }
                );

                // Remove entity from ECS
                factory->RemoveEntity(*m_selectedEntity);
                m_uiNames->erase(*m_selectedEntity);
                *m_selectedEntity = INVALID_ENTITY;
            }
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

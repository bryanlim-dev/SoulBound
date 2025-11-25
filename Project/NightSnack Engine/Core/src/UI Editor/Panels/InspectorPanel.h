/******************************************************************************
 * File:        InspectorPanel.h
 * Project:     NightSnack
 * Author(s):	Wei Jie Lim (Primary, 100%) - Created and implemented inspector panel
 *
 * Description:
 * Inspector panel showing details of selected entity. Displays components and
 * their properties, allowing real-time editing of parameters.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once
#include "AllPanels.h"
#include <stb_image.h>
#include <filesystem>

namespace NightSnack
{
    class InspectorPanel {
    public:

        /************************************************************************/
        /*!
        \brief
		    Constructor for InspectorPanel.

        \param[in] coord
			- Reference to Coordinator.

		\param[in] selectedEntity
		    - Shared pointer to currently selected entity.

		\param[in] uiNames
		    - Shared pointer to map of entity to UI names.
        */
        /************************************************************************/
        InspectorPanel(Coordinator& coord, std::shared_ptr<Entity> selectedEntity, std::shared_ptr<std::unordered_map<Entity, std::string>> uiNames, Tilemap& tilemap)
			: m_coord(coord), m_selectedEntity(selectedEntity), m_uiNames(uiNames), m_tilemap(tilemap)
        {
            // Default order
            m_components =
            {
                "Transform",
                "Physics",
                "Tilemap",
                "Camera",
                "Rigidbody",
                "Collider",
                "Renderer",
                "Scripting",
                "Animator",
                "Font"
			};
        }

        /************************************************************************/
        /*!
        \brief
		    Draws inspector panel showing details of selected entity.
        */
        /************************************************************************/
        void Draw()
        {
            //ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_Once);
            ImGui::Begin("Inspector");

			// Check if an entity is selected
            if (m_selectedEntity && *m_selectedEntity != INVALID_ENTITY)
            {
                // Draw entity info
                DrawEntityInfo(*m_selectedEntity);
                ImGui::Separator();

                for (size_t i = 0; i < m_components.size(); ++i)
                {
                    const std::string& compName = m_components[i];
                    ImGui::PushID(static_cast<int>(i));

                    // Only show header if the entity has the component
                    bool hasComponent = false;
                    if (compName == "Transform") { hasComponent = m_coord.HasComponent<Transform>(*m_selectedEntity); }
                    if (compName == "Physics")   { hasComponent = true; }
					if (compName == "Tilemap")   { hasComponent = true; }
					if (compName == "Camera")    { hasComponent = m_coord.HasComponent<Camera>(*m_selectedEntity); }
                    if (compName == "Rigidbody") { hasComponent = m_coord.HasComponent<RigidBody>(*m_selectedEntity); }
                    if (compName == "Collider")  { hasComponent = m_coord.HasComponent<Collider>(*m_selectedEntity); }
                    if (compName == "Renderer")  { hasComponent = m_coord.HasComponent<Renderable>(*m_selectedEntity); }
					if (compName == "Scripting") { hasComponent = m_coord.HasComponent<Script>(*m_selectedEntity); }
                    if (compName == "Animator")  { hasComponent = m_coord.HasComponent<Animator>(*m_selectedEntity); }
                    if (compName == "Font")      { hasComponent = m_coord.HasComponent<Font>(*m_selectedEntity); }

                    if (hasComponent)
                    {
                        std::string headerLabel = compName + "##" + std::to_string(i);
                        bool open = ImGui::CollapsingHeader(headerLabel.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

                        // When header is clicked/held
                        if (ImGui::IsItemActive())
                        {
                            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
                            {
                                ImGui::SetDragDropPayload("COMPONENT_HEADER", &i, sizeof(size_t));
                                ImGui::Text("Move %s", compName.c_str());
                                ImGui::EndDragDropSource();
                            }
                        }

						// When header is dropped
                        if (ImGui::BeginDragDropTarget())
                        {
                            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("COMPONENT_HEADER"))
                            {
                                size_t srcIdx = *(const size_t*)payload->Data;
                                if (srcIdx != i)
                                {
                                    std::string moved = m_components[srcIdx];
                                    m_components.erase(m_components.begin() + srcIdx);
                                    size_t insertIdx = (srcIdx < i) ? i : i;
                                    m_components.insert(m_components.begin() + insertIdx, moved);
                                }
                            }
                            ImGui::EndDragDropTarget();
                        }

						// Draw components
                        if (open)
                        {
                            if (compName == "Transform") { TransformComponent(); }
                            if (compName == "Physics")   { PhysicsComponent(); }
							if (compName == "Tilemap")   { TilemapComponent(); }
							if (compName == "Camera")    { CameraComponent(); }
                            if (compName == "Rigidbody") { RigidBodyComponent(); }
                            if (compName == "Collider")  { ColliderComponent(); }
                            if (compName == "Renderer")  { RendererComponent(); }
							if (compName == "Scripting") { ScriptingComponent(); }
                            if (compName == "Animator")  { AnimatorComponent(); }
                            if (compName == "Font")      { FontComponent(); }
                        }
                        ImGui::Separator();
                    }

                    ImGui::PopID();
                }

                AddComponentButton();
                RemoveComponentButton();
            }
            else
            {
                ImGui::Text("No entity selected.");
            }

            ImGui::End();
        }

    private:
        Coordinator& m_coord;
        std::shared_ptr<Entity> m_selectedEntity;
        std::shared_ptr<std::unordered_map<Entity, std::string>> m_uiNames;

        std::vector<std::string> m_components;

		Tilemap& m_tilemap;

        /************************************************************************/
        /*!
        \brief
		    Draws basic information of the entity such as name and ID.

		\param[in] entity
		    - The entity to draw info for.
        */
        /************************************************************************/
        void DrawEntityInfo(Entity entity);

        /************************************************************************/
        /*!
        \brief
			Draws Transform component details which includes position, rotation,
            and scale.
        */
        /************************************************************************/
        void TransformComponent();

        /************************************************************************/
        /*!
        \brief
		    Draws Physics component details which includes mass, drag, and other
			physics-related properties.
        */
        /************************************************************************/
        void PhysicsComponent();

        /************************************************************************/
        /*!
        \brief
			Draws Tilemap component details which displays tilemap overlay in Scene.
        */
        /************************************************************************/
        void TilemapComponent();

        /************************************************************************/
        /*!
        \brief
            Draws Camera component details which includes camera type, zoom,
			and other camera-related properties.
        */
        /************************************************************************/
        void CameraComponent();

        /************************************************************************/
        /*!
        \brief
		    Draws RigidBody component details which includes velocity, angular
        */
        /************************************************************************/
		void RigidBodyComponent();

        /************************************************************************/
        /*!
        \brief
            Draws Collider component details which includes collider type, size,
			and other collider-related properties.
        */
        /************************************************************************/
		void ColliderComponent();

        /************************************************************************/
        /*!
        \brief
		    Draws Renderer component details which includes mesh, texture, and
			other rendering-related properties.
        */
        /************************************************************************/
		void RendererComponent();

        /************************************************************************/
        /*!
        \brief
            Draws Animator component details which includes animation clips, 
            speed, and other animation-related properties.
        */
        /************************************************************************/
        void AnimatorComponent();

        /************************************************************************/
        /*!
        \brief
            Draws Font component details which includes font size, type, and
            other font-related properties.
        */
        /************************************************************************/
        void FontComponent();

        /************************************************************************/
        /*!
        \brief
            Draws Scripting component details which includes script name, and
            other scripting-related properties.
        */
        /************************************************************************/
        void ScriptingComponent();

        /************************************************************************/
        /*!
        \brief
			Adds a button to add new components to the selected entity.
        */
        /************************************************************************/
        void AddComponentButton();

        /************************************************************************/
        /*!
        \brief
            Adds a button to add new components to the selected entity.
        */
        /************************************************************************/
        void RemoveComponentButton();
    };

	// Draws basic information of the entity such as name and ID.
    inline void InspectorPanel::DrawEntityInfo(Entity entity)
    {
        auto factory = m_coord.GetSystem<EntityFactory>();

		// Retrieve Prefab name for the entity
        if (m_uiNames->find(entity) == m_uiNames->end()) {
            (*m_uiNames)[entity] = factory ? factory->GetEntityPrefabName(entity) : "Entity " + std::to_string(entity);
        }
		// Input field for entity name
        char nameBuffer[128];
        strncpy_s(nameBuffer, (*m_uiNames)[entity].c_str(), sizeof(nameBuffer));
        nameBuffer[sizeof(nameBuffer) - 1] = 0;

        ImGui::PushID("EntityNameField");
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 80.0f);
        if (ImGui::InputText("##EntityName", nameBuffer, sizeof(nameBuffer))) {
            (*m_uiNames)[entity] = std::string(nameBuffer);
        }
        ImGui::SameLine();
        ImGui::TextDisabled("ID: %u", entity);
        ImGui::PopID();

		// Layer selection dropdown
        if (m_coord.HasComponent<Transform>(entity))
        {
            auto& t = m_coord.ModifyComponent<Transform>(entity);
            const char* layerNames[] = { "_ENGINE", "BACKGROUND", "GAMELAYER", "FOREGROUND", "GAMEUI", "_ENGINE2"};
            int currentLayer = static_cast<int>(t.layer);

            ImGui::Text("Layer");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(120.0f);
            if (ImGui::Combo("##LayerCombo", &currentLayer, layerNames, IM_ARRAYSIZE(layerNames))) {
                //std::cout << "original: " << static_cast<int>(t.layer) << ", new: " << currentLayer << std::endl;
                t.layer = static_cast<LayerType>(currentLayer);
                m_coord.BufferEntitySignature(entity);
				//std::cout << "Now: " << static_cast<int>(t.layer) << std::endl;
            }
        }

		// Prefab of the entity

    }

	// Draws Transform component details which includes position, rotation, and scale.
    inline void InspectorPanel::TransformComponent()
    {
        auto& t = m_coord.ModifyComponent<Transform>(*m_selectedEntity);

        if (ImGui::BeginTable("TransformTable", 3, ImGuiTableFlags_SizingFixedFit))
        {
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthFixed, 60.0f);

            // Header row
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(1); ImGui::Text("X");
            ImGui::TableSetColumnIndex(2); ImGui::Text("Y");

            // Display the respective position based on its entity layer
            float pos[2];
            float scale[2];
            if (t.layer == LayerType::GAMEUI) {
                pos[0] = t.UIbasePos.x; pos[1] = t.UIbasePos.y;
                scale[0] = t.UIbaseScale.x; scale[1] = t.UIbaseScale.y;
            } else {
                pos[0] = t.pos.x; pos[1] = t.pos.y;
                scale[0] = t.scale.x; scale[1] = t.scale.y;
            }

            // Position row
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Position");
            ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(-FLT_MIN); 
            ImGui::InputFloat("##posx", &pos[0], 0.0f, 0.0f, "%.2f");
            ImGui::TableSetColumnIndex(2); ImGui::SetNextItemWidth(-FLT_MIN); 
            ImGui::InputFloat("##posy", &pos[1], 0.0f, 0.0f, "%.2f");

            // Rotation row
            float rot = t.rot;
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Rotation");
            ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(-FLT_MIN); 
            ImGui::InputFloat("##rot", &rot, 0.0f, 0.0f, "%.2f");
            t.rot = rot;

            // Scale row
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Scale");
            ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(-FLT_MIN); 
            ImGui::InputFloat("##scalex", &scale[0], 0.0f, 0.0f, "%.2f");
            ImGui::TableSetColumnIndex(2); ImGui::SetNextItemWidth(-FLT_MIN); 
            ImGui::InputFloat("##scaley", &scale[1], 0.0f, 0.0f, "%.2f");

            // Update the respective position based on its entity layer
            if (t.layer == LayerType::GAMEUI) {
                t.UIbasePos.x = pos[0]; t.UIbasePos.y = pos[1];
                t.UIbaseScale.x = scale[0]; t.UIbaseScale.y = scale[1];
            } else {
                t.pos.x = pos[0]; t.pos.y = pos[1];
                t.scale.x = scale[0]; t.scale.y = scale[1];
            }

            ImGui::EndTable();
        }
	}

	// Draws Physics component details which includes mass, drag, and other physics-related properties.
    inline void InspectorPanel::PhysicsComponent()
    {
        auto ps = m_coord.GetSystem<PhysicsSystem>();
        if (!ps) return;

        if (ImGui::BeginTable("PhysicsSettingsTable", 2, ImGuiTableFlags_SizingFixedFit))
        {
            ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, 60.0f);

            // Gravity
            float gravity = ps->GetGravity();
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Gravity");
            ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::InputFloat("##gravity", &gravity, 0.0f, 0.0f, "%.2f")) { ps->SetGravity(gravity); }

			// Drag
            float drag = ps->GetDrag();
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Drag");
            ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::InputFloat("##drag", &drag, 0.0f, 0.0f, "%.2f")) { ps->SetDrag(drag); }

			// Friction
            float friction = ps->GetFriction();
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Friction");
            ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::InputFloat("##friction", &friction, 0.0f, 0.0f, "%.2f")) { ps->SetFriction(friction); }

            ImGui::EndTable();
        }
    }

	// Draws Tilemap component details which displays tilemap overlay in Scene.
    inline void InspectorPanel::TilemapComponent()
    {
        m_tilemap.DrawTilemapSettings();
        m_tilemap.DrawTileEditor(m_coord, *m_selectedEntity);
    }

	// Draws RigidBody component details which includes velocity, angular
    inline void InspectorPanel::RigidBodyComponent()
    {
        if (!m_coord.HasComponent<RigidBody>(*m_selectedEntity))
            return;

        auto& rb = m_coord.ModifyComponent<RigidBody>(*m_selectedEntity);

        if (ImGui::BeginTable("RigidBodyTable", 3, ImGuiTableFlags_SizingFixedFit))
        {
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthFixed, 80.0f);

            // Mass
            float mass = 1.f / rb.invMass;
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Mass");
            ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(-FLT_MIN); ImGui::InputFloat("##mass", &mass, 0.0f, 0.0f, "%.2f");
            rb.invMass = 1.f / mass;

            // Velocity
            float vel[2] = { rb.vel.x, rb.vel.y };
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Velocity");
            ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(-FLT_MIN); ImGui::Text("X"); ImGui::SameLine(); ImGui::InputFloat("##velx", &vel[0], 0.0f, 0.0f, "%.2f");
            ImGui::TableSetColumnIndex(2); ImGui::SetNextItemWidth(-FLT_MIN); ImGui::Text("Y"); ImGui::SameLine(); ImGui::InputFloat("##vely", &vel[1], 0.0f, 0.0f, "%.2f");
            rb.vel.x = vel[0]; rb.vel.y = vel[1];

            // Acceleration
            float accel[2] = { rb.accel.x, rb.accel.y };
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Acceleration");
            ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(-FLT_MIN); ImGui::Text("X"); ImGui::SameLine(); ImGui::InputFloat("##accelx", &accel[0], 0.0f, 0.0f, "%.2f");
            ImGui::TableSetColumnIndex(2); ImGui::SetNextItemWidth(-FLT_MIN); ImGui::Text("Y"); ImGui::SameLine(); ImGui::InputFloat("##accely", &accel[1], 0.0f, 0.0f, "%.2f");
            rb.accel.x = accel[0]; rb.accel.y = accel[1];

            // UseGravity
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("UseGravity");
            ImGui::TableSetColumnIndex(1); ImGui::Checkbox("##useGravity", &rb.useGravity);

            ImGui::EndTable();
        }
    }

    // Draws Camera component details which includes camera type, zoom,
    inline void InspectorPanel::CameraComponent()
    {
        if (!m_coord.HasComponent<Camera>(*m_selectedEntity))
            return;
        auto& cam = m_coord.ModifyComponent<Camera>(*m_selectedEntity);

        if (ImGui::BeginTable("CameraTable", 2, ImGuiTableFlags_SizingFixedFit))
        {
            ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, 100.0f);

            // Is Main Camera
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Is Main Cam");
            ImGui::TableSetColumnIndex(1);
            ImGui::Checkbox("##IsMainCam", &cam.isMainCam);

            // Zoom
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Zoom");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::InputFloat("##Zoom", &cam.zoom, 0.1f, 1.0f, "%.2f");

            // Width
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Width");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::InputFloat("##Width", &cam.width, 1.0f, 10.0f, "%.0f");

            // Height
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Height");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::InputFloat("##Height", &cam.height, 1.0f, 10.0f, "%.0f");

            ImGui::EndTable();
        }
    }

	// Draws Collider component details which includes collider type, size, and other collider-related properties.
    inline void InspectorPanel::ColliderComponent()
    {
        if (!m_coord.HasComponent<Collider>(*m_selectedEntity))
            return;

        auto& c = m_coord.ModifyComponent<Collider>(*m_selectedEntity);

        if (ImGui::BeginTable("ColliderTable", 2, ImGuiTableFlags_SizingFixedFit))
        {
            ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, 80.0f);

            // Shape type
            const char* shapeTypes[] = { "AABB", "Circle" };
            int shapeIdx = (c.shape == ShapeType::AABB) ? 0 : 1;
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Shape");
            ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::Combo("##Shape", &shapeIdx, shapeTypes, IM_ARRAYSIZE(shapeTypes)))
                c.shape = (shapeIdx == 0) ? ShapeType::AABB : ShapeType::Circle;

            // Collider type
            const char* colliderTypes[] = { "Solid", "Trigger", "Ground" };
            int typeIdx = (c.type == ColliderType::Solid) ? 0 : (c.type == ColliderType::Trigger) ? 1 : 2;
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Type");
            ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::Combo("##Type", &typeIdx, colliderTypes, IM_ARRAYSIZE(colliderTypes)))
                c.type = (typeIdx == 0) ? ColliderType::Solid : (typeIdx == 1) ? ColliderType::Trigger : ColliderType::Ground;

            // Is Static
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Is Static");
            ImGui::TableSetColumnIndex(1); ImGui::Checkbox("##isStatic", &c.isStatic);

            // Shape data
            if (c.shape == ShapeType::AABB)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Width");
                ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(-FLT_MIN); ImGui::InputFloat("##Width", &c.width, 0.0f, 0.0f, "%.2f");

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Height");
                ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(-FLT_MIN); ImGui::InputFloat("##Height", &c.height, 0.0f, 0.0f, "%.2f");
            }
            else if (c.shape == ShapeType::Circle)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Radius");
                ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(-FLT_MIN); ImGui::InputFloat("##Radius", &c.radius, 0.0f, 0.0f, "%.2f");
            }

            // Collision state (read-only)
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Is Grounded");
            ImGui::TableSetColumnIndex(1); ImGui::Text("%s", c.isGrounded ? "True" : "False");

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Colliding");
            ImGui::TableSetColumnIndex(1); ImGui::Text("%s", c.colliding ? "True" : "False");

            ImGui::EndTable();
        }
    }

	// Draws Renderer component details which includes mesh, texture, and other rendering-related properties.
    inline void InspectorPanel::RendererComponent()
    {
        if (!m_coord.HasComponent<Renderable>(*m_selectedEntity))
            return;

        auto& renderable = m_coord.GetComponent<Renderable>(*m_selectedEntity);
        auto graphics = m_coord.GetSystem<GraphicsSystem>();

        if (ImGui::BeginTable("RendererTable", 2, ImGuiTableFlags_SizingFixedFit))
        {
            ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, 120.0f);

			// Mesh Name (read-only for now)
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Mesh");
            ImGui::TableSetColumnIndex(1); ImGui::TextDisabled("%s", renderable.meshName.c_str());

			// Texture with drag-and-drop
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Texture");
            ImGui::TableSetColumnIndex(1);
			// Get the actual texture name from the material's texture
            std::string textureName = "None";
            Material* mat = graphics ? graphics->GetMaterial(renderable.materialName) : nullptr;
            if (mat && mat->GetTexture())
            {
                std::filesystem::path texPath(mat->GetTexture()->GetTextureName());
                textureName = texPath.filename().string();
            }
            ImGui::TextColored(ImVec4(0.2f, 0.6f, 1.0f, 1.0f), "%s", textureName.c_str());
			// Accept drag-and-drop from AssetsPanel
            ImVec2 rect_min = ImGui::GetItemRectMin();
            ImVec2 rect_max = ImGui::GetItemRectMax();
            if (ImGui::BeginDragDropTarget())
            {
                ImGui::GetWindowDrawList()->AddRectFilled(rect_min, rect_max, IM_COL32(50, 150, 255, 80));
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_IMAGE")) {
                    const char* droppedPath = static_cast<const char*>(payload->Data);
                    if (graphics)
                    {
                        stbi_set_flip_vertically_on_load(true);
                        graphics->UpdateTextureOfMaterial(droppedPath, *m_selectedEntity);
                    }
                }
                ImGui::EndDragDropTarget();
            }

			// isWireframe checkbox
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Wireframe");
            bool wireframe = renderable.isWireframe;
            ImGui::TableSetColumnIndex(1); ImGui::Checkbox("##Wireframe", &wireframe);
            if (wireframe != renderable.isWireframe) {
                m_coord.ModifyComponent<Renderable>(*m_selectedEntity).isWireframe = wireframe;
            }
			// wasModified checkbox
            //ImGui::TableNextRow();
            //ImGui::TableSetColumnIndex(0); ImGui::Text("Was Modified");
            //ImGui::TableSetColumnIndex(1); ImGui::Checkbox("##WasModified", &renderable.wasModified);

            ImGui::EndTable();
        }
    }

	// Draws Animator component details which includes animation clips, speed, and other animation-related properties.
    inline void InspectorPanel::AnimatorComponent()
    {
        if (!m_coord.HasComponent<Animator>(*m_selectedEntity))
            return;

        auto& animator = m_coord.GetComponent<Animator>(*m_selectedEntity);

        if (ImGui::BeginTable("AnimatorTable", 2, ImGuiTableFlags_SizingFixedFit))
        {
            ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 140.0f);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, 130.0f);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Current Animation Clip");
            ImGui::TableSetColumnIndex(1); ImGui::Text("%s", animator.currentAnimationClip.c_str());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Sprite Sheet");
            ImGui::TableSetColumnIndex(1); ImGui::Text("%s", animator.spriteSheetName.c_str());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Current Frame Index");
            ImGui::TableSetColumnIndex(1); ImGui::Text("%d", animator.currentFrameIndex);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Current Time");
            ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f", animator.currentTime);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Playback Speed");
            ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f", animator.playbackSpeed);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Is Playing");
            ImGui::TableSetColumnIndex(1); ImGui::Text("%s", animator.isPlaying ? "True" : "False");

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Is Looping");
            ImGui::TableSetColumnIndex(1); ImGui::Text("%s", animator.isLooping ? "True" : "False");

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Last Facing Right");
            ImGui::TableSetColumnIndex(1); ImGui::Text("%s", animator.lastFacingRight ? "True" : "False");

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("UV Top Left");
            ImGui::TableSetColumnIndex(1); ImGui::Text("(%.2f, %.2f)", animator.uvTopLeft.x, animator.uvTopLeft.y);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("UV Bottom Right");
            ImGui::TableSetColumnIndex(1); ImGui::Text("(%.2f, %.2f)", animator.uvBottomRight.x, animator.uvBottomRight.y);

            ImGui::EndTable();
        }
    }

	// Draws Font component details which includes font size, type, and other font-related properties.
    inline void InspectorPanel::FontComponent()
    {
        if (!m_coord.HasComponent<Font>(*m_selectedEntity))
            return;

        auto& fontComp = m_coord.ModifyComponent<Font>(*m_selectedEntity);

        if (ImGui::BeginTable("FontTable", 2, ImGuiTableFlags_SizingFixedFit))
        {
            ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, 80.0f);

			// Font Type (read-only)
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Font Type");
            ImGui::TableSetColumnIndex(1); ImGui::TextDisabled("%s", fontComp.fontType.c_str());

			// Font Text (modifiable)
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Text");
            ImGui::TableSetColumnIndex(1);
            char textBuffer[256];
            strncpy_s(textBuffer, fontComp.text.c_str(), sizeof(textBuffer));
            textBuffer[sizeof(textBuffer) - 1] = 0;
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::InputText("##FontText", textBuffer, sizeof(textBuffer)))
            {
                fontComp.text = std::string(textBuffer);
            }

			// Font Size
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Font Size");
            ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::InputFloat("##FontSize", &fontComp.fontSize, 0.0f, 0.0f, "%.2f");
			// Text Color box
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Text Color");
            ImGui::TableSetColumnIndex(1);
            ImGui::ColorEdit3("##FontColor", reinterpret_cast<float*>(&fontComp.textColor), ImGuiColorEditFlags_NoInputs);

            ImGui::EndTable();
        }
    }

	// Draws Scripting component details which includes script name, and other scripting-related properties.
    inline void InspectorPanel::ScriptingComponent()
    {
        if (!m_coord.HasComponent<Script>(*m_selectedEntity))
            return;

        auto& script = m_coord.GetComponent<Script>(*m_selectedEntity);

        if (ImGui::BeginTable("ScriptTable", 1 + script.names.size(), ImGuiTableFlags_SizingFixedFit))
        {
            ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, 120.0f);

            ImGui::TableNextRow();
            int indexCount = 0;
            ImGui::TableSetColumnIndex(indexCount); ImGui::Text("Script");
            for (std::string names : script.names)
            {
                indexCount++;
                ImGui::TableSetColumnIndex(indexCount); ImGui::TextDisabled("%s", names.c_str());
            }

            ImGui::EndTable();
        }
    }

    inline void InspectorPanel::AddComponentButton()
    {
        // Center the button horizontally
        ImGui::Dummy(ImVec2(0.0f, 20.0f));
        float panelWidth = ImGui::GetWindowSize().x;
        float buttonWidth = 150.0f;
        ImGui::SetCursorPosX((panelWidth - buttonWidth) * 0.5f);

        // Build a list of missing components
        struct CompInfo { const char* name; bool has; std::function<void()> add; };
        std::vector<CompInfo> missing;
        Entity e = *m_selectedEntity;
        if (!m_coord.HasComponent<Transform>(e))   missing.push_back({ "Transform", false, [this, e] { m_coord.AddComponent<Transform>(e, Transform{}); } });
        if (!m_coord.HasComponent<RigidBody>(e))   missing.push_back({ "Rigidbody", false, [this, e] { m_coord.AddComponent<RigidBody>(e, RigidBody{}); } });
        if (!m_coord.HasComponent<Collider>(e))    missing.push_back({ "Collider", false, [this, e] { m_coord.AddComponent<Collider>(e, Collider{}); } });
        if (!m_coord.HasComponent<Renderable>(e))  missing.push_back({ "Renderer", false, [this, e] { m_coord.AddComponent<Renderable>(e, Renderable{}); } });
		if (!m_coord.HasComponent<Script>(e))      missing.push_back({ "Scripting", false, [this, e] { m_coord.AddComponent<Script>(e, Script{}); } });
        if (!m_coord.HasComponent<Animator>(e))    missing.push_back({ "Animator", false, [this, e] { m_coord.AddComponent<Animator>(e, Animator{}); } });
        if (!m_coord.HasComponent<Font>(e))        missing.push_back({ "Font", false, [this, e] { m_coord.AddComponent<Font>(e, Font{}); } });

        // Disable button if no components left to add
        if (missing.empty())
        {
            ImGui::BeginDisabled();
            ImGui::Button("Add Component", ImVec2(buttonWidth, 0));
            ImGui::EndDisabled();
            return;
        }

        ImVec2 buttonPos = ImGui::GetCursorScreenPos();
        if (ImGui::Button("Add Component", ImVec2(buttonWidth, 0)))
        {
            float availableBelow = ImGui::GetWindowPos().y + ImGui::GetWindowSize().y - (buttonPos.y + ImGui::GetFrameHeight());
            float popupHeight = ImGui::GetFrameHeight() * static_cast<float>(missing.size());

            if (availableBelow < popupHeight)
            {
                // Anchor popup's bottom to button's bottom
                ImGui::SetNextWindowPos(ImVec2(buttonPos.x, buttonPos.y + ImGui::GetFrameHeight() - popupHeight), ImGuiCond_Appearing);
            }
            else
            {
                // Default: show below
                ImGui::SetNextWindowPos(ImVec2(buttonPos.x, buttonPos.y + ImGui::GetFrameHeight()), ImGuiCond_Appearing);
            }
            ImGui::OpenPopup("AddComponentPopup");
        }

        // Lambda to handle menu item creation and component addition
        auto addMenuItem = [this](const char* name, auto addFunc, bool hasComponent) {
            if (!hasComponent) {
                if (ImGui::MenuItem(name)) {
                    addFunc();
                    ImGui::CloseCurrentPopup();
                }
            }
        };

        if (ImGui::BeginPopup("AddComponentPopup"))
        {
            addMenuItem("Transform", [this, e] { m_coord.AddComponent<Transform>(e, Transform{}); }, m_coord.HasComponent<Transform>(e));
            addMenuItem("Rigidbody", [this, e] { m_coord.AddComponent<RigidBody>(e, RigidBody{}); }, m_coord.HasComponent<RigidBody>(e));
            addMenuItem("Collider", [this, e] { m_coord.AddComponent<Collider>(e, Collider{}); }, m_coord.HasComponent<Collider>(e));
            addMenuItem("Renderer", [this, e] { m_coord.AddComponent<Renderable>(e, Renderable{}); }, m_coord.HasComponent<Renderable>(e));
            addMenuItem("Scripting", [this, e] { m_coord.AddComponent<Script>(e, Script{}); }, m_coord.HasComponent<Script>(e));
            addMenuItem("Animator", [this, e] { m_coord.AddComponent<Animator>(e, Animator{}); }, m_coord.HasComponent<Animator>(e));
            addMenuItem("Font", [this, e] { m_coord.AddComponent<Font>(e, Font{}); }, m_coord.HasComponent<Font>(e));
            ImGui::EndPopup();
        }
	}

    inline void InspectorPanel::RemoveComponentButton()
    {
        // Center the button horizontally
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        float panelWidth = ImGui::GetWindowSize().x;
        float buttonWidth = 150.0f;
        ImGui::SetCursorPosX((panelWidth - buttonWidth) * 0.5f);

        // Build a list of removable components (exclude Transform and Physics)
        struct CompInfo { const char* name; bool has; std::function<void()> remove; };
        std::vector<CompInfo> removable;
        Entity e = *m_selectedEntity;
        if (m_coord.HasComponent<RigidBody>(e))   removable.push_back({ "Rigidbody", true, [this, e] { m_coord.RemoveComponent<RigidBody>(e); } });
        if (m_coord.HasComponent<Collider>(e))    removable.push_back({ "Collider", true, [this, e] { m_coord.RemoveComponent<Collider>(e); } });
        if (m_coord.HasComponent<Renderable>(e))  removable.push_back({ "Renderer", true, [this, e] { m_coord.RemoveComponent<Renderable>(e); } });
        if (m_coord.HasComponent<Script>(e))      removable.push_back({ "Scripting", true, [this, e] { m_coord.RemoveComponent<Script>(e); } });
        if (m_coord.HasComponent<Animator>(e))    removable.push_back({ "Animator", true, [this, e] { m_coord.RemoveComponent<Animator>(e); } });
        if (m_coord.HasComponent<Font>(e))        removable.push_back({ "Font", true, [this, e] { m_coord.RemoveComponent<Font>(e); } });

        // Disable button if no components left to remove
        if (removable.empty())
        {
            ImGui::BeginDisabled();
            ImGui::Button("Remove Component", ImVec2(buttonWidth, 0));
            ImGui::EndDisabled();
            return;
        }

        ImVec2 buttonPos = ImGui::GetCursorScreenPos();
        if (ImGui::Button("Remove Component", ImVec2(buttonWidth, 0)))
        {
            float availableBelow = ImGui::GetWindowPos().y + ImGui::GetWindowSize().y - (buttonPos.y + ImGui::GetFrameHeight());
            float popupHeight = ImGui::GetFrameHeight() * static_cast<float>(removable.size());

            if (availableBelow < popupHeight)
            {
                // Anchor popup's bottom to button's bottom
                ImGui::SetNextWindowPos(ImVec2(buttonPos.x, buttonPos.y + ImGui::GetFrameHeight() - popupHeight), ImGuiCond_Appearing);
            }
            else
            {
                // Default: show below
                ImGui::SetNextWindowPos(ImVec2(buttonPos.x, buttonPos.y + ImGui::GetFrameHeight()), ImGuiCond_Appearing);
            }
            ImGui::OpenPopup("RemoveComponentPopup");
        }

        // Lambda to handle menu item creation and component removal
        auto removeMenuItem = [this](const char* name, auto removeFunc, bool hasComponent) {
            if (hasComponent) {
                if (ImGui::MenuItem(name)) {
                    removeFunc();
                    ImGui::CloseCurrentPopup();
                }
            }
            };

        if (ImGui::BeginPopup("RemoveComponentPopup"))
        {
            for (const auto& info : removable)
            {
                removeMenuItem(info.name, info.remove, info.has);
            }
            ImGui::EndPopup();
        }
    }

} // NightSnack

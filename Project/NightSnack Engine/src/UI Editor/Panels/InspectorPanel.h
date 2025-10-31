/******************************************************************************
 * File:        InspectorPanel.h
 * Project:     NightSnack
 * Author(s):	
 *
 * Description:
 * Inspector panel showing details of selected entity.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once
#include "AllPanels.h"

namespace NightSnack
{
    class InspectorPanel {
    public:
        InspectorPanel(Coordinator& coord, std::shared_ptr<Entity> selectedEntity, std::unordered_map<Entity, std::string>& uiNames)
            : m_coord(coord), m_selectedEntity(selectedEntity), m_uiNames(uiNames) {}

        void Draw()
        {
            //ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_Once);
            ImGui::Begin("Inspector");

            if (m_selectedEntity && *m_selectedEntity != INVALID_ENTITY)
            {
                DrawEntityInfo(*m_selectedEntity); // Draw name/layer section
                ImGui::Separator();

				TransformComponent();

                PhysicsComponent();

                RigidBodyComponent();

                ColliderComponent();

				RendererComponent();
                // Add more components as needed...
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
		std::unordered_map<Entity, std::string>& m_uiNames;

        void DrawEntityInfo(Entity entity);
        void TransformComponent();
        void PhysicsComponent();
		void RigidBodyComponent();
		void ColliderComponent();
		void RendererComponent();
    };

    inline void InspectorPanel::DrawEntityInfo(Entity entity)
    {
        auto factory = m_coord.GetSystem<EntityFactory>();

        if (m_uiNames.find(entity) == m_uiNames.end()) {
            m_uiNames[entity] = factory ? factory->GetEntityPrefabName(entity) : "Entity " + std::to_string(entity);
        }

        char nameBuffer[128];
        strncpy_s(nameBuffer, m_uiNames[entity].c_str(), sizeof(nameBuffer));
        nameBuffer[sizeof(nameBuffer) - 1] = 0;

        ImGui::PushID("EntityNameField");
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 80.0f);
        if (ImGui::InputText("##EntityName", nameBuffer, sizeof(nameBuffer))) {
            m_uiNames[entity] = std::string(nameBuffer);
        }
        ImGui::SameLine();
        ImGui::TextDisabled("ID: %u", entity);
        ImGui::PopID();

        if (m_coord.HasComponent<Transform>(entity))
        {
            auto& t = m_coord.ModifyComponent<Transform>(entity);
            const char* layerNames[] = { "ENGINE", "BACKGROUND", "GAMELAYER", "FOREGROUND", "GAMEUI" };
            int currentLayer = static_cast<int>(t.layer);

            ImGui::Text("Layer");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(120.0f);
            if (ImGui::Combo("##LayerCombo", &currentLayer, layerNames, IM_ARRAYSIZE(layerNames))) {
                t.layer = static_cast<LayerType>(currentLayer);
            }
        }
    }

    inline void InspectorPanel::TransformComponent()
    {
        auto& t = m_coord.ModifyComponent<Transform>(*m_selectedEntity);

        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::BeginTable("TransformTable", 3, ImGuiTableFlags_SizingFixedFit))
            {
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 80.0f); // Label
                ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthFixed, 60.0f);
                ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthFixed, 60.0f);

                // Header row
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(1); ImGui::Text("X");
                ImGui::TableSetColumnIndex(2); ImGui::Text("Y");

                // Position row
                float pos[2] = { t.pos.x, t.pos.y };
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Position");
                ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(-FLT_MIN); ImGui::InputFloat("##posx", &pos[0], 0.0f, 0.0f, "%.2f");
                ImGui::TableSetColumnIndex(2); ImGui::SetNextItemWidth(-FLT_MIN); ImGui::InputFloat("##posy", &pos[1], 0.0f, 0.0f, "%.2f");
                t.pos.x = pos[0]; t.pos.y = pos[1];

                // Rotation row
                float rot = t.rot;
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Rotation");
                ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(-FLT_MIN); ImGui::InputFloat("##rot", &rot, 0.0f, 0.0f, "%.2f");
                t.rot = rot;

                // Scale row
                float scale[2] = { t.scale.x, t.scale.y };
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Scale");
                ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(-FLT_MIN); ImGui::InputFloat("##scalex", &scale[0], 0.0f, 0.0f, "%.2f");
                ImGui::TableSetColumnIndex(2); ImGui::SetNextItemWidth(-FLT_MIN); ImGui::InputFloat("##scaley", &scale[1], 0.0f, 0.0f, "%.2f");
                t.scale.x = scale[0]; t.scale.y = scale[1];

                ImGui::EndTable();
            }
        }
        ImGui::Separator();
	}

    inline void InspectorPanel::PhysicsComponent()
    {
        auto ps = m_coord.GetSystem<PhysicsSystem>();
        if (!ps)
            return;

        if (ImGui::CollapsingHeader("Physics Settings", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::BeginTable("PhysicsSettingsTable", 2, ImGuiTableFlags_SizingFixedFit))
            {
                ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, 120.0f);

                // Gravity row
                float gravity = ps->GetGravity();
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Gravity");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (ImGui::InputFloat("##gravity", &gravity, 0.0f, 0.0f, "%.2f"))
                    ps->SetGravity(gravity);

                // Drag row
                float drag = ps->GetDrag();
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Drag");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (ImGui::InputFloat("##drag", &drag, 0.0f, 0.0f, "%.2f"))
                    ps->SetDrag(drag);

                // Friction row
                float friction = ps->GetFriction();
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("Friction");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (ImGui::InputFloat("##friction", &friction, 0.0f, 0.0f, "%.2f"))
                    ps->SetFriction(friction);

                ImGui::EndTable();
            }
        }
        ImGui::Separator();
    }

    inline void InspectorPanel::RigidBodyComponent()
    {
        if (!m_coord.HasComponent<RigidBody>(*m_selectedEntity))
            return;

        auto& rb = m_coord.ModifyComponent<RigidBody>(*m_selectedEntity);

        if (ImGui::CollapsingHeader("Rigidbody", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // Mass
            ImGui::Text("Mass");
            ImGui::SetNextItemWidth(120);
            float mass = 1.f / rb.invMass;
            ImGui::InputFloat("##mass", &mass, 0.0f, 0.0f, "%.2f");
            rb.invMass = 1.f / mass;

            // Velocity
            float vel[2] = { rb.vel.x, rb.vel.y };
            ImGui::Text("Velocity");
            ImGui::SetNextItemWidth(60);
            ImGui::InputFloat("X##vel", &vel[0], 0.0f, 0.0f, "%.2f");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(60);
            ImGui::InputFloat("Y##vel", &vel[1], 0.0f, 0.0f, "%.2f");
            rb.vel.x = vel[0];
            rb.vel.y = vel[1];

            // Acceleration
            float accel[2] = { rb.accel.x, rb.accel.y };
            ImGui::Text("Acceleration");
            ImGui::SetNextItemWidth(60);
            ImGui::InputFloat("X##accel", &accel[0], 0.0f, 0.0f, "%.2f");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(60);
            ImGui::InputFloat("Y##accel", &accel[1], 0.0f, 0.0f, "%.2f");
            rb.accel.x = accel[0];
            rb.accel.y = accel[1];

            // UseGravity
            ImGui::Text("UseGravity");
            ImGui::SameLine();
            ImGui::Checkbox("##useGravity", &rb.useGravity);
        }
        ImGui::Separator();
	}

    inline void InspectorPanel::ColliderComponent()
    {
        if (!m_coord.HasComponent<Collider>(*m_selectedEntity))
            return;

        auto& c = m_coord.ModifyComponent<Collider>(*m_selectedEntity);

        if (ImGui::CollapsingHeader("Collider", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // Shape type
            const char* shapeTypes[] = { "AABB", "Circle" };
            int shapeIdx = (c.shape == ShapeType::AABB) ? 0 : 1;
            if (ImGui::Combo("Shape", &shapeIdx, shapeTypes, IM_ARRAYSIZE(shapeTypes)))
            {
                c.shape = (shapeIdx == 0) ? ShapeType::AABB : ShapeType::Circle;
            }

            // Collider type
            const char* colliderTypes[] = { "Solid", "Trigger", "Ground" };
            int typeIdx = (c.type == ColliderType::Solid) ? 0 : (c.type == ColliderType::Trigger) ? 1 : 2;
            if (ImGui::Combo("Type", &typeIdx, colliderTypes, IM_ARRAYSIZE(colliderTypes)))
            {
                c.type = (typeIdx == 0) ? ColliderType::Solid : (typeIdx == 1) ? ColliderType::Trigger : ColliderType::Ground;
            }

            // Is Static
            ImGui::Checkbox("Is Static", &c.isStatic);

            // Shape data
            if (c.shape == ShapeType::AABB)
            {
                ImGui::InputFloat("Width", &c.width, 0.0f, 0.0f, "%.2f");
                ImGui::InputFloat("Height", &c.height, 0.0f, 0.0f, "%.2f");
            }
            else if (c.shape == ShapeType::Circle)
            {
                ImGui::InputFloat("Radius", &c.radius, 0.0f, 0.0f, "%.2f");
            }

            // Collision state (read-only)
            ImGui::Text("Is Grounded: %s", c.isGrounded ? "True" : "False");
            ImGui::Text("Colliding: %s", c.colliding ? "True" : "False");
        }
        ImGui::Separator();
    }

    inline void InspectorPanel::RendererComponent()
    {
        if (!m_coord.HasComponent<Renderable>(*m_selectedEntity))
            return;

        auto& r = m_coord.ModifyComponent<Renderable>(*m_selectedEntity);

        if (ImGui::CollapsingHeader("Renderer", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // Mesh Name (read-only for now)
            ImGui::Text("Mesh");
            ImGui::SameLine(100);
            ImGui::TextDisabled("%s", r.meshName.c_str());

            // Material Name (read-only for now)
            ImGui::Text("Material");
            ImGui::SameLine(100);
            ImGui::TextDisabled("%s", r.materialName.c_str());

            // isWireframe checkbox
            ImGui::Checkbox("Wireframe", &r.isWireframe);

            // wasModified checkbox
            ImGui::Checkbox("Was Modified", &r.wasModified);
        }
        ImGui::Separator();
	}

} // NightSnack

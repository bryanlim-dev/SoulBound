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
        InspectorPanel(Coordinator& coord, std::shared_ptr<Entity> selectedEntity)
            : m_coord(coord), m_selectedEntity(selectedEntity) {}

        void Draw()
        {
            ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_Once);
            ImGui::Begin("Inspector");

            if (m_selectedEntity && *m_selectedEntity != INVALID_ENTITY)
            {
                ImGui::Text("Entity: %u", *m_selectedEntity);

                if (m_coord.HasComponent<Transform>(*m_selectedEntity))
                {
                    const auto& t = m_coord.GetComponent<Transform>(*m_selectedEntity);
                    ImGui::Separator();
                    ImGui::Text("Transform");
                    ImGui::Text("  Pos: (%.2f, %.2f, %.2f)", t.pos.x, t.pos.y, t.pos.z);
                    ImGui::Text("  Rot: %.2f", t.rot);
                    ImGui::Text("  Scale: (%.2f, %.2f)", t.scale.x, t.scale.y);
                }
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
    };

} // NightSnack

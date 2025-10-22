/******************************************************************************
 * File:        OptionsPanel.h
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
    class OptionsPanel {
    public:
        OptionsPanel(Coordinator& coord) : m_coord(coord) {}

        void Draw()
        {
			// Set initial window size and style
            ImGui::SetNextWindowSize(ImVec2(320, 300), ImGuiCond_Always);

			// Options Window
            ImGui::Begin("Options");

            if (ImGui::BeginTabBar("OptionsTabs"))
            {
                // Audio Tab
                if (ImGui::BeginTabItem("Audio"))
                {
                    ImGui::Text("Volume Controls");
                    ImGui::SliderFloat("Master Volume", &m_masterVolume, 0.f, 1.0f);
                    ImGui::SliderFloat("Music Volume", &m_musicVolume, 0.f, 1.0f);
                    ImGui::SliderFloat("SFX Volume", &m_sfxVolume, 0.f, 1.0f);

					// Set volumes in AudioSystem (future implementation)

                    ImGui::EndTabItem();
                }

                // Debug Tab
                if (ImGui::BeginTabItem("Debug"))
                {
					// Performance Viewer
                    static bool togglePerfViewer = false;
                    if (ImGui::Button("View Performance Viewer"))
                    {
                        togglePerfViewer = !togglePerfViewer;
                    }

                    if (togglePerfViewer)
                    {
                        ImGui::Separator();
                        ImGui::Text("Performance Viewer (System Time Consumption)");

                        // Get Timer from ApplicationSystem
                        auto appSystem = m_coord.GetSystem<ApplicationSystem>();
                        if (appSystem)
                        {
                            appSystem->GetTimer().DisplaySystemTimers();
                        }
                        else
                        {
                            ImGui::Text("ApplicationSystem not found");
                        }
                    }
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }

            ImGui::End();
        }

    private:
        Coordinator& m_coord;
        float m_masterVolume = 0.5f;
        float m_musicVolume = 0.5f;
        float m_sfxVolume = 0.5f;
    };
}

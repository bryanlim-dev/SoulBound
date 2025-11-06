/******************************************************************************
 * File:        OptionsPanel.h
 * Project:     NightSnack
 * Author(s):	Wei Jie Lim (Primary, 100%) - Created and implemented options panel
 *
 * Description:
 * Options panel for configuring application settings. Includes audio volume controls
 * and debug options.
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

        /************************************************************************/
        /*!
        \brief
		    Constructor for OptionsPanel.

        \param[in] coord
			- Reference to Coordinator.
        */
        /************************************************************************/
        OptionsPanel(Coordinator& coord) : m_coord(coord) {}

        /************************************************************************/
        /*!
        \brief
		    Draws options panel for audio and debug settings.
        */
        /************************************************************************/
        void Draw()
        {
			// Set initial window size and style
            //ImGui::SetNextWindowSize(ImVec2(320, 300), ImGuiCond_Once);

			// Options Window
            ImGui::Begin("Options");

            if (ImGui::BeginTabBar("OptionsTabs"))
            {
                // Audio Tab
                if (ImGui::BeginTabItem("Audio"))
                {
                    ImGui::Text("Volume Controls");
                    auto audioSystem = m_coord.GetSystem<AudioSystem>();
                    if (audioSystem)
                    {
                        // Master Volume
                        float master = audioSystem->GetMasterVolume();
                        if (ImGui::SliderFloat("Master Volume", &master, 0.f, 1.0f))
                            audioSystem->SetMasterVolume(master);

                        // Music Volume
                        float music = audioSystem->GetBGMVolume();
                        if (ImGui::SliderFloat("Music Volume", &music, 0.f, 1.0f))
                            audioSystem->SetBGMVolume(music);

                        // SFX Volume
                        float sfx = audioSystem->GetSFXVolume();
                        if (ImGui::SliderFloat("SFX Volume", &sfx, 0.f, 1.0f))
                            audioSystem->SetSFXVolume(sfx);
                    }

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

					// Display performance viewer if toggled
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

		// Audio volume levels
        float m_masterVolume = 1.f;
        float m_musicVolume = 1.f;
        float m_sfxVolume = 1.f;
    };
}

/******************************************************************************
 * File:        OptionsPanel.h
 * Project:     NightSnack
 * Author(s):	Wei Jie Lim (Primary, 100%) - Created and implemented options panel
 *
 * Description:
 * Options panel for configuring application settings. Includes audio volume controls
 * and debug options.
 *
 * Copyright � 2025 DigiPen Institute of Technology Singapore.
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

                // Window settings Tab
                if (ImGui::BeginTabItem("Window"))
                {
                    auto appSystem = m_coord.GetSystem<ApplicationSystem>();
					static bool fullscreenEnabled = appSystem->m_appIsFullscreen || appSystem->m_appIsFullscreenViaButton;
                    static bool vsyncEnabled = true;
                    static bool frameCapEnabled = true;
                    static float frameCap = 60.0f;

                    if (appSystem)
                    {
						// Initialize frame cap state
                        float currentCap = appSystem->GetFrameCap();
                        frameCapEnabled = (currentCap > 0.0f);
                        if (frameCapEnabled) { frameCap = currentCap; }

                        // Initialize fullscreen state
                        fullscreenEnabled = ApplicationSystem::m_appIsFullscreen || ApplicationSystem::m_appIsFullscreenViaButton;
                        ImGui::Text("Fullscreen");
                        ImGui::SameLine(100);
                        if (ImGui::Checkbox("##FullscreenToggle", &fullscreenEnabled))
                        {
                            if (fullscreenEnabled)
                                appSystem->EnterFullscreen();
                            else
                                appSystem->ExitFullscreen();
                        }
                    }
                    ImGui::Separator();

                    // VSync toggle
                    ImGui::Text("VSync");
                    ImGui::SameLine(50);
                    if (ImGui::Checkbox("##VSync", &vsyncEnabled))
                    {
                        if (appSystem)
                        {
                            appSystem->EnableVSync(vsyncEnabled);
                        }
                    }

                    // Frame Cap toggle
                    ImGui::SameLine(100);
                    ImGui::Text("FrameCap");
                    ImGui::SameLine(170);
                    if (ImGui::Checkbox("##FrameCapEnable", &frameCapEnabled))
                    {
                        if (appSystem)
                        {
                            if (!frameCapEnabled)
                            {
                                // Disable frame cap
                                appSystem->SetFrameCap(0.0f);
                            }
                            else
                            {
                                // Enable frame cap with last value
								appSystem->SetFrameCap(frameCap);
                            }
                        }
                    }

                    // Frame Cap value input (only enabled if frame cap is on)
                    ImGui::BeginDisabled(!frameCapEnabled);
                    ImGui::Text("FrameCap Value");
                    ImGui::SameLine(100);
                    ImGui::SetNextItemWidth(100);
                    if (ImGui::InputFloat("##FrameCapValue", &frameCap, 1.0f, 10.0f, "%.1f"))
                    {
                        if (appSystem && frameCapEnabled)
                        {
                            appSystem->SetFrameCap(frameCap);
                        }
                    }
                    ImGui::EndDisabled();

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

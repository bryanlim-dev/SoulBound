/******************************************************************************
 * File:        PrefabsPanel.h
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 100%) - Drawing of prefabs tab
 *             
 *
 * Description:
 * Displays archetypes from a JSON file in the Prefabs panel.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
#pragma once

namespace NightSnack
{
    /**
     * @brief Prefabs panel for displaying and managing archetypes from JSON files
     */
    class PrefabsPanel {
    public:
        /**
        * @brief constructor
        * @param coord reference to the ECS coordinator
        * @param archetypesPath path to the archetypes JSON file
        */
        PrefabsPanel(Coordinator& coord, const std::string& archetypesPath) 
          : m_coord(coord),
            m_archetypesPath(archetypesPath)
        {
            LoadArchetypes();
        }

        /**
        * @brief draws the prefabs panel
        */
        void Draw()
        {
            if (ImGui::BeginTabItem("Prefabs"))
            {
                DrawPrefabsContent();
                ImGui::EndTabItem();
            }
        }

    private:
        Coordinator& m_coord;                              
        std::string m_archetypesPath;                           
        nlohmann::json m_archetypes;
        std::vector<std::string> m_prefabNames;
        bool m_showJsonProperties = false;
        std::string m_selectedPrefabName;

        /**
        * @brief loads archetypes from the JSON file
        */
        void LoadArchetypes()
        {
            try
            {
                std::ifstream file(m_archetypesPath);
                if (!file.is_open())
                {
                    std::cerr << "[PrefabsPanel] Failed to open archetypes file: "
                        << m_archetypesPath << std::endl;
                    return;
                }

                m_archetypes = nlohmann::json::parse(file);
                file.close();

                // extract prefab names from the JSON
                if (m_archetypes.is_object())
                {
                    for (auto it = m_archetypes.begin(); it != m_archetypes.end(); ++it)
                    {
                        m_prefabNames.push_back(it.key());
                    }
                }
            }
            catch (const std::exception& e)
            {
                std::cerr << "[PrefabsPanel] Error loading archetypes: " << e.what() << std::endl;
            }
        }

        /**
        * @brief draws the prefabs panel content
        */
        void DrawPrefabsContent()
        {
            ImGui::Text("Archetypes from: %s", m_archetypesPath.c_str());
            ImGui::Separator();

            if (m_prefabNames.empty())
            {
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f),
                    "No archetypes found or failed to load JSON file");
                return;
            }

            // display archetypes in a list
            //ImGui::Text("Available Prefabs (%zu):", m_prefabNames.size());
            ImGui::BeginChild("PrefabsList", ImVec2(0, 0), true);

            for (const auto& prefabName : m_prefabNames)
            {
                ImGui::PushID(prefabName.c_str());

                // display prefab name as selectable item
                if (ImGui::Selectable(prefabName.c_str()))
                {
                    OnPrefabSelected(prefabName);
                }

                // drag and drop source for prefabs
                // not in use for now
                //if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                //{
                //    ImGui::SetDragDropPayload("PREFAB_ARCHETYPE", prefabName.c_str(),
                //        prefabName.size() + 1);
                //    ImGui::Text("Create: %s", prefabName.c_str());
                //    ImGui::EndDragDropSource();
                //}

                //// show tooltip with prefab details on hover
                //if (ImGui::IsItemHovered())
                //{
                //    ImGui::BeginTooltip();
                //    ImGui::Text("Prefab: %s", prefabName.c_str());

                //    // show prefab components if available
                //    if (m_archetypes.contains(prefabName) &&
                //        m_archetypes[prefabName].is_object())
                //    {
                //        ImGui::Separator();
                //        ImGui::Text("Components:");
                //        for (auto it = m_archetypes[prefabName].begin();
                //            it != m_archetypes[prefabName].end(); ++it)
                //        {
                //            ImGui::BulletText("%s", it.key().c_str());
                //        }
                //    }

                //    ImGui::EndTooltip();
                //}

                ImGui::PopID();
            }

            ImGui::EndChild();

            // draw JSON properties window if a prefab is selected
            if (m_showJsonProperties && !m_selectedPrefabName.empty())
            {
                DrawJsonProperties(m_selectedPrefabName);
            }
        }

        /**
        * @brief draws the JSON properties window for a selected prefab
        * @param prefabName name of the prefab to display properties for
        */
        void DrawJsonProperties(const std::string& prefabName)
        {
            std::string windowTitle = "Properties:";
            ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiCond_FirstUseEver);
            ImGui::Begin(windowTitle.c_str(), &m_showJsonProperties);

            if (m_archetypes.contains(prefabName) && m_archetypes[prefabName].is_object())
            {
                auto& prefabData = m_archetypes[prefabName];

                // display basic prefab info
                ImGui::Text("Name: %s", prefabName.c_str());
                ImGui::Text("Components: %zu", prefabData.size());
                ImGui::Separator();

                // display each component and its properties
                ImGui::Text("Component Properties:");
                ImGui::BeginChild("ComponentProperties", ImVec2(0, 300), true);

                for (auto& component : prefabData.items())
                {
                    std::string componentName = component.key();
                    auto& componentData = component.value();

                    if (ImGui::TreeNode(componentName.c_str()))
                    {
                        DisplayComponentProperties(componentName, componentData);
                        ImGui::TreePop();
                    }
                }

                ImGui::EndChild();
                ImGui::Separator();

                // action buttons
                if (ImGui::Button("Close"))
                {
                    m_showJsonProperties = false;
                    m_selectedPrefabName = "";
                }

                ImGui::SameLine();
            }
            else
            {
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f),
                    "Invalid prefab data or prefab not found");
            }

            ImGui::End();
        }

        /**
        * @brief displays properties for a specific component
        * @param componentName name of the component
        * @param componentData JSON data for the component
        */
        void DisplayComponentProperties(const std::string& componentName, const nlohmann::json& componentData)
        {
            if (componentData.is_object())
            {
                for (auto& property : componentData.items())
                {
                    std::string propName = property.key();
                    auto& propValue = property.value();

                    ImGui::BulletText("%s: ", propName.c_str());
                    ImGui::SameLine();

                    if (propValue.is_string())
                    {
                        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "%s", propValue.get<std::string>().c_str());
                    }
                    else if (propValue.is_number_float())
                    {
                        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "%.2f", propValue.get<float>());
                    }
                    else if (propValue.is_number_integer())
                    {
                        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.4f, 1.0f), "%d", propValue.get<int>());
                    }
                    else if (propValue.is_boolean())
                    {
                        ImGui::TextColored(ImVec4(0.8f, 0.4f, 0.8f, 1.0f), "%s", propValue.get<bool>() ? "true" : "false");
                    }
                    else if (propValue.is_object())
                    {
                        // handle nested objects (position, scale etc)
                        if (ImGui::TreeNode("Properties"))
                        {
                            for (auto& nestedProp : propValue.items())
                            {
                                ImGui::BulletText("%s: ", nestedProp.key().c_str());
                                ImGui::SameLine();

                                if (nestedProp.value().is_number_float())
                                {
                                    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "%.2f", nestedProp.value().get<float>());
                                }
                                else if (nestedProp.value().is_number_integer())
                                {
                                    ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.4f, 1.0f), "%d", nestedProp.value().get<int>());
                                }
                            }
                            ImGui::TreePop();
                        }
                    }
                    else if (propValue.is_array())
                    {
                        // handle arrays
                        ImGui::TextColored(ImVec4(0.8f, 0.6f, 0.2f, 1.0f), "[Array - %zu elements]", propValue.size());
                    }
                    else
                    {
                        ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "Unknown type");
                    }
                }
            }
            else
            {
                ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "Invalid component data structure");
            }
        }

        /**
         * @brief handles prefab selection and shows properties window
         * @param prefabName The name of the selected prefab
         */
        void OnPrefabSelected(const std::string& prefabName)
        {
            m_selectedPrefabName = prefabName;
            m_showJsonProperties = true;
            std::cout << "[PrefabsPanel] Selected prefab: " << prefabName << std::endl;
        }
    };
}

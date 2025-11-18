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
        PrefabsPanel(Coordinator& coord, const std::string& archetypesRoot)
            : m_coord(coord),
            m_archetypesRoot(archetypesRoot)
        {
            LoadArchetypes();
        }

        /**
        * @brief selects a prefab for preview (called from main assets panel)
        * @param prefabName name of the prefab to select
        */
        void SelectPrefab(const std::string& prefabName)
        {
            m_selectedPrefabName = prefabName;
        }

        /**
        * @brief draws prefab properties in the asset inspector
        * @param prefabName name of the prefab
        */
        void DrawPrefabProperties(const std::string& prefabName)
        {
            if (m_archetypes.contains(prefabName) && m_archetypes[prefabName].is_object())
            {
                auto& prefabData = m_archetypes[prefabName];

                ImGui::Text("Prefab: %s", prefabName.c_str());
                ImGui::Text("Components: %zu", prefabData.size());
                ImGui::Separator();

                ImGui::Text("Component Properties:");
                ImGui::BeginChild("PrefabComponentProperties", ImVec2(0, 300), true);

                for (auto& component : prefabData.items())
                {
                    std::string componentName = component.key();
                    auto& componentData = component.value();

                    if (ImGui::TreeNode(componentName.c_str()))
                    {
                        DisplayComponentProperties(componentData);
                        ImGui::TreePop();
                    }
                }

                ImGui::EndChild();
            }
            else
            {
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f),
                    "Invalid prefab data or prefab not found");
            }
        }

    private:
        Coordinator& m_coord;
        std::string m_archetypesRoot;
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
                std::ifstream file(m_archetypesRoot);
                if (!file.is_open())
                {
                    std::cerr << "Failed to open archetypes file: "
                        << m_archetypesRoot << std::endl;
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
                std::cerr << "Error loading archetypes: " << e.what() << std::endl;
            }
        }

        /**
        * @brief displays properties for a specific component
        * @param componentName name of the component
        * @param componentData JSON data for the component
        */
        void DisplayComponentProperties(const nlohmann::json& componentData)
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
    };
}
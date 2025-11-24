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
        PrefabsPanel(Coordinator& coord, const std::string& prefabsFolder)
            : m_coord(coord),
            m_prefabsFolder(prefabsFolder)
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
            if (!m_archetypes.contains(prefabName) ||
                !m_archetypes[prefabName].is_object())
            {
                ImGui::TextColored(ImVec4(1, 0.4f, 0.4f, 1),
                    "Invalid prefab or data not found.");
                return;
            }

            auto& prefabData = m_archetypes[prefabName];

            ImGui::Text("Prefab: %s", prefabName.c_str());

            ImGui::Separator();

            // json UI
            for (auto& [componentName, componentData] : prefabData.items())
            {
                if (ImGui::CollapsingHeader(componentName.c_str(),
                    ImGuiTreeNodeFlags_DefaultOpen))
                {
                    if (componentName == "Transform")               DrawTransformEditor(componentData);
                    else if (componentName == "Color")              DrawColorEditor(componentData);
                    else if (componentName == "RigidBody")          DrawRigidBodyEditor(componentData);
                    else if (componentName == "Collider")           DrawColliderEditor(componentData);
                    else if (componentName == "Renderable")         DrawRenderableEditor(componentData);
                    //else if (componentName == "Camera")             DrawCameraEditor(componentData);
                    else if (componentName == "Animator")           DrawAnimatorEditor(componentData);
                    else if (componentName == "PlayerController")   DrawPlayerControllerEditor(componentData);
                    else if (componentName == "Font")               DrawFontEditor(componentData);
                    else if (componentName == "ButtonInfo")         DrawButtonInfoEditor(componentData);
                    else if (componentName == "Script")             DrawScriptEditor(componentData);

                    else {
                        // Fallback generic editor
                        DrawJsonFields(componentData);
                    }
                }
            }

            ImGui::Separator();


            // raw json preview
            if (ImGui::CollapsingHeader("JSON Preview", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::BeginChild("PrefabJsonPreview", ImVec2(0, 300), true);

                for (auto& [componentName, componentJson] : prefabData.items())
                {
                    if (ImGui::TreeNode(componentName.c_str()))
                    {
                        DisplayComponentProperties(componentJson);
                        ImGui::TreePop();
                    }
                }

                ImGui::EndChild();
            }

            // save prefab button if changes are made
            if (!m_prefabModified)
            {
                ImGui::BeginDisabled();
            }

            if (ImGui::Button("Save Prefab"))
            {
                EditPrefab(prefabName);
                m_prefabModified = false;
            }

            if (!m_prefabModified)
            {
                ImGui::EndDisabled();
            }
        }

    private:
        Coordinator& m_coord;
        std::string m_prefabsFolder;
        std::unordered_map<std::string, nlohmann::json> m_archetypes;
        std::vector<std::string> m_prefabNames;
        bool m_showJsonProperties = false;
        std::string m_selectedPrefabName;

        bool m_prefabModified = false;

        /**
        * @brief loads archetypes from the JSON file
        */
        void LoadArchetypes()
        {
            try
            {
                m_archetypes.clear();
                m_prefabNames.clear();

                // iterate through all .json files in the Prefabs folder
                for (const auto& entry : std::filesystem::directory_iterator(m_prefabsFolder))
                {
                    if (entry.path().extension() == ".json")
                    {
                        std::ifstream file(entry.path());
                        if (!file.is_open())
                        {
                            std::cerr << "Failed to open prefab file: " << entry.path() << std::endl;
                            continue;
                        }

                        // get prefab name from filename
                        std::string prefabName = entry.path().stem().string();

                        // parse and store the JSON data
                        nlohmann::json prefabData = nlohmann::json::parse(file);
                        m_archetypes[prefabName] = prefabData;
                        m_prefabNames.push_back(prefabName);

                        file.close();
                    }
                }
            }
            catch (const std::exception& e)
            {
                std::cerr << "Error loading prefabs: " << e.what() << std::endl;
            }
        }

        /*void EditPrefab(std::string const& prefabName) {
             access json with prefabname
             m_archetypes[prefabName];

             edit the selected json


             save the json to file; the actual json file

             reload all entities with this prefab
            m_coord.GetSystem<EntityFactory>()->UpdatePrefab(prefabName);

        }*/

        void EditPrefab(const std::string& prefabName) 
        {
            if (!m_archetypes.contains(prefabName))
            {
                std::cerr << "Prefab not found: " << prefabName << std::endl;
                return;
            }

            try
            {
                // construct the file path
                std::filesystem::path filePath = std::filesystem::path(m_prefabsFolder) / (prefabName + ".json");

                // open file for writing
                std::ofstream file(filePath);
                if (!file.is_open())
                {
                    std::cerr << "Failed to open prefab file for writing: " << filePath << std::endl;
                    return;
                }

                // write the JSON with formatting (4 space indent for easier reading of the json file)
                file << m_archetypes[prefabName].dump(4);
                file.close();

                std::cout << "Prefab saved: " << prefabName << std::endl;

                // reload all entities using this prefab
                m_coord.GetSystem<EntityFactory>()->UpdatePrefab(prefabName);
            }\
            catch (const std::exception& e)
            {
                std::cerr << "Error saving prefab " << prefabName << ": " << e.what() << std::endl;
            }
        }

        /**
        * @brief displays properties for a specific component
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

                    if (propValue.is_null())
                    {
                        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "null");
                    }
                    else if (propValue.is_string())
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

        void DrawTransformEditor(nlohmann::json& j)
        {
            auto& pos = j["Position"];
            auto& scale = j["Scale"];

            float p[3] = { pos["x"], pos["y"], pos["z"] };
            if (ImGui::DragFloat3("Position", p, 0.1f))
            {
                pos["x"] = p[0]; pos["y"] = p[1]; pos["z"] = p[2];
                m_prefabModified = true;
            }

            float rot = j["Rotation"];
            if (ImGui::DragFloat("Rotation", &rot, 0.1f))
            {
                j["Rotation"] = rot;
                m_prefabModified = true;
            }

            float s[2] = { scale["x"], scale["y"] };
            if (ImGui::DragFloat2("Scale", s, 0.1f))
            {
                scale["x"] = s[0]; scale["y"] = s[1];
                m_prefabModified = true;
            }
        }

        void DrawColorEditor(nlohmann::json& j)
        {
            float color[4] = { j["r"], j["g"], j["b"], j["a"] };

            if (ImGui::ColorEdit4("Color", color))
            {
                j["r"] = color[0];
                j["g"] = color[1];
                j["b"] = color[2];
                j["a"] = color[3];
                m_prefabModified = true;
            }
        }

        void DrawRigidBodyEditor(nlohmann::json& j)
        {
            float mass = j["mass"];
            bool useGravity = j["useGravity"];

            if (ImGui::DragFloat("Mass", &mass, 0.1f))
            {
                j["mass"] = mass;
                m_prefabModified = true;
            }

            if (ImGui::Checkbox("Use Gravity", &useGravity))
            {
                j["useGravity"] = useGravity;
                m_prefabModified = true;
            }
        }

        void DrawColliderEditor(nlohmann::json& j)
        {
            std::string shape = j["shape"];
            std::string type = j["type"];
            bool isStatic = j["isStatic"];

            char shapeBuf[64];
            strcpy_s(shapeBuf, shape.c_str());
            if (ImGui::InputText("Shape", shapeBuf, sizeof(shapeBuf)))
            {
                shape = shapeBuf;
                m_prefabModified = true;
            }
            j["shape"] = shape;

            char typeBuf[64];
            strcpy_s(typeBuf, type.c_str());
            if (ImGui::InputText("Type", typeBuf, sizeof(typeBuf)))
            {
                type = typeBuf;
                m_prefabModified = true;
            }
            j["type"] = type;

            if (ImGui::Checkbox("Static", &isStatic))
            {
                m_prefabModified = true;
            }
            j["isStatic"] = isStatic;

            if (shape == "Circle")
            {
                float radius = j["radius"];
                if (ImGui::DragFloat("Radius", &radius))
                {
                    j["radius"] = radius;
                    m_prefabModified = true;
                }
            }
            else
            {
                float w = j["width"];
                float h = j["height"];

                if (ImGui::DragFloat("Width", &w))
                {
                    j["width"] = w;
                    m_prefabModified = true;
                }

                if (ImGui::DragFloat("Height", &h))
                {
                    j["height"] = h;
                    m_prefabModified = true;
                }
            }
        }

        void DrawRenderableEditor(nlohmann::json& j)
        {
            char mesh[128]; strcpy_s(mesh, j["meshName"].get<std::string>().c_str());
            if (ImGui::InputText("Mesh", mesh, sizeof(mesh)))
            {
                j["meshName"] = mesh;
                m_prefabModified = true;
            }

            char mat[128]; strcpy_s(mat, j["materialName"].get<std::string>().c_str());
            if (ImGui::InputText("Material", mat, sizeof(mat)))
            {
                j["materialName"] = mat;
                m_prefabModified = true;
            }

            bool wf = j.contains("isWireframe") ? j["isWireframe"].get<bool>() : false;
            if (ImGui::Checkbox("Wireframe", &wf))
            {
                j["isWireframe"] = wf;
                m_prefabModified = true;
            }
        }

        void DrawPlayerControllerEditor(nlohmann::json& j)
        {
            float move = j["moveSpeed"];
            float jump = j["jumpSpeed"];

            if (ImGui::DragFloat("Move Speed", &move, 1.0f))
            {
                m_prefabModified = true;
            }

            if (ImGui::DragFloat("Jump Speed", &jump, 1.0f))
            {
                m_prefabModified = true;
            }

            j["moveSpeed"] = move;
            j["jumpSpeed"] = jump;
        }

        void DrawAnimatorEditor(nlohmann::json& j)
        {
            char buf[128]; strcpy_s(buf, j["spriteSheetName"].get<std::string>().c_str());
            if (ImGui::InputText("Sprite Sheet", buf, sizeof(buf)))
            {
                j["spriteSheetName"] = buf;
                m_prefabModified = true;
            }

            float speed = j["playbackSpeed"];
            bool playing = j["isPlaying"];
            bool looping = j["isLooping"];

            if (ImGui::DragFloat("Playback Speed", &speed, 0.05f))
            {
                m_prefabModified = true;
            }

            if (ImGui::Checkbox("Playing", &playing))
            {
                m_prefabModified = true;
            }

            if (ImGui::Checkbox("Looping", &looping))
            {
                m_prefabModified = true;
            }

            j["playbackSpeed"] = speed;
            j["isPlaying"] = playing;
            j["isLooping"] = looping;
        }

        void DrawFontEditor(nlohmann::json& j)
        {
            char text[128]; strcpy_s(text, j["text"].get<std::string>().c_str());
            if (ImGui::InputText("Text", text, sizeof(text)))
            {
                j["text"] = text;
                m_prefabModified = true;
            }

            float size = j["fontSize"];
            if (ImGui::DragFloat("Size", &size, 0.05f))
            {
                m_prefabModified = true;
            }
            j["fontSize"] = size;

            auto& c = j["textColor"];
            float color[3] = { c["r"], c["g"], c["b"] };

            if (ImGui::ColorEdit3("Text Color", color))
            {
                c["r"] = color[0];
                c["g"] = color[1];
                c["b"] = color[2];
                m_prefabModified = true;
            }
        }

        void DrawButtonInfoEditor(nlohmann::json& j)
        {
            char action[64]; strcpy_s(action, j["action"].get<std::string>().c_str());
            if (ImGui::InputText("Action", action, sizeof(action)))
            {
                j["action"] = action;
                m_prefabModified = true;
            }

            char type[64]; strcpy_s(type, j["type"].get<std::string>().c_str());
            if (ImGui::InputText("Type", type, sizeof(type)))
            {
                j["type"] = type;
                m_prefabModified = true;
            }
        }

        void DrawScriptEditor(nlohmann::json& j)
        {
            char name[128];
            strcpy_s(name, j["name"].get<std::string>().c_str());

            if (ImGui::InputText("Script Name", name, sizeof(name)))
            {
                j["name"] = name;
                m_prefabModified = true;
            }
        }

        void DrawJsonFields(nlohmann::json& j)
        {
            for (auto& [key, val] : j.items())
            {
                if (val.is_number_float())
                {
                    float f = val;
                    if (ImGui::DragFloat(key.c_str(), &f))
                    {
                        val = f;
                        m_prefabModified = true;
                    }
                }
                else if (val.is_number_integer())
                {
                    int i = val;
                    if (ImGui::DragInt(key.c_str(), &i))
                    {
                        val = i;
                        m_prefabModified = true;
                    }
                }
                else if (val.is_boolean())
                {
                    bool b = val;
                    if (ImGui::Checkbox(key.c_str(), &b))
                    {
                        val = b;
                        m_prefabModified = true;
                    }
                }
                else if (val.is_string())
                {
                    char buf[128];
                    strcpy_s(buf, val.get<std::string>().c_str());
                    if (ImGui::InputText(key.c_str(), buf, sizeof(buf)))
                    {
                        val = buf;
                        m_prefabModified = true;
                    }
                }
                else if (val.is_object())
                {
                    if (ImGui::TreeNode(key.c_str()))
                    {
                        DrawJsonFields(val);
                        ImGui::TreePop();
                    }
                }
            }
        }
    };
}
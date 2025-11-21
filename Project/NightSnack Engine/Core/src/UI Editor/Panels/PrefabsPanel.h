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
            }

            float rot = j["Rotation"];
            if (ImGui::DragFloat("Rotation", &rot, 0.1f))
                j["Rotation"] = rot;

            float s[2] = { scale["x"], scale["y"] };
            if (ImGui::DragFloat2("Scale", s, 0.1f))
            {
                scale["x"] = s[0]; scale["y"] = s[1];
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
            }
        }

        void DrawRigidBodyEditor(nlohmann::json& j)
        {
            float mass = j["mass"];
            bool useGravity = j["useGravity"];

            if (ImGui::DragFloat("Mass", &mass, 0.1f))
                j["mass"] = mass;

            if (ImGui::Checkbox("Use Gravity", &useGravity))
                j["useGravity"] = useGravity;
        }

        void DrawColliderEditor(nlohmann::json& j)
        {
            std::string shape = j["shape"];
            std::string type = j["type"];
            bool isStatic = j["isStatic"];

            char shapeBuf[64];
            strcpy(shapeBuf, shape.c_str());
            if (ImGui::InputText("Shape", shapeBuf, sizeof(shapeBuf)))
            {
                shape = shapeBuf;
            }
            j["shape"] = shape;

            char typeBuf[64];
            strcpy(typeBuf, type.c_str());
            if (ImGui::InputText("Type", typeBuf, sizeof(typeBuf)))
            {
                type = typeBuf;
            }
            j["type"] = type;

            ImGui::Checkbox("Static", &isStatic);
            j["isStatic"] = isStatic;

            if (shape == "Circle")
            {
                float radius = j["radius"];
                if (ImGui::DragFloat("Radius", &radius))
                    j["radius"] = radius;
            }
            else
            {
                float w = j["width"];
                float h = j["height"];

                if (ImGui::DragFloat("Width", &w))
                    j["width"] = w;

                if (ImGui::DragFloat("Height", &h))
                    j["height"] = h;
            }
        }

        void DrawRenderableEditor(nlohmann::json& j)
        {
            char mesh[128]; strcpy(mesh, j["meshName"].get<std::string>().c_str());
            if (ImGui::InputText("Mesh", mesh, sizeof(mesh)))
                j["meshName"] = mesh;

            char mat[128]; strcpy(mat, j["materialName"].get<std::string>().c_str());
            if (ImGui::InputText("Material", mat, sizeof(mat)))
                j["materialName"] = mat;

            bool wf = j.contains("isWireframe") ? j["isWireframe"].get<bool>() : false;
            if (ImGui::Checkbox("Wireframe", &wf))
                j["isWireframe"] = wf;
        }

        void DrawPlayerControllerEditor(nlohmann::json& j)
        {
            float move = j["moveSpeed"];
            float jump = j["jumpSpeed"];

            ImGui::DragFloat("Move Speed", &move, 1.0f);
            ImGui::DragFloat("Jump Speed", &jump, 1.0f);

            j["moveSpeed"] = move;
            j["jumpSpeed"] = jump;
        }

        /*void DrawCameraEditor(nlohmann::json& j)
        {
            float zoom = j["zoom"];
            if (ImGui::DragFloat("Zoom", &zoom, 0.1f))
                j["zoom"] = zoom;

            bool main = j["isMainCamera"];
            if (ImGui::Checkbox("Main Camera", &main))
                j["isMainCamera"] = main;
        }*/

        void DrawAnimatorEditor(nlohmann::json& j)
        {
            char buf[128]; strcpy(buf, j["spriteSheetName"].get<std::string>().c_str());
            if (ImGui::InputText("Sprite Sheet", buf, sizeof(buf)))
                j["spriteSheetName"] = buf;

            float speed = j["playbackSpeed"];
            bool playing = j["isPlaying"];
            bool looping = j["isLooping"];

            ImGui::DragFloat("Playback Speed", &speed, 0.05f);
            ImGui::Checkbox("Playing", &playing);
            ImGui::Checkbox("Looping", &looping);

            j["playbackSpeed"] = speed;
            j["isPlaying"] = playing;
            j["isLooping"] = looping;
        }


        void DrawFontEditor(nlohmann::json& j)
        {
            char text[128]; strcpy(text, j["text"].get<std::string>().c_str());
            if (ImGui::InputText("Text", text, sizeof(text)))
                j["text"] = text;

            float size = j["fontSize"];
            ImGui::DragFloat("Size", &size, 0.05f);
            j["fontSize"] = size;

            auto& c = j["textColor"];
            float color[3] = { c["r"], c["g"], c["b"] };

            if (ImGui::ColorEdit3("Text Color", color))
            {
                c["r"] = color[0];
                c["g"] = color[1];
                c["b"] = color[2];
            }
        }

        void DrawButtonInfoEditor(nlohmann::json& j)
        {
            char action[64]; strcpy(action, j["action"].get<std::string>().c_str());
            if (ImGui::InputText("Action", action, sizeof(action)))
                j["action"] = action;

            char type[64]; strcpy(type, j["type"].get<std::string>().c_str());
            if (ImGui::InputText("Type", type, sizeof(type)))
                j["type"] = type;
        }

        void DrawScriptEditor(nlohmann::json& j)
        {
            char name[128];
            strcpy(name, j["name"].get<std::string>().c_str());

            if (ImGui::InputText("Script Name", name, sizeof(name)))
                j["name"] = name;
        }

        void DrawJsonFields(nlohmann::json& j)
        {
            for (auto& [key, val] : j.items())
            {
                if (val.is_number_float())
                {
                    float f = val;
                    if (ImGui::DragFloat(key.c_str(), &f))
                        val = f;
                }
                else if (val.is_number_integer())
                {
                    int i = val;
                    if (ImGui::DragInt(key.c_str(), &i))
                        val = i;
                }
                else if (val.is_boolean())
                {
                    bool b = val;
                    if (ImGui::Checkbox(key.c_str(), &b))
                        val = b;
                }
                else if (val.is_string())
                {
                    char buf[128];
                    strcpy(buf, val.get<std::string>().c_str());
                    if (ImGui::InputText(key.c_str(), buf, sizeof(buf)))
                        val = buf;
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
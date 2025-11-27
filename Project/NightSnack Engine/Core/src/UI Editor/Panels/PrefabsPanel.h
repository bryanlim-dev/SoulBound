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
                    else if (componentName == "Audio")              DrawAudioEditor(componentData);

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
            ImGui::BeginDisabled(!m_prefabModified);


            if (ImGui::Button("Save Prefab"))
            {
                EditPrefab(prefabName);
                m_prefabModified = false;
            }

            ImGui::EndDisabled();
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

                        //if (j["name"].is_array()) {
                        //    // Handle as array
                        //    for (const auto& name : j["name"]) {
                        //        s.names.push_back(name.get<std::string>());
                        //    }
                        //}
                        //else {
                        //    m_archetypes[prefabName] = prefabData;
                        //}

						//from_json(prefabData, m_archetypes[prefabName]);
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

        /**
        * @brief saves modified prefab data back to JSON file and updates entities
        * @param prefabName name of the prefab to save
        */
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
            }
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

        /**
        * @brief draws editor UI for Transform component properties
        * @param j JSON data for the Transform component
        */
        void DrawTransformEditor(nlohmann::json& j)
        {
            auto& pos = j["Position"];
            auto& scale = j["Scale"];

            float p[3] = { pos["x"], pos["y"], pos["z"] };
            if (ImGui::DragFloat3("Position", p, 0.1f))
            {
                pos["x"] = p[0];
                pos["y"] = p[1];
                pos["z"] = p[2];
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
                scale["x"] = s[0];
                scale["y"] = s[1];
                m_prefabModified = true;
            }
        }

        /**
        * @brief draws editor UI for Color component properties
        * @param j JSON data for the Color component
        */
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

        /**
        * @brief draws editor UI for RigidBody component properties
        * @param j JSON data for the RigidBody component
        */
        void DrawRigidBodyEditor(nlohmann::json& j)
        {
            float mass = j["mass"];
            bool useGravity = j["useGravity"];

            if (ImGui::DragFloat("Mass", &mass, 0.1f, 0.0f, 1000.0f))
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

        /**
        * @brief draws editor UI for Collider component properties
        * @param j JSON data for the Collider component
        */
        void DrawColliderEditor(nlohmann::json& j)
        {
            // Shape dropdown
            std::string shape = j["shape"];
            const char* shapes[] = { "Circle", "AABB" };
            int currentShapeIdx = 0;

            for (int i = 0; i < IM_ARRAYSIZE(shapes); i++)
            {
                if (shape == shapes[i])
                {
                    currentShapeIdx = i;
                    break;
                }
            }

            if (ImGui::Combo("Shape", &currentShapeIdx, shapes, IM_ARRAYSIZE(shapes)))
            {
                j["shape"] = shapes[currentShapeIdx];
                shape = shapes[currentShapeIdx];
                m_prefabModified = true;
            }

            // Type dropdown
            std::string type = j["type"];
            const char* types[] = { "Solid", "Trigger" };
            int currentTypeIdx = 0;

            for (int i = 0; i < IM_ARRAYSIZE(types); i++)
            {
                if (type == types[i])
                {
                    currentTypeIdx = i;
                    break;
                }
            }

            if (ImGui::Combo("Type", &currentTypeIdx, types, IM_ARRAYSIZE(types)))
            {
                j["type"] = types[currentTypeIdx];
                type = types[currentTypeIdx];
                m_prefabModified = true;
            }

            // Static checkbox
            bool isStatic = j["isStatic"];
            if (ImGui::Checkbox("Static", &isStatic))
            {
                j["isStatic"] = isStatic;
                m_prefabModified = true;
            }

            // Shape-specific properties
            if (shape == "Circle")
            {
                float radius = j["radius"];
                if (ImGui::DragFloat("Radius", &radius, 0.1f, 0.1f, 100.0f))
                {
                    j["radius"] = radius;
                    m_prefabModified = true;
                }
            }
            else if (shape == "AABB")
            {
                float w = j["width"];
                float h = j["height"];

                if (ImGui::DragFloat("Width", &w, 0.1f, 0.1f, 100.0f))
                {
                    j["width"] = w;
                    m_prefabModified = true;
                }

                if (ImGui::DragFloat("Height", &h, 0.1f, 0.1f, 100.0f))
                {
                    j["height"] = h;
                    m_prefabModified = true;
                }
            }
        }

        /**
        * @brief draws editor UI for Renderable component properties
        * @param j JSON data for the Renderable component
        */
        void DrawRenderableEditor(nlohmann::json& j)
        {
            // Mesh dropdown
            std::string meshName = j["meshName"];
            const char* meshes[] = { "square", "circle" };
            int currentMeshIdx = 0;

            for (int i = 0; i < IM_ARRAYSIZE(meshes); i++)
            {
                if (meshName == meshes[i])
                {
                    currentMeshIdx = i;
                    break;
                }
            }

            if (ImGui::Combo("Mesh", &currentMeshIdx, meshes, IM_ARRAYSIZE(meshes)))
            {
                j["meshName"] = meshes[currentMeshIdx];
                m_prefabModified = true;
            }

            // material dropdown
            std::string matName = j["materialName"];

            auto graphicsSystem = m_coord.GetSystem<GraphicsSystem>();
            std::vector<std::string> availableMaterials;

            if (graphicsSystem) {
                availableMaterials = graphicsSystem->GetMaterialMapStrings();
                //std::cout << "Loaded " << availableMaterials.size() << " materials from GraphicsSystem.\n";
            }
            else {
                // Fallback if graphics system is not available
                availableMaterials = { "ColorMat" };
            }

            // find current material index
            int currentMatIdx = 0;
            bool foundMaterial = false;
            for (int i = 0; i < availableMaterials.size(); i++)
            {
                if (matName == availableMaterials[i])
                {
                    currentMatIdx = i;
                    foundMaterial = true;
                    break;
                }
            }

            // if current material not found, default to first one
            if (!foundMaterial && !availableMaterials.empty()) {
                currentMatIdx = 0;
                j["materialName"] = availableMaterials[0];
                m_prefabModified = true;
            }

            // create C-style array for ImGui
            std::vector<const char*> materialNames;
            for (const auto& mat : availableMaterials)
            {
                materialNames.push_back(mat.c_str());
            }

            if (ImGui::Combo("Material", &currentMatIdx, materialNames.data(), static_cast<int>(materialNames.size())))
            {
                j["materialName"] = availableMaterials[currentMatIdx];
                m_prefabModified = true;
            }

            // wireframe checkbox
            bool wireframe = j.contains("isWireframe") ? j["isWireframe"].get<bool>() : false;
            if (ImGui::Checkbox("Wireframe", &wireframe))
            {
                j["isWireframe"] = wireframe;
                m_prefabModified = true;
            }
        }

        /**
        * @brief draws editor UI for PlayerController component properties
        * @param j JSON data for the PlayerController component
        */
        void DrawPlayerControllerEditor(nlohmann::json& j)
        {
            float moveSpeed = j["moveSpeed"];
            float jumpSpeed = j["jumpSpeed"];

            if (ImGui::DragFloat("Move Speed", &moveSpeed, 1.0f, 0.0f, 1000.0f))
            {
                j["moveSpeed"] = moveSpeed;
                m_prefabModified = true;
            }

            if (ImGui::DragFloat("Jump Speed", &jumpSpeed, 1.0f, 0.0f, 1000.0f))
            {
                j["jumpSpeed"] = jumpSpeed;
                m_prefabModified = true;
            }
        }

        /**
        * @brief draws editor UI for Animator component properties
        * @param j JSON data for the Animator component
        */
        void DrawAnimatorEditor(nlohmann::json& j)
        {
            // Sprite sheet input
            std::string spriteSheet = j["spriteSheetName"];
            char buf[128];
            strcpy_s(buf, spriteSheet.c_str());

            if (ImGui::InputText("Sprite Sheet", buf, sizeof(buf)))
            {
                j["spriteSheetName"] = std::string(buf);
                m_prefabModified = true;
            }

            // Playback speed slider
            float speed = j["playbackSpeed"];
            if (ImGui::SliderFloat("Playback Speed", &speed, 0.0f, 5.0f))
            {
                j["playbackSpeed"] = speed;
                m_prefabModified = true;
            }

            // Playing checkbox
            bool playing = j["isPlaying"];
            if (ImGui::Checkbox("Playing", &playing))
            {
                j["isPlaying"] = playing;
                m_prefabModified = true;
            }

            // Looping checkbox
            bool looping = j["isLooping"];
            if (ImGui::Checkbox("Looping", &looping))
            {
                j["isLooping"] = looping;
                m_prefabModified = true;
            }
        }

        /**
        * @brief draws editor UI for Font component properties
        * @param j JSON data for the Font component
        */
        void DrawFontEditor(nlohmann::json& j)
        {
            // Text input
            std::string text = j["text"];
            char textBuf[256];
            strcpy_s(textBuf, text.c_str());

            if (ImGui::InputTextMultiline("Text", textBuf, sizeof(textBuf), ImVec2(-1, 60)))
            {
                j["text"] = std::string(textBuf);
                m_prefabModified = true;
            }

            // Font size input
            float fontSize = j["fontSize"];
            if (ImGui::DragFloat("Font Size", &fontSize, 0.5f, 0.2f, 10.0f))
            {
                j["fontSize"] = fontSize;
                m_prefabModified = true;
            }

            // Text color picker
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

        /**
        * @brief draws editor UI for ButtonInfo component properties
        * @param j JSON data for the ButtonInfo component
        */
        void DrawButtonInfoEditor(nlohmann::json& j)
        {
            // Action dropdown (common button actions)
            std::string action = j["action"];
            const char* actions[] = {
                "StartGame", "LoadLevel", "Quit", "Pause",
                "Resume", "Settings", "MainMenu", "Custom"
            };
            int currentActionIdx = 0;
            bool isCustomAction = true;

            for (int i = 0; i < IM_ARRAYSIZE(actions); i++)
            {
                if (action == actions[i])
                {
                    currentActionIdx = i;
                    isCustomAction = false;
                    break;
                }
            }

            if (ImGui::Combo("Action", &currentActionIdx, actions, IM_ARRAYSIZE(actions)))
            {
                j["action"] = actions[currentActionIdx];
                action = actions[currentActionIdx];
                m_prefabModified = true;
            }

            // Custom action input if "Custom" is selected
            if (currentActionIdx == IM_ARRAYSIZE(actions) - 1)
            {
                char customAction[128];
                strcpy_s(customAction, action.c_str());

                if (ImGui::InputText("Custom Action", customAction, sizeof(customAction)))
                {
                    j["action"] = std::string(customAction);
                    m_prefabModified = true;
                }
            }

            // Type dropdown
            std::string type = j["type"];
            const char* types[] = { "Normal", "Toggle", "Radio" };
            int currentTypeIdx = 0;

            for (int i = 0; i < IM_ARRAYSIZE(types); i++)
            {
                if (type == types[i])
                {
                    currentTypeIdx = i;
                    break;
                }
            }

            if (ImGui::Combo("Button Type", &currentTypeIdx, types, IM_ARRAYSIZE(types)))
            {
                j["type"] = types[currentTypeIdx];
                m_prefabModified = true;
            }
        }

        /**
        * @brief draws editor UI for Script component properties
        * @param j JSON data for the Script component
        */
        void DrawScriptEditor(nlohmann::json& j)
        {
            // Script name input
            std::vector<std::string> scriptNames;
            for (std::string const& s : j["name"]) {
				scriptNames.push_back(s);
            }
            std::string scriptDisplay = scriptNames[0];
            for (int i{ 1 }; i < scriptNames.size(); ++i) {
				scriptDisplay += ", " + scriptNames[i];
            }

            char nameBuf[128];
            strcpy_s(nameBuf, scriptDisplay.c_str());

            if (ImGui::InputText("Scripts ", nameBuf, sizeof(nameBuf)))
            {
                j["name"].clear();
                for (auto& s : SplitScripts(nameBuf)) {
                    j["name"].push_back(s);
                }
                m_prefabModified = true;
            }
        }

        /**
        * @brief splits comma-separated script names into individual script names
        * @param s comma-separated string of script names
        * @return vector of individual script names
        */
        std::vector<std::string> SplitScripts(std::string const& s) {
            std::stringstream iss{ s };
            std::string script;

			std::vector<std::string> scripts;

			while (std::getline(iss, script, ',')) {
				// trim whitespace
				script.erase(0, script.find_first_not_of(" \t\n\r\f\v"));
				script.erase(script.find_last_not_of(" \t\n\r\f\v") + 1);
				if (!script.empty()) {
					scripts.push_back(script);
				}
			}
            return scripts;
        }

        /**
        * @brief generic JSON field editor for unknown component types
        * @param j JSON data to edit
        */
        void DrawJsonFields(nlohmann::json& j)
        {
            for (auto& [key, val] : j.items())
            {
                if (val.is_number_float())
                {
                    float f = val;
                    if (ImGui::DragFloat(key.c_str(), &f, 0.1f))
                    {
                        j[key] = f;
                        m_prefabModified = true;
                    }
                }
                else if (val.is_number_integer())
                {
                    int i = val;
                    if (ImGui::DragInt(key.c_str(), &i))
                    {
                        j[key] = i;
                        m_prefabModified = true;
                    }
                }
                else if (val.is_boolean())
                {
                    bool b = val;
                    if (ImGui::Checkbox(key.c_str(), &b))
                    {
                        j[key] = b;
                        m_prefabModified = true;
                    }
                }
                else if (val.is_string())
                {
                    std::string str = val;
                    char buf[256];
                    strcpy_s(buf, str.c_str());

                    if (ImGui::InputText(key.c_str(), buf, sizeof(buf)))
                    {
                        j[key] = std::string(buf);
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
                else if (val.is_array())
                {
                    ImGui::Text("%s: [Array - %zu elements]", key.c_str(), val.size());
                    
                }
            }
        }

        void DrawAudioEditor(nlohmann::json& j)
        {
            // Get available audio keys for dropdown
            auto audioSystem = m_coord.GetSystem<AudioSystem>();
            std::vector<std::string> availableAudio;
            if (audioSystem) {
                availableAudio = audioSystem->GetAvailableAudioKeys();
            }
            else {
                availableAudio = { "No audio system available" };
            }

            // audio key dropdown
            std::string audioKey = j["audioKey"];
            int currentAudioIdx = 0;
            bool foundAudio = false;

            for (int i = 0; i < availableAudio.size(); i++) 
            {
                if (audioKey == availableAudio[i]) {
                    currentAudioIdx = i;
                    foundAudio = true;
                    break;
                }
            }

            // if current audio not found, default to first one
            if (!foundAudio && !availableAudio.empty() && availableAudio[0] != "No audio system available") 
            {
                currentAudioIdx = 0;
                j["audioKey"] = availableAudio[0];
                m_prefabModified = true;
            }

            std::vector<const char*> audioNames;
            for (const auto& audioName : availableAudio) 
            {
                audioNames.push_back(audioName.c_str());
            }

            if (ImGui::Combo("Audio Key", &currentAudioIdx, audioNames.data(), static_cast<int>(audioNames.size()))) 
            {
                j["audioKey"] = availableAudio[currentAudioIdx];
                m_prefabModified = true;
            }

            // volume slider
            float volume = j["volume"];
            if (ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f, "%.2f")) 
            {
                j["volume"] = volume;
                m_prefabModified = true;
            }

            //lLoop checkbox
            bool loop = j["loop"];
            if (ImGui::Checkbox("Loop", &loop)) 
            {
                j["loop"] = loop;
                m_prefabModified = true;
            }

            // play on start checkbox
            bool playOnStart = j["playOnStart"];
            if (ImGui::Checkbox("Play on Start", &playOnStart)) 
            {
                j["playOnStart"] = playOnStart;
                m_prefabModified = true;
            }

            //// 3D audio checkbox
            //bool is3D = j["is3D"];
            //if (ImGui::Checkbox("3D Audio", &is3D)) 
            //{
            //    j["is3D"] = is3D;
            //    m_prefabModified = true;
            //}
        }
    };
}
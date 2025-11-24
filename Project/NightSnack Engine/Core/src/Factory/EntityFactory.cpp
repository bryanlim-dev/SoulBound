/******************************************************************************
 * File:        EntityFactory.cpp
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan
 *
 * Description:
 * Defines member functions of EntityFactory class.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "EntityFactory.h"

namespace NightSnack {

    // unused for now...
    void EntityFactory::Update() {

        static bool printed = false;
        if (!printed) {
            PrintPrefabNames();
            printed = true;
        }
    }

    void EntityFactory::PostUpdate() {

        // Destroy all removed entities
        for (Entity e : m_toBeDestroyed) {
            m_coord.DestroyEntity(e);
        }
        m_toBeDestroyed.clear();

        // Unload discarded json files
        for (size_t key : m_toBeUnloaded) {
            m_coord.GetSystem<ResourceManager>()->UnloadJson(key);
        }
        m_toBeUnloaded.clear();

    }

    Entity EntityFactory::Create(std::string const& name) {

        size_t key = m_prefabKeys[name];

        Entity e = m_prefabManager.CreateFromPrefab(GetArchetypes(key));
        if (e == INVALID_ENTITY) { return e; }

        Transform trans{};

        trans.pos.x = m_coord.GetSystem<InputSystem>()->GetMousePosX();
        trans.pos.y = m_coord.GetSystem<InputSystem>()->GetMousePosY();
        trans.pos.z = 1.f;
        // default layer...
        trans.layer = LayerType::GAMELAYER;

        trans.scale = Vec2{ 50.f, 50.f };	// default scale...

        m_coord.ModifyComponent<Transform>(e) = trans;
        entityToPrefab[e] = name;
        prefabToEntities.insert({ name, e });

        m_coord.BufferEntitySignature(e);

		return e;
	}

    // For creating entities at specific locations, scaling etc...
    Entity EntityFactory::Create(std::string const& name, Transform trans) {

        size_t key = m_prefabKeys[name];

        Entity e = m_prefabManager.CreateFromPrefab(GetArchetypes(key));
        if (e == INVALID_ENTITY) { return e; }

        m_coord.ModifyComponent<Transform>(e) = trans;
        entityToPrefab[e] = name;
        prefabToEntities.insert({ name, e });

        m_coord.BufferEntitySignature(e);

		return e;
	}

    // Clone an entity prefab at specific locations, scaling etc...
    Entity EntityFactory::Clone(Entity e, Transform trans) {

        std::string name = entityToPrefab[e];
        size_t key = m_prefabKeys[name];

        Entity clone = m_prefabManager.CreateFromPrefab(GetArchetypes(key));
        if (clone == INVALID_ENTITY) { return clone; }

        m_coord.ModifyComponent<Transform>(e) = trans;
        entityToPrefab[e] = name;
        prefabToEntities.insert({name, e});

        m_coord.BufferEntitySignature(clone);


		return clone;
	}

    void EntityFactory::RemoveEntity(Entity e) {
        m_toBeDestroyed.insert(e);
    }

    void EntityFactory::PrintPrefabNames() const {

        std::vector<std::string> names = GetPrefabNames();

        std::cout << "Loaded Prefabs: ";
        for (std::string const& s : names) {
            std::cout << s << " ";
        }
        std::cout << '\n';
    }

    std::vector<std::string> EntityFactory::GetPrefabNames() const {

        std::vector<std::string> names;
        std::unordered_map<std::string, size_t>::const_iterator it;
        for (it = m_prefabKeys.begin(); it != m_prefabKeys.end(); ++it) {
            names.emplace_back(it->first);
        }
        return names;
    }

    std::unordered_map<std::string, std::string> EntityFactory::GetPrefabMaterials() const {

        std::unordered_map<std::string, std::string> materials;
        std::unordered_map<std::string, size_t>::const_iterator it;

        for (it = m_prefabKeys.begin(); it != m_prefabKeys.end(); ++it) {

            json const& prefab = GetArchetypes(it->second);

            if (prefab.contains("Renderable")) {
                std::string mat = prefab["Renderable"]["materialName"];
                materials.insert({ it->first, mat });
            }
        }
        return materials;
    }

    std::string EntityFactory::GetEntityPrefabName(Entity e) const {
        auto it = entityToPrefab.find(e);
        if (it != entityToPrefab.end()) {
            return it->second;
        }
        return "UnknownPrefab";
    }

    void EntityFactory::UpdatePrefab(std::string const& prefab) {
        auto entities = prefabToEntities.equal_range(prefab);

        // Unload old prefab version
        size_t key = 0;
        key = m_prefabKeys[prefab];
        m_toBeUnloaded.emplace_back(key);

        // Read new version
        std::string filepath = RESOURCE_PATH + std::string{ "GameData/Prefabs" } + prefab + ".json";
        LOAD_RESULT result = m_coord.GetSystem<ResourceManager>()->LoadJson(filepath, key);
        if (result == LOAD_FAIL_BADPATH) {
            std::cerr << "Failed to read json file: " << filepath << '\n';
        }
        m_prefabKeys[prefab] = key;
        json const& j = GetArchetypes(key);

        // Update entities which use this prefab
        for (auto it = entities.first; it != entities.second; ++it ) {

            Entity e = it->second;
            m_prefabManager.UpdateComponents(e, j);
        }
    }

    json const& EntityFactory::GetArchetypes(size_t key) const {
        return m_coord.GetSystem<ResourceManager>()->GetJsonRaw(key);
    }

    void EntityFactory::LoadLevel(std::string const& levelName) {

        size_t key = m_levelKeys[levelName];
        json const& level = GetArchetypes(key);

        // Resets ECS and destroys all current entities;
        UnloadLevel();
        m_previousLevelKey = m_currentLevelKey;
        m_currentLevelKey = key;

        try {
            for (auto const& item : level["Entities"]) {
                Create(item["prefab"], item["Transform"]);
            }
            for (Entity e : m_coord.GetLivingEntities()) {
                m_coord.BufferEntitySignature(e);
            }
        }
        catch (...) {
            std::cerr << "[EntityFactory]: Failed to created entities for Level '" << levelName << "'\n";
        }
    }

    void EntityFactory::ReloadPrefabs() {
        // Queue old keys to be unloaded.
        for (auto const& [name, key] : m_prefabKeys) {
            m_toBeUnloaded.emplace_back(key);
        }
        // Clear prefab keys.
        m_prefabKeys.clear();
        // Reread json files from "GameData/Prefabs", assign keys into m_prefabKeys.
        std::filesystem::path path = std::filesystem::canonical(RESOURCE_PATH + std::string{ "GameData/Prefabs" });
        LoadJsonFolder(m_prefabKeys, path.string());
    }
    void EntityFactory::ReloadLevels() {
        // Queue old keys to be unloaded.
        for (auto const& [name, key] : m_levelKeys) {
            m_toBeUnloaded.emplace_back(key);
        }
        // Clear level keys.
        m_levelKeys.clear();
        // Reread json files from "GameData/Levels", assign keys into m_levelKeys.
        std::filesystem::path path = std::filesystem::canonical(RESOURCE_PATH + std::string{ "GameData/Levels" });
        LoadJsonFolder(m_levelKeys, path.string());
    }

    // Loads all json files within a folder given a directory path
    void EntityFactory::LoadJsonFolder(std::unordered_map<std::string, size_t>& map, std::string const& path) {

        //std::cout << "[EntityFactory] Reading Json Folder '" << path << "'\n";

        // Iterate through entries in directory.
        for (auto const& entry : std::filesystem::directory_iterator(path))
        {
            size_t key = 0;
            //std::cout << "[Entity Factory] Attempting to read: " << entry.path() << '\n';
            LOAD_RESULT result = m_coord.GetSystem<ResourceManager>()->LoadJson(entry.path().string(), key);
            //std::cout << "[EntityFactory] entry.path: " << entry.path() << '\n';
            if (result == LOAD_FAIL_BADPATH) {
                std::cerr << "Failed to read json file: " << entry.path() << '\n';
                continue;
            }
            map[entry.path().stem().string()] = key;
        }
    }

    // Save all Living entities into a Level json file (Prefab Name and Transform)
    bool EntityFactory::SaveLevel(std::string const& levelName, bool overwrite) {

        // Output file path
        std::filesystem::path outPath = "GameData/Levels" + (levelName + ".json");

        // Check if a prefab already exists with the same name
        if (std::filesystem::exists(outPath) && !overwrite) {
            std::cout << "There already exists a level named '" << levelName << "' at '" << "GameData/Levels" << '\n';
            return false;
        }

        // Output json object
        json level;
        level["Entities"] = json::array();

        // Iterate through living entities;
        for (Entity e : m_coord.GetLivingEntities()) {
            json j;

            std::string prefabName = entityToPrefab[e];
            j["prefab"] = prefabName;

            Transform const& t = m_coord.GetComponent<Transform>(e);
            std::string layer;
            switch (t.layer) {
            case LayerType::_ENGINE:
                layer = "_Engine";
                break;
            case LayerType::BACKGROUND:
                layer = "Background";
                break;
            case LayerType::FOREGROUND:
                layer = "Foreground";
                break;
            case LayerType::GAMEUI:
                layer = "GameUI";
                break;
            case LayerType::_ENGINE2:
                layer = "_Engine2";
                break;
            default:
                layer = "GameLayer";
                break;
            }

            j["Transform"] = {
                { "Position", { {"x", t.pos.x}, {"y", t.pos.y}, {"z", t.pos.z} } },
                { "Rotation", t.rot },
                { "Scale",    { {"x", t.scale.x}, {"y", t.scale.y} } },
                { "UIBasePosition", { {"x", t.UIbasePos.x}, {"y", t.UIbasePos.y}, {"z", t.UIbasePos.z} } },
				{ "UIBaseScale",    { {"x", t.UIbaseScale.x}, {"y", t.UIbaseScale.y} } },
                { "Layer", layer }
            };

            // Insert into Level json
            level["Entities"].push_back(j);
        }
        // Write file -------------------------------------------------------
        std::ofstream ofs(outPath);
        if (!ofs.is_open()) {
            std::cerr << "Failed to write level file: " << outPath << "\n";
            return false;
        }

        ofs << std::setw(4) << level;
        ofs.close();

        std::cout << "Saved level to: " << outPath << "\n";
        return true;
    }

    // Save all current components of an Entity as a Prefab
    bool EntityFactory::SaveAsPrefab(Entity e, std::string const& prefabName, bool overwrite) {

        // Output file path
        std::filesystem::path outPath = "GameData/Prefabs" + (prefabName + ".json");

        // Check if a prefab already exists with the same name
        if (std::filesystem::exists(outPath) && !overwrite) {
            std::cout << "There already exists a prefab named '" << prefabName << "' at '" << "GameData/Prefabs" << '\n';
            return false;
        }
        // JSON object that will become the prefab content
        json j;

        // RIGIDBODY ------------------------------------------------------
        if (m_coord.HasComponent<RigidBody>(e)) {
            RigidBody const& rb = m_coord.GetComponent<RigidBody>(e);
            float mass = 1.f;
            if (rb.invMass != 0.f) {
                mass = 1.f / rb.invMass;
            }
            j["RigidBody"] = {
                {"mass", mass},
                {"useGravity", rb.useGravity}
            };
        }
        // COLLIDER -------------------------------------------------------
        if (m_coord.HasComponent<Collider>(e)) {
            Collider const& c = m_coord.GetComponent<Collider>(e);
            json col;
            col["isStatic"] = c.isStatic;

            // Type
            if (c.type == ColliderType::Solid) {
                col["type"] = "Solid";
            }
            else if (c.type == ColliderType::Trigger) {
                col["type"] = "Trigger";
            }
            else if (c.type == ColliderType::Ground) {
                col["type"] = "Ground";
            }

            // Shape & dimensions
            if (c.shape == ShapeType::AABB) {
                col["shape"] = "AABB";
                col["width"] = c.width;
                col["height"] = c.height;
            }
            else if (c.shape == ShapeType::Circle) {
                col["shape"] = "Circle";
                col["radius"] = c.radius;
            }

            j["Collider"] = col;
        }
        // RENDERABLE -----------------------------------------------------
        if (m_coord.HasComponent<Renderable>(e)) {
            Renderable const& r = m_coord.GetComponent<Renderable>(e);
            j["Renderable"] = {
                {"meshName", r.meshName},
                {"materialName", r.materialName},
                {"isWireframe", r.isWireframe}
            };
        }
        // COLOR ----------------------------------------------------------
        if (m_coord.HasComponent<Color>(e)) {
            Color const& c = m_coord.GetComponent<Color>(e);
            j["Color"] = {
                {"r", c.r},
                {"g", c.g},
                {"b", c.b},
                {"a", c.a}
            };
        }
        // CAMERA ---------------------------------------------------------
        if (m_coord.HasComponent<Camera>(e)) {
            Camera const& c = m_coord.GetComponent<Camera>(e);
            j["Camera"] = {
                {"isMainCam", c.isMainCam},
                {"zoom",      c.zoom},
                {"width",     c.width},
                {"height",    c.height}
            };
        }
        // STATES ---------------------------------------------------------
        //if (m_coord.HasComponent<States>(e)) {
        //    States const& s = m_coord.GetComponent<States>(e);
        //    j["States"] = {
        //        {"States", { "StateMachine", s.}}
        //    };
        //}

        // ANIMATOR -------------------------------------------------------
        if (m_coord.HasComponent<Animator>(e)) {
            Animator const& a = m_coord.GetComponent<Animator>(e);
            j["Animator"] = {
                {"spriteSheetName", a.spriteSheetName},
                {"playbackSpeed", a.playbackSpeed},
                {"isPlaying", a.isPlaying},
                {"isLooping", a.isLooping}
            };
        }
        // FONT -----------------------------------------------------------
        if (m_coord.HasComponent<Font>(e)) {
            Font const& f = m_coord.GetComponent<Font>(e);

            j["Font"] = {
                {"fontType", f.fontType},
                {"text", f.text},
                {"fontSize", f.fontSize},
                { "textColor", {{"r", f.textColor.x}, {"g", f.textColor.y}, {"b", f.textColor.z}} },
                {"offsetPosition", {{"x", f.offsetPosition.x}, {"y", f.offsetPosition.y}, {"z", f.offsetPosition.z}} }
            };
        }
        // BUTTON ---------------------------------------------------------
        if (m_coord.HasComponent<ButtonInfo>(e)) {
            ButtonInfo const& b = m_coord.GetComponent<ButtonInfo>(e);

            std::string action, type;
            switch (b.action) {
            case ButtonAction::PLAYAUDIO:
                action = "PlayAudio";
                break;
            case ButtonAction::PLAYAUDIO2:
                action = "PlayAudio2";
                break;
            case ButtonAction::PLAYAUDIO3:
                action = "PlayAudio3";
                break;
            case ButtonAction::PLAYSTRESSTEST:
                action = "StressTest";
                break;
            case ButtonAction::CHANGELEVELS:
                action = "ChangeLevel";
                break;
            default:
                type = "Unknown";
                break;
            }
            switch (b.type) {
            case ButtonType::GAMEMENU:
                type = "GameMenu";
                break;
            default:
                type = "Unknown";
                break;
            }
            j["ButtonInfo"] = {
                {"action", action},
                {"type", type}
            };
        }
        // SCRIPT ---------------------------------------------------------
        if (m_coord.HasComponent<Script>(e)) {
            Script const& s = m_coord.GetComponent<Script>(e);
            j["Script"] = {
                {"name", s.name}
            };
        }
        //  ---------------------------------------------------------------

        // Write to file
        std::ofstream ofs(outPath);
        if (!ofs.is_open()) {
            std::cerr << "Failed to write prefab file: " << outPath << "\n";
            return false;
        }
        // pretty-print with 4-space indent
        ofs << std::setw(4) << j;
        ofs.close();
        std::cout << "Saved prefab to: " << outPath << "\n";

        // Update mapping of current entity to new created prefab
        entityToPrefab[e] = prefabName;

        return true;
    }

    // Create "Prefabs" and "Levels" folder in "assets/GameData" if they don't exist.
    void EntityFactory::CreateFolders() const {
        // Create prefabs folder if not existing
        std::filesystem::path prefabs = RESOURCE_PATH + std::string{ "GameData/Prefabs" };
        if (!std::filesystem::exists(prefabs)) {
            std::filesystem::create_directories(prefabs);
            std::cout << "[EntityFactory] Created 'Prefabs' folder\n";
        }
        else {
            std::cout << "[EntityFactory] 'Prefabs' folder already exists\n";
        }
        // Same for levels folder
        std::filesystem::path levels = RESOURCE_PATH + std::string{ "GameData/Levels" };
        if (!std::filesystem::exists(levels)) {
            std::filesystem::create_directories(levels);
            std::cout << "[EntityFactory] Created 'Levels' folder\n";
        }
        else {
            std::cout << "[EntityFactory] 'Levels' folder already exists\n";
        }
    }

    // Destroys all entities and Resets all ECS managers.
    void EntityFactory::UnloadLevel() {
        for (Entity e{}; e < MAX_ENTITIES; ++e) {
            m_coord.DestroyEntity(e);
        }
        m_coord.ResetEverything();
    }

    // Helper function to split Monolithic Json file (containing all archetypes)
    // into individual prefab json files.
    void EntityFactory::TransferPrefabs(std::string const& monolith) {
        // USE CASE:
        // std::filesystem::path monolith = GAMEDATA_DIR / "Archetypes/Archetypes.json";
        // TransferPrefabs(monolith.string());

        std::ifstream file(monolith);
        json j;
        file >> j;

        std::vector<Entity> entities;
        std::vector<std::string> names;

        for (auto const& [name, data] : j.items()) {
            entities.push_back(m_prefabManager.CreateFromPrefab(data));
            names.push_back(name);
        }
        for (int i{}; i < entities.size(); ++i) {
            SaveAsPrefab(entities[i], names[i], true);
            RemoveEntity(entities[i]);
        }
        std::cout << "Prefabs transferred\n";
    }

}	// NightSnack

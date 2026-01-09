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

    // Called at the end of Core loop to unload prefabs and destroy entities.
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

    // Default create an entity with no components (except transform)
    Entity EntityFactory::Create() {

        Entity e = m_coord.CreateEntity();
        if (e == INVALID_ENTITY) { 
            std::cerr << "[EntityFactory] Failed to default create Entity\n";
            return e; 
        }

        Transform trans{ Vec3{0.f, 0.f, 0.f}, 0.f, Vec2{50.f, 50.f} };
        trans.layer = LayerType::GAMELAYER;

        m_coord.AddComponent<Transform>(e, trans);
        entityToPrefab[e] = "++Default";
        prefabToEntities.insert({ "++Default", e });

        m_coord.BufferEntitySignature(e);

        return e;
    }

    // Create with prefab name
    Entity EntityFactory::Create(std::string const& name) {

        size_t key = m_prefabKeys[name];
        if (key == 0) return Create();

        Entity e = m_prefabManager.CreateFromPrefab(GetArchetypes(key));
        if (e == INVALID_ENTITY) { return e; }

        Transform trans{};

        trans.pos.x = m_coord.GetSystem<InputSystem>()->GetMousePosX();
        trans.pos.y = m_coord.GetSystem<InputSystem>()->GetMousePosY();
        trans.pos.z = 0.f;
        // default layer...
        trans.layer = LayerType::GAMELAYER;

        trans.scale = Vec2{ 50.f, 50.f };	// default scale...

        m_coord.ModifyComponent<Transform>(e) = trans;
        entityToPrefab[e] = name;
        prefabToEntities.insert({ name, e });

        m_coord.BufferEntitySignature(e);

		return e;
	}

    // Create an entity with a json file of entity information and a Transform
    Entity EntityFactory::Create(json const& prefab, Transform const& trans) {
        Entity e = m_prefabManager.CreateFromPrefab(prefab);
        if (e == INVALID_ENTITY) { return e; }

        m_coord.ModifyComponent<Transform>(e) = trans;

        entityToPrefab[e] = prefab["name"];
        prefabToEntities.insert({ prefab["name"], e });

        m_coord.BufferEntitySignature(e);

        return e;
    }

    // For creating entities at specific locations, scaling etc...
    Entity EntityFactory::Create(std::string const& name, Transform const& trans) {

        size_t key = m_prefabKeys[name];
        if (key == 0) return Create();

        Entity e = m_prefabManager.CreateFromPrefab(GetArchetypes(key));
        if (e == INVALID_ENTITY) { return e; }

        m_coord.ModifyComponent<Transform>(e) = trans;
        entityToPrefab[e] = name;
        prefabToEntities.insert({ name, e });

        m_coord.BufferEntitySignature(e);

		return e;
	}

    // Clone an entity prefab at specific locations, scaling etc...
    Entity EntityFactory::Clone(Entity e) {

        std::string name = entityToPrefab[e];
        size_t key = m_prefabKeys[name];
        if (key == 0) return Create();

        Entity clone = m_prefabManager.CreateFromPrefab(GetArchetypes(key));
        if (clone == INVALID_ENTITY) { return clone; }

        Transform& t = m_coord.ModifyComponent<Transform>(clone);
        t = m_coord.GetComponent<Transform>(e);
        t.pos.x = m_coord.GetSystem<InputSystem>()->GetMousePosX();
        t.pos.y = m_coord.GetSystem<InputSystem>()->GetMousePosY();

        entityToPrefab[clone] = name;
        prefabToEntities.insert({name, clone});

        m_coord.BufferEntitySignature(clone);

		return clone;
	}

    // Queue entity to be destroyed at end of Core loop
    void EntityFactory::RemoveEntity(Entity e) {
        m_toBeDestroyed.insert(e);
    }

    // Print names of loaded prefabs
    void EntityFactory::PrintPrefabNames() const {

        std::vector<std::string> names = GetPrefabNames();

        std::cout << "Loaded Prefabs: ";
        for (std::string const& s : names) {
            std::cout << s << " ";
        }
        std::cout << '\n';
    }
    // Obtain names of loaded prefabs
    std::vector<std::string> EntityFactory::GetPrefabNames() const {

        std::vector<std::string> names;
        std::unordered_map<std::string, size_t>::const_iterator it;
        for (it = m_prefabKeys.begin(); it != m_prefabKeys.end(); ++it) {
            names.emplace_back(it->first);
        }
        return names;
    }

    // Obtain materials of prefabs if they have a "Renderable" component
    std::unordered_map<std::string, std::string> EntityFactory::GetPrefabMaterials() const {

        std::unordered_map<std::string, std::string> materials;
        std::unordered_map<std::string, size_t>::const_iterator it;

        for (it = m_prefabKeys.begin(); it != m_prefabKeys.end(); ++it) {

            if (it->second == 0) continue;
            json const& prefab = GetArchetypes(it->second);

            if (prefab.contains("Renderable")) {
                std::string mat = prefab["Renderable"]["materialName"];
                materials.insert({ it->first, mat });
            }
        }
        return materials;
    }

    // Obtain the prefab name of a specific entity
    std::string EntityFactory::GetEntityPrefabName(Entity e) const {
        auto it = entityToPrefab.find(e);
        if (it != entityToPrefab.end()) {
            return it->second;
        }
        return "UnknownPrefab";
    }

    // Reloads a specific prefab -> Updates json stored by the RM
    void EntityFactory::UpdatePrefab(std::string const& prefab) {
        auto entities = prefabToEntities.equal_range(prefab);

        // Unload old prefab version
        size_t key = 0;
        key = m_prefabKeys[prefab];

        // Read new version
        std::filesystem::path path = RESOURCE_PATH + ("GameData/Prefabs/" + prefab + ".json");
        std::ifstream ifs(path);
        if (!ifs) {
			std::cerr << "Failed to read json file: " << path << '\n';
			return;
        }
        json j;
        ifs >> j;

		// Update primary json stored in ResourceManager
		m_coord.GetSystem<ResourceManager>()->UpdateJson(key, j);

        // Update entities which use this prefab
        for (auto it = entities.first; it != entities.second; ++it ) {

            Entity e = it->second;
            m_prefabManager.UpdateComponents(e, j);
        }
    }

    // Get a specific json with a key from the RM
    json const& EntityFactory::GetArchetypes(size_t key) const {
        return m_coord.GetSystem<ResourceManager>()->GetJsonRaw(key);
    }

    // Create all entities in a level given a level json
    void EntityFactory::CreateLevelEntities(json const& level) {
        // Resets ECS and destroys all current entities;
        DestroyLevelEntities();
        try {
            for (auto const& item : level["Entities"]) {
                Create(std::string{ item["prefab"] }, item["Transform"]);
                //std::cout << "[EntityFactory] Creating prefab<" << item["prefab"] << "> at ("
                //    << item["Transform"]["Position"]["x"] << ", "
                //    << item["Transform"]["Position"]["y"] << ", "
                //    << item["Transform"]["Position"]["z"] << ")\n";
            }
        }
        catch (...) {
            std::cerr << "[EntityFactory]: Failed to create entities for Level\n";
        }
    }

    // Reload all prefabs in "./assets/Prefabs/"
    void EntityFactory::ReloadPrefabs() {
        // Queue old keys to be unloaded.
        for (auto const& [name, key] : m_prefabKeys) {
            m_toBeUnloaded.emplace_back(key);
        }
        // Clear prefab keys.
        m_prefabKeys.clear();
        // Reread json files from "GameData/Prefabs", assign keys into m_prefabKeys.
        //std::filesystem::path path = std::filesystem::canonical(PREFABS_PATH);
        LoadJsonFolder(m_prefabKeys, "Prefabs/");
    }

    // Loads all json files within a folder given a directory path
    void EntityFactory::LoadJsonFolder(std::unordered_map<std::string, size_t>& map, std::string const& path) {

        std::string fullpath = RESOURCE_PATH + ("GameData/" + path);
        //std::cout << "[EntityFactory] Reading Json Folder '" << fullpath << "'\n";

        // Iterate through entries in directory.
        for (auto const& entry : std::filesystem::directory_iterator(fullpath))
        {
            size_t key = 0;

            std::string entrypath = "GameData/" + path + entry.path().filename().string();
            //std::cout << "[Entity Factory] Attempting to read: " << entrypath << '\n';
      
            LOAD_RESULT result = m_coord.GetSystem<ResourceManager>()->LoadJson(entrypath, key);
            //std::cout << "[EntityFactory] entry.path: " << entry.path() << '\n';
            if (result == LOAD_FAIL_BADPATH) {
                std::cerr << "Failed to read json file: " << entry.path() << '\n';
                continue;
            }
            map[entry.path().stem().string()] = key;
        }
    }

    // Save all Living entities into a Level json file (Prefab Name and Transform)
    json EntityFactory::SaveLevelEntities() {

        json entities = json::array();

        // Iterate through living entities;
        for (Entity e : m_coord.GetLivingEntities()) {
            json j;

            Transform const& t = m_coord.GetComponent<Transform>(e);
            std::string prefabName = entityToPrefab[e];
            if (prefabName == "++Default" && !t.prefabModified) {
                std::cerr << "Default entity not saved\n";
                continue;
            }
            // Output file path
            std::filesystem::path outPath = RESOURCE_PATH + ("GameData/Prefabs/" + prefabName + ".json");

            // Edge case: Prefab was deleted and entity using it still exists within a level
            if (!std::filesystem::exists(outPath)) {
                std::cout << "Saved as prefab: " << prefabName << '\n';
                SaveAsPrefab(e, prefabName);
            }
            // Prefab was modified and not saved
            if (t.prefabModified) {
                // Check if a prefab already exists with the same name
                bool exists = std::filesystem::exists(outPath);
                size_t cnt = 1;

                std::string newPrefabName;
                while (exists) {
                    // prefab1, prefab2 etc...
                    newPrefabName = prefabName + std::to_string(cnt);
                    // check if new name already exists;
                    std::filesystem::path tempout = RESOURCE_PATH + ("GameData/Prefabs/" + newPrefabName + ".json");
                    exists = std::filesystem::exists(tempout);
                    cnt++;
                }
                j["prefab"] = newPrefabName;
                SaveAsPrefab(e, newPrefabName);
            }
            else {
                j["prefab"] = prefabName;
            }

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
            entities.push_back(j);
        }
        return entities;
    }

    // Store all entity component information into a json and returns it.
    json EntityFactory::CacheEntityComponents(Entity e) const {
        json j;

        auto it = entityToPrefab.find(e);
        if (entityToPrefab.find(e) != entityToPrefab.end()) {
            j["name"] = it->second;
        }
        if (m_coord.GetComponent<Transform>(e).prefabModified) j["modified"] = true;
        else j["modified"] = false;

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
            }
            else if (c.shape == ShapeType::Circle) {
                col["shape"] = "Circle";
            }
            col["width"] = c.width;
            col["height"] = c.height;
            col["radius"] = c.radius;

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
            std::string action, type, target;

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

            case ButtonAction::LOADLEVEL: action = "LoadLevel"; break;
            case ButtonAction::OPENMENU: action = "OpenMenu"; break;
            default: type = "Unknown"; break;
            }

            switch (b.type) {
            case ButtonType::CLICKER: type = "Clicker"; break;
            case ButtonType::SLIDER: type = "Slider"; break;
            default: type = "Unknown";  break;
            }
            target = b.target;

            j["ButtonInfo"] = { {"action", action}, {"type", type}, {"target", target} };
        }
        // SCRIPT ---------------------------------------------------------
        if (m_coord.HasComponent<Script>(e)) {
            Script const& s = m_coord.GetComponent<Script>(e);
            j["Script"] = {
                {"name", s.names}
            };
        }
        //  ---------------------------------------------------------------
        if (m_coord.HasComponent<PlayerController>(e)) {    // temp
            PlayerController const& p = m_coord.GetComponent<PlayerController>(e);
            j["PlayerController"] = {
                {"moveSpeed", p.moveSpeed},
                {"jumpSpeed", p.jumpSpeed}
            };
        }
        // AUDIO ----------------------------------------------------------
        if (m_coord.HasComponent<Audio>(e)) {
            Audio const& a = m_coord.GetComponent<Audio>(e);
            j["Audio"] = {
                {"audioKey", a.audioKey},
                {"isPlaying", a.isPlaying},
                {"loop", a.loop},
                {"playOnStart", a.playOnStart},
                { "volume", a.volume }
            };
        }
        return j;
    }

    // Save all current components of an Entity as a Prefab
    bool EntityFactory::SaveAsPrefab(Entity e, std::string const& prefabName, bool overwrite) {

        // Output file path
        std::filesystem::path outPath = RESOURCE_PATH + ("GameData/Prefabs/" + prefabName + ".json");

        // Check if a prefab already exists with the same name
        if (std::filesystem::exists(outPath) && !overwrite) {
            std::cout << "There already exists a prefab named '" << prefabName << "' at '" << "GameData/Prefabs" << '\n';
            return false;
        }
        // JSON object that will become the prefab content
        json j = CacheEntityComponents(e);
        if (j.contains("name")) j.erase("name");
        if (j.contains("modified")) j.erase("modified");

        // Write to file
        std::ofstream ofs(outPath);
        if (!ofs.is_open()) {
            std::cerr << "[EntityFactory] Failed to write to prime prefab file: " << outPath << '\n';
            return false;
        }
        ofs << std::setw(4) << j;
        ofs.close();
        std::cout << "[EntityFactory] Saved prefab to: " << outPath << '\n';

        // Use ResourceManager to load new prefab file
        m_coord.GetSystem<ResourceManager>()->UpdateJson(m_prefabKeys[prefabName], j);
        //std::string prefabpath = "GameData/Prefabs/" + prefabName + ".json";
        std::cout << "[EntityFactory] Updated Json in RM for: " << prefabName << '\n';

        // Update mappings of current entity to new created prefab
        entityToPrefab[e] = prefabName;
        prefabToEntities.insert({ prefabName, e });
        m_prefabNames.push_back(prefabName);

        // Update entities of this prefab;
        auto range = prefabToEntities.equal_range(prefabName);
        // Cache transforms
        std::vector<Transform> transCache;
        for (auto i = range.first; i != range.second; ++i) {
            transCache.emplace_back(m_coord.GetComponent<Transform>(i->second));
            RemoveEntity(i->second);
        }
        for (Transform& t : transCache) {
            Create(prefabName, t);
        }

        m_coord.ModifyComponent<Transform>(e).prefabModified = false;

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
    void EntityFactory::DestroyLevelEntities() {
        for (Entity e{}; e < MAX_ENTITIES; ++e) {
            m_coord.DestroyEntity(e);
        }
        m_coord.ResetEverything();

        entityToPrefab.clear();
        prefabToEntities.clear();
        m_prefabKeys["++Default"] = 0;
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
    // Rewrites all entity prefab jsons, to be used if need to update format of all json files 
    void EntityFactory::GlobalPrefabReload() {
        std::vector<std::pair<Entity, std::string>> entities;

		for (std::string& s : GetPrefabNames()) {
            Entity e = Create(s, Transform{ Vec3{}, 0.f, Vec2{50.f,50.f} });
            entities.push_back({ e, s });
		}
        for (auto const& [e, name] : entities) {
            SaveAsPrefab(e, name, 1);
        }
		// Remove all created entities
		for (auto const& [e, name] : entities) {
			RemoveEntity(e);
		}
    }
}	// NightSnack

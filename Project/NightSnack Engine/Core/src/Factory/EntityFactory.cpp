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

        for (std::pair<std::string, Transform>& p : m_prefabCache) {
            Create(p.first, p.second);
        }
        m_prefabCache.clear();
    }

    // Called at the end of Core loop to unload prefabs and destroy entities.
    void EntityFactory::PostUpdate() {

        // Destroy all removed entities
        for (Entity e : m_toBeDestroyed) {

            if (entityToPrefab.find(e) != entityToPrefab.end()) {

                auto range = prefabToEntities.equal_range(entityToPrefab[e]);
                std::unordered_multimap<std::string, Entity>::iterator it;

                for (auto i = range.first; i != range.second; ++i) {
                    if (i->second == e) {
                        it = i;
                        break;
                    }
                }
                entityToPrefab.erase(e);
                prefabToEntities.erase(it);
            }
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
        m_coord.AddComponent<Hierarchy>(e, Hierarchy{});

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
    Entity EntityFactory::Create(json const& j, Transform const& trans) {
        Entity e = INVALID_ENTITY;

        m_coord.ModifyComponent<Transform>(e) = trans;

        if (j["isPrefab"]) {
            e = m_prefabManager.CreateFromPrefab(j["name"]);

            entityToPrefab[e] = j["name"];
            prefabToEntities.insert({ j["name"], e });
        }
        else {
            e = Create();
            UnpackEntityComponents(e, j["Components"]);
        }

        m_coord.BufferEntitySignature(e);

        return e;
    }

    // Create entity from prefab with a specific Transform;
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

    void EntityFactory::MapEntityPrefab(Entity e, std::string const& name) {
        
        if (entityToPrefab.find(e) != entityToPrefab.end()) {
            std::string old = entityToPrefab[e];
            entityToPrefab.erase(e);
            entityToPrefab[e] = name;

            auto const& it = prefabToEntities.equal_range(old);
            for (auto i = it.first; i != it.second; ++i) {
                if (i->second == e) {
                    prefabToEntities.erase(i);
                    break;
                }
            }
        }
        prefabToEntities.insert({ name, e });
        m_prefabNames.push_back(name);
    }

    // Clone an entity prefab at specific locations, scaling etc...
    Entity EntityFactory::Clone(Entity e, Vec2 const& pos) {

        Entity clone = INVALID_ENTITY;
        if (entityToPrefab.find(e) != entityToPrefab.end()) {

            std::string name = entityToPrefab[e];
            size_t key = m_prefabKeys[name];

            clone = m_prefabManager.CreateFromPrefab(GetArchetypes(key));

            entityToPrefab[clone] = name;
            prefabToEntities.insert({ name, clone });
        }
        else {
            json j = CacheEntityComponents(e);
            
            clone = Create();
            UnpackEntityComponents(clone, j);
        }
        if (clone == INVALID_ENTITY) { return clone; }

        Transform& t = m_coord.ModifyComponent<Transform>(clone);
        t = m_coord.GetComponent<Transform>(e);
        t.pos.x = m_coord.GetSystem<InputSystem>()->GetMouseWorldPosX() + pos.x;
        t.pos.y = m_coord.GetSystem<InputSystem>()->GetMouseWorldPosY() + pos.y;

        m_coord.BufferEntitySignature(clone);

		return clone;
	}

    // Queue entity to be destroyed at end of Core loop
    void EntityFactory::RemoveEntity(Entity e) { 

        // Update Hierarchy chain
        Hierarchy const& h = m_coord.GetComponent<Hierarchy>(e);

        // if has parent
        if (h.parent != INVALID_ENTITY) {

            // erase himself from parent's children
            Hierarchy& ph = m_coord.ModifyComponent<Hierarchy>(h.parent);
            ph.children.erase(e);

            // add children to parent's children
            for (Entity c : h.children) {
                m_coord.ModifyComponent<Hierarchy>(c).parent = h.parent;
                ph.children.insert(c);
            }
        }
        else {
            // else unparent all children
            for (Entity c : h.children) {
                m_coord.ModifyComponent<Hierarchy>(c).parent = INVALID_ENTITY;
            }
        }

        // Mark Entity for deletion
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
        return m_coord.GetComponent<Transform>(e).name;
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

    void EntityFactory::UnpackEntityComponents(Entity e, json const& j) {
        if (j.contains("RigidBody")) m_coord.AddComponent<RigidBody>(e, j["RigidBody"]);
        if (j.contains("Collider")) m_coord.AddComponent<Collider>(e, j["Collider"]);
        if (j.contains("Renderable")) m_coord.AddComponent<Renderable>(e, j["Renderable"]);
        if (j.contains("Color")) m_coord.AddComponent<Color>(e, j["Color"]);
        if (j.contains("Animator")) m_coord.AddComponent<Animator>(e, j["Animator"]);
        if (j.contains("Font")) m_coord.AddComponent<Font>(e, j["Font"]);
        if (j.contains("ButtonInfo")) m_coord.AddComponent<ButtonInfo>(e, j["ButtonInfo"]);
        if (j.contains("Script")) m_coord.AddComponent<Script>(e, j["Script"]);
        if (j.contains("PlayerController")) m_coord.AddComponent<PlayerController>(e, j["PlayerController"]);
        if (j.contains("Audio")) m_coord.AddComponent<Audio>(e, j["Audio"]);
    }

    // Create all entities in a level given a level json
    void EntityFactory::CreateLevelEntities(json const& level) {
        // Resets ECS and destroys all current entities;
        //DestroyLevelEntities();
        try {
            for (auto const& item : level["Entities"]) {
                Entity e;
                if (item["isPrefab"]) {
                    e = Create(std::string{ item["Prefab"] }, item["Transform"]);
                    //std::cout << "[EntityFactory] Creating prefab<" << item["Prefab"] << "> at ("
                    //    << item["Transform"]["Position"]["x"] << ", "
                    //    << item["Transform"]["Position"]["y"] << ", "
                    //    << item["Transform"]["Position"]["z"] << ")\n";
                }
                else if (item.contains("Components")) {
                    e = Create();
                    m_coord.ModifyComponent<Transform>(e) = item["Transform"];
                    UnpackEntityComponents(e, item["Components"]);
                }
                else {
                    std::cerr << "[EntityFactory] Failed to create a Level Entity, no <Components> or <Prefab> in json\n";
                    continue;
                }

                //if (item.contains("Collider")) {
                //    if (m_coord.HasComponent<Collider>(e))
                //        m_coord.ModifyComponent<Collider>(e) = item["Collider"];
                //    else
                //        m_coord.AddComponent<Collider>(e, item["Collider"]);
                //    //if (m_coord.ModifyComponent<Collider>(e).shape == ShapeType::AABB)
                //    //    std::cout << "Entity collider: AABB\n";
                //    //else
                //    //    std::cout << "Entity collider: Circle\n";
                //}

                // Create child entities
                if (item.contains("Children")) {
                    CreateEntityChildren(item.at("Children"), e);
                }
            }
        }
        catch (std::exception const& e) {
            std::cerr << "Invalid JSON format " << e.what() << '\n';
            std::cerr << "[EntityFactory]: Failed to create entities for Level\n";
        }
    }
    void EntityFactory::CreateEntityChildren(json const& j, Entity parent) {

        Hierarchy& h = m_coord.ModifyComponent<Hierarchy>(parent);

        for (auto const& e : j) {
            Entity child;
            
            if (e["isPrefab"]) {
                child = Create(std::string{ e["Prefab"] }, e["Transform"]);
            }
            else {
                child = Create();
                try {
                    m_coord.ModifyComponent<Transform>(child) = e["Transform"];
                    UnpackEntityComponents(child, e["Components"]);
                }
                catch (std::exception& e) {
                    std::cerr << "[Factory] Failed to create child: << " << e.what() << '\n';
                }
            }

            if (e.contains("Collider")) {
                if (m_coord.HasComponent<Collider>(child))
                    m_coord.ModifyComponent<Collider>(child) = e["Collider"];
                else
                    m_coord.AddComponent<Collider>(child, e["Collider"]);
            }

            m_coord.ModifyComponent<Hierarchy>(child).parent = parent;
            h.children.insert(child);

            // Create child entities
            if (e.contains("Children"))
                CreateEntityChildren(e["Children"], child);
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

    json EntityFactory::CacheEntityChildren(Hierarchy const& h) {
        json j = json::array();

        for (Entity c : h.children) {
            try {
                json child;

                child["Transform"] = m_coord.GetComponent<Transform>(c);

                if (entityToPrefab.find(c) != entityToPrefab.end()) {
                    child["Prefab"] = entityToPrefab[c];
                    child["isPrefab"] = true;
                }
                else {
                    child["Components"] = CacheEntityComponents(c);
                    child["isPrefab"] = false;
                }
                if (m_coord.HasComponent<Collider>(c)) {
                    child["Collider"] = m_coord.GetComponent<Collider>(c);
                }

                Hierarchy const& hc = m_coord.GetComponent<Hierarchy>(c);
                if (!hc.children.empty()) {
                    child["Children"] = CacheEntityChildren(hc);
                }
                j.push_back(child);
            }
            catch (std::exception const& e) {
                std::cerr << "[EntityFactory] Failed to create child entity\n";
                std::cerr << "Invalid JSON format " << e.what() << '\n';
            }
        }
        return j;
    }

    Entity EntityFactory::CreateCollider(Collider const& c, Transform const& t) {

        Entity e = Create();
        Transform& trans = m_coord.ModifyComponent<Transform>(e);
        trans = t;
        trans.name = "Collider";
        m_coord.AddComponent<Collider>(e, c);

        // Invisible renderer component
        m_coord.AddComponent<Renderable>(e, Renderable{});
        m_coord.AddComponent<Color>(e, Color{});    // zero alpha

        m_coord.AddComponent<RigidBody>(e, RigidBody{});

        return e;
    }
    Entity EntityFactory::CreateSprite(Renderable const& r, Transform const& t) {
        Entity e = Create();
        Transform& trans = m_coord.ModifyComponent<Transform>(e);
        trans = t;
        trans.name = "Sprite-" + t.name;
        m_coord.AddComponent<Renderable>(e, r);

        return e;
    }

    json EntityFactory::CacheLevelEntities() {
        json entities = json::array();

        for (Entity e : m_coord.GetLivingEntities()) {

            // Skip camera entities (they should be handled by SceneManager and Level.jsons)
            if (m_coord.HasComponent<Camera>(e)) continue;

            Hierarchy const& h = m_coord.GetComponent<Hierarchy>(e);
            Transform const& t = m_coord.GetComponent<Transform>(e);

            // Only process root entities for level saving
            if (h.parent != INVALID_ENTITY) continue;
            if (t.name == "CamTrigger") continue;

            // Build entity JSON
            json j;
            j["Transform"] = t;
            if (entityToPrefab.find(e) != entityToPrefab.end()) {
                j["Prefab"] = entityToPrefab[e];
                j["isPrefab"] = true;
            }
            else {
                j["Components"] = CacheEntityComponents(e);
                j["isPrefab"] = false;
            }

            if (!h.children.empty()) {
                j["Children"] = CacheEntityChildren(h);
            }

            entities.push_back(j);
        }

        return entities;
    }

    json EntityFactory::SaveLevelEntities() {
        json entities = json::array();

        for (Entity e : m_coord.GetLivingEntities()) {

            // Skip camera entities (they should be handled by SceneManager and Level.jsons)
            if (m_coord.HasComponent<Camera>(e)) continue;

            Hierarchy const& h = m_coord.GetComponent<Hierarchy>(e);
            Transform const& t = m_coord.GetComponent<Transform>(e);
            if (t.name == "CamTrigger") continue;

            // Only process root entities for level saving
            if (h.parent != INVALID_ENTITY) continue;

            // Build entity JSON
            json j;

            j["Transform"] = t;

            if (entityToPrefab.find(e) != entityToPrefab.end()) {
                std::string prefabName = entityToPrefab[e];

                // Handle prefab saving/updating
                SaveAsPrefab(e, prefabName, t.prefabModified);
                j["Prefab"] = prefabName;
                j["isPrefab"] = true;
            }
            else {
                j["Components"] = CacheEntityComponents(e);
                j["isPrefab"] = false;
            }

            if (!h.children.empty()) {
                j["Children"] = CacheEntityChildren(h);
            }

            entities.push_back(j);
        }

        return entities;
    }

    std::string EntityFactory::EnsurePrefabExists(Entity e,
        std::string const& prefabName,
        bool isModified) {
        std::filesystem::path basePath = RESOURCE_PATH + std::string{ "GameData/Prefabs/" };
        std::filesystem::path prefabPath = basePath / (prefabName + ".json");

        // Prefab doesn't exist - create it
        if (!std::filesystem::exists(prefabPath)) {
            SaveAsPrefab(e, prefabName);
            return prefabName;
        }

        // Prefab exists but wasn't modified - use original
        if (!isModified) {
            return prefabName;
        }

        // Prefab was modified - create variant with unique name
        std::string variantName;
        size_t counter = 1;

        do {
            variantName = prefabName + std::to_string(counter++);
            prefabPath = basePath / (variantName + ".json");
        } while (std::filesystem::exists(prefabPath));

        SaveAsPrefab(e, variantName);
        entityToPrefab[e] = variantName;  // Update mapping

        return variantName;
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
        if (m_coord.HasComponent<RigidBody>(e)) j["RigidBody"] = m_coord.GetComponent<RigidBody>(e);
        // COLLIDER -------------------------------------------------------
        if (m_coord.HasComponent<Collider>(e)) j["Collider"] = m_coord.GetComponent<Collider>(e);
        // RENDERABLE -----------------------------------------------------
        if (m_coord.HasComponent<Renderable>(e)) j["Renderable"] = m_coord.GetComponent<Renderable>(e);
        // COLOR ----------------------------------------------------------
        if (m_coord.HasComponent<Color>(e)) j["Color"] = m_coord.GetComponent<Color>(e);
        // CAMERA ---------------------------------------------------------
        //if (m_coord.HasComponent<Camera>(e)) j["Camera"] = m_coord.GetComponent<Camera>(e);
        // ANIMATOR -------------------------------------------------------
        if (m_coord.HasComponent<Animator>(e)) j["Animator"] = m_coord.GetComponent<Animator>(e);
        // FONT -----------------------------------------------------------
        if (m_coord.HasComponent<Font>(e)) j["Font"] = m_coord.GetComponent<Font>(e);
        // BUTTON ---------------------------------------------------------
        if (m_coord.HasComponent<ButtonInfo>(e)) j["ButtonInfo"] = m_coord.GetComponent<ButtonInfo>(e);
        // SCRIPT ---------------------------------------------------------
        if (m_coord.HasComponent<Script>(e)) j["Script"] = m_coord.GetComponent<Script>(e);
        //  ---------------------------------------------------------------
        if (m_coord.HasComponent<PlayerController>(e)) j["PlayerController"] = m_coord.GetComponent<PlayerController>(e);
        // AUDIO ----------------------------------------------------------
        if (m_coord.HasComponent<Audio>(e)) j["Audio"] = m_coord.GetComponent<Audio>(e);
        // CHARACTER ----------------------------------------------------------
        if (m_coord.HasComponent<Character>(e)) j["Character"] = m_coord.GetComponent<Character>(e);

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

        // Update entities of this prefab;
        m_prefabCache.clear();
        // Cache transforms
        auto range = prefabToEntities.equal_range(prefabName);
        for (auto i = range.first; i != range.second; ++i) {
            std::pair<std::string, Transform> p (prefabName, m_coord.GetComponent<Transform>(i->second));
            m_prefabCache.push_back(p);
            RemoveEntity(i->second);
        }
        // Update mappings of current entity to new created prefab
        //if (!std::filesystem::exists(outPath) && !overwrite) {
        entityToPrefab[e] = prefabName;
        prefabToEntities.insert({ prefabName, e });
        m_prefabNames.push_back(prefabName);
        //}

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

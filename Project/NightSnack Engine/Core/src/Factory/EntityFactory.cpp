/******************************************************************************
 * File:        EntityFactory.cpp
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan
 *
 * Description:
 * Defines member functions of EntityFactory class.
 *
 * Copyright � 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "EntityFactory.h"

namespace NightSnack {

	// unused for now...
	void EntityFactory::Update(int steps) {
		(void)steps;
		
		static bool printed = false;

		if (!printed) {
			PrintPrefabNames();
			printed = true;
		}
	}

	void EntityFactory::PostUpdate() {
		
		for (Entity e : m_toBeDestroyed) {
			m_coord.DestroyEntity(e);
		}
        m_toBeDestroyed.clear();

	}

	Entity EntityFactory::Create(std::string const& name) {
		Entity e = m_prefabManager.CreateFromPrefab(name, GetArchetypes());
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

		return e;
	}

	// For creating entities at specific locations, scaling etc...
	Entity EntityFactory::Create(std::string const& name, Transform trans) {
		Entity e = m_prefabManager.CreateFromPrefab(name, GetArchetypes());
		if (e == INVALID_ENTITY) { return e; }

		m_coord.ModifyComponent<Transform>(e) = trans;
		
		entityToPrefab[e] = name;

		return e;
	}

	// Clone an entity prefab at specific locations, scaling etc...
	Entity EntityFactory::Clone(Entity e, Transform trans) {
		std::string name = entityToPrefab[e];

		Entity clone = m_prefabManager.CreateFromPrefab(name, GetArchetypes());
		if (clone == INVALID_ENTITY) { return clone; }

		m_coord.ModifyComponent<Transform>(e) = trans;

		entityToPrefab[e] = name;

		return clone;
	}

	void EntityFactory::RemoveEntity(Entity e) {
		m_toBeDestroyed.insert(e);
	}

	void EntityFactory::PrintPrefabNames() const {

		std::cout << "Prefabs: ";
		for (auto& [name, data] : GetArchetypes().items()) {
			std::cout << name << " ";
		}
		std::cout << '\n';
	}

	std::vector<std::string> const& EntityFactory::GetPrefabNames() const {
		return m_prefabNames;
	}

	std::unordered_map<std::string, std::string> EntityFactory::GetPrefabMaterials() const {
		std::unordered_map<std::string, std::string> materials;

		for (auto& [name, data] : GetArchetypes().items()) {
			if (data.contains("Renderable")) {
				std::string mat = data["Renderable"]["materialName"];
				materials.insert({ name, mat });
			}
		}
		return materials;
	}

	std::string EntityFactory::GetEntityPrefabName(Entity e) const
	{
		auto it = entityToPrefab.find(e);
		if (it != entityToPrefab.end()) {
			return it->second;
		}
		return "UnknownPrefab";
	}

	const json& EntityFactory::GetArchetypes() const
	{
		return m_coord.GetSystem<ResourceManager>()->GetJsonRaw(m_archetypesKey);
	}

    bool EntityFactory::SaveLevel(std::string const& levelName, bool overwrite) {

        // Output file path
        std::filesystem::path outPath = GAMEDATA_DIR / "Levels" / (levelName + ".json");

        // Check if a prefab already exists with the same name
        if (std::filesystem::exists(outPath) && !overwrite) {
            std::cout << "There already exists a level named '" << levelName << "' at '" << GAMEDATA_DIR / "Levels" << '\n';
            return false;
        }

        // Output json object
        json level;
        level["Entities"] = json::array();

        // Iterate through living entities;
        for (Entity e : m_coord.GetLivingEntities()) {
            json j;

            std::string prefabName = entityToPrefab[e];
            j["prefab"] = { prefabName };

            Transform const& t = m_coord.GetComponent<Transform>(e);
            j["Transform"] = {
                { "Position", { {"x", t.pos.x}, {"y", t.pos.y}, {"z", t.pos.z} } },
                { "Rotation", t.rot },
                { "Scale",    { {"x", t.scale.x}, {"y", t.scale.y} } }
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

    bool EntityFactory::SaveAsPrefab(Entity e, std::string const& prefabName, bool overwrite) {

        // Output file path
        std::filesystem::path outPath = GAMEDATA_DIR / "Prefabs" / (prefabName + ".json");

        // Check if a prefab already exists with the same name
        if (std::filesystem::exists(outPath) && !overwrite) {
            std::cout << "There already exists a prefab named '" << prefabName << "' at '" << GAMEDATA_DIR / "Prefabs" << '\n';
            return false;
        }
        // JSON object that will become the prefab content
        json j;

        // TRANSFORM ------------------------------------------------------
        //if (m_coord.HasComponent<Transform>(e)) {
        //    Transform const& t = m_coord.GetComponent<Transform>(e);
        //    j["Transform"] = {
        //        { "Position", { {"x", t.pos.x}, {"y", t.pos.y}, {"z", t.pos.z} } },
        //        { "Rotation", t.rot },
        //        { "Scale",    { {"x", t.scale.x}, {"y", t.scale.y} } }
        //    };
        //}
        
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
                {"textColor", {{"r", f.textColor.x}, {"g", f.textColor.y}, {"b", f.textColor.z}} }
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

    void EntityFactory::CreateFolders() const {
        // Create prefabs folder if not existing
        std::filesystem::path prefabs = GAMEDATA_DIR / "Prefabs";
        if (!std::filesystem::exists(prefabs)) {
            std::filesystem::create_directories(prefabs);
            std::cout << "[EntityFactory] Created 'Prefabs' folder\n";
        }
        // Same for levels folder
        std::filesystem::path levels = GAMEDATA_DIR / "Levels";
        if (!std::filesystem::exists(levels)) {
            std::filesystem::create_directories(levels);
            std::cout << "[EntityFactory] Created 'Levels' folder\n";
        }
    }

    void EntityFactory::UnloadLevel() {
        for (Entity e{}; e < MAX_ENTITIES; ++e) {
            m_coord.DestroyEntity(e);
        }
        m_coord.ResetEverything();
    }

    void EntityFactory::TransferPrefabs() {
        {
            std::vector<Entity> entities;
            std::vector<std::string> names;
            for (auto& [name, data] : GetArchetypes().items()) {
                entities.push_back(Create(name));
                names.push_back(name);
            }
            for (int i{}; i < entities.size(); ++i) {
                SaveAsPrefab(entities[i], names[i], false);
                RemoveEntity(entities[i]);
            }
        }
    }

}	// NightSnack

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
#include "Systems/Graphics.h"

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

	// Create 1000 entities for stress test - Combination 1;
	void EntityFactory::StressTestCreate(LayerType layer, std::string const& prefabName, int seed, Vec2 const& scale) {

		// seed for rand;
		srand(seed * static_cast<unsigned>(glfwGetTime()));
		int Hvp = m_coord.GetWindowHeight();
		int Wvp = m_coord.GetWindowWidth();

		Transform t{ Vec3{}, {}, scale, layer };

		for (int i{}; i < 1000; ++i) {

			// Random position within viewport;
			t.pos = { static_cast<float>(rand() % Wvp - Wvp / 2),
			static_cast<float>(rand() % Hvp - Hvp / 2), 1.f };

			Entity e2 = Create(prefabName, t);

			// Randomized vector for Force direction;
			Vec2 v = { static_cast<float>(rand() % Wvp - Wvp / 2),
			static_cast<float>(rand() % Hvp - Hvp / 2) };

			Force f; 
			RigidBody& r = m_coord.ModifyComponent<RigidBody>(e2);
			if (v.x != 0.f || v.y != 0.f) {
				f.direction = v.normalized();
				f.magnitude = 25.f;
				f.lifetime = 1000000.f;
				f.isActive = true;
				
				r.force = f;
			}
			r.useGravity = false;
		}
		for (Entity e : m_coord.GetLivingEntities()) {
			m_coord.BufferEntitySignature(e);
		}
	}

	void EntityFactory::RemoveEntity(Entity e) {
		m_toBeDestroyed.insert(e);
	}

	void EntityFactory::StressTestEnd() {

		UnLoadLevel();

		LoadLevel("../assets/GameData/DefaultLevel.json");
	}

	void EntityFactory::InstancedTestCreate(LayerType layer, std::string const& prefabName, int seed, Vec2 const& scale) {

		// seed for rand;
		srand(seed * static_cast<unsigned>(glfwGetTime()));
		int Hvp = m_coord.GetWindowHeight();
		int Wvp = m_coord.GetWindowWidth();

		Transform t{ Vec3{}, {}, scale, layer };

		for (int i{}; i < 7000; ++i) {

			// Random position within viewport;
			t.pos = { static_cast<float>(rand() % Wvp - Wvp / 2),
			static_cast<float>(rand() % Hvp - Hvp / 2), 1.f };

			Entity e2 = Create(prefabName, t);
		}
		for (Entity e : m_coord.GetLivingEntities()) {
			m_coord.BufferEntitySignature(e);
		}
	}

	void EntityFactory::PrintPrefabNames() const {

		std::cout << "Prefabs: ";
		for (auto& [name, data] : GetArchetypes().items()) {
			std::cout << name << " ";
		}
		std::cout << '\n';
	}

	std::vector<std::string> const& EntityFactory::GetPrefabNames() const {
		std::vector<std::string> names;
		for (auto& [name, data] : GetArchetypes().items()) {
			names.emplace_back(name);
		}
		return names;
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
}	// NightSnack

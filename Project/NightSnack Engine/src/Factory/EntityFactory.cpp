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
	void EntityFactory::Update(float dt, Coordinator& coord) {

		(void)coord;
		(void)dt;

	}

	Entity EntityFactory::Create(std::string const& name) {
		Entity e = m_prefabManager.CreateFromPrefab(name, m_archetypes);
		if (e == INVALID_ENTITY) { return e; }

		// Make spawned entity be at mouse position;
		Transform trans{};
		trans.pos.x = m_coord.GetSystem<InputSystem>()->GetMousePosX();
		trans.pos.y = m_coord.GetSystem<InputSystem>()->GetMousePosY();

		m_coord.ModifyComponent<Transform>(e).pos = trans.pos;

		return e;
	}

	// For creating entities at specific locations, scaling etc...
	Entity EntityFactory::Create(std::string const& name, Transform trans) {
		Entity e = m_prefabManager.CreateFromPrefab(name, m_archetypes);
		if (e == INVALID_ENTITY) { return e; }

		m_coord.ModifyComponent<Transform>(e) = trans;

		return e;
	}

	// Clone an entity prefab at specific locations, scaling etc...
	Entity EntityFactory::Clone(Entity e, Transform trans) {
		std::string name = entityToPrefab[e];

		Entity clone = m_prefabManager.CreateFromPrefab(name, m_archetypes);
		if (clone == INVALID_ENTITY) { return clone; }

		m_coord.ModifyComponent<Transform>(e) = trans;

		return clone;
	}

	void EntityFactory::DoubleEntities() {
		// seed for rand;
		srand(69);
		// current number of live entities;
		size_t currNum = m_coord.GetLivingEntities().size();
		int Hvp = m_coord.GetWindowHeight();
		int Wvp = m_coord.GetWindowWidth();

		std::vector<std::string> prefabs{ "Square", "Circle", "Player" };

		// double the number of living entities with random transforms;
		for (size_t i{}; i < currNum && (currNum + i) < MAX_ENTITIES; ++i) {
			Vec3 pos = { static_cast<float>(rand() % Wvp - Wvp / 2),
			static_cast<float>(rand() % Hvp - Hvp / 2) };
			float rot = { static_cast<float>(rand() % 360) };
			Vec2 scale = { static_cast<float>(rand() % 50 + 15), static_cast<float>(rand() % 50 + 15) };

			Transform trans{ pos, rot, scale };

			// create from a random prefab;
			Create(prefabs[i % 3], trans);
		}
	}

	void EntityFactory::Remove(Entity e) {
		m_toBeDestroyed.insert(e);
	}

	void EntityFactory::HalfEntities() {
		std::set<Entity> entities = m_coord.GetLivingEntities();

		// count only eligible entities (skip default scene)
		size_t eligibleCount = 0;
		for (Entity e : entities) {
			// entities 0-10 are the default scene, which we do not want to remove for testing purposes...
			if (e >= 10) ++eligibleCount;
		}

		size_t toDestroy = eligibleCount / 2;
		size_t destroyed = 0;

		for (Entity e : entities) {
			if (e < 10) continue; // skip default scene

			Remove(e);
			if (++destroyed >= toDestroy) {
				break;
			}
		}
	}

	std::vector<std::string> const& EntityFactory::GetPrefabNames() {
		return std::vector<std::string>{};
	}

}	// NightSnack

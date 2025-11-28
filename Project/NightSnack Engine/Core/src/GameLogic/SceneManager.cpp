#include "SceneManager.h"

namespace NightSnack::Scene {

	// Take snapshot of current world
	void SceneManager::CacheLevel() {

		std::set<Entity> entities = m_coord.GetLivingEntities();
		if (entities.empty()) { return; }

		m_levelCache.clear();
		m_levelCache.reserve(entities.size());

		for (Entity e : entities) {
			m_levelCache.emplace_back(std::make_pair(
				m_coord.GetSystem<EntityFactory>()->CacheEntityComponents(e),
				m_coord.GetComponent<Transform>(e)));
		}
	}

	// Restore scene to cached level
	void SceneManager::RestoreLevel() {

		if (m_levelCache.empty()) { return; }

		auto const& factory = m_coord.GetSystem<EntityFactory>();
		factory->UnloadLevel();

		for (auto const& unit : m_levelCache) {
			factory->Create(unit.first, unit.second);
		}
	}

}	// NightSnack
#include "SceneManager.h"

namespace NightSnack::Scene {

	void SceneManager::CreateOverlayMenu() {

		if (m_overlayMenu.empty()) { return; }
		if (overlayOn) { return; }

		// Create Entities for overlay menu
		auto const& factory = m_coord.GetSystem<EntityFactory>();

		for (auto const& unit : m_overlayMenu) {
			Entity e = factory->Create(unit.first, unit.second);
			m_overlayEntites.push_back(e);
		}
		overlayOn = true;
	}

	void SceneManager::RemoveOverlayMenu() {

		if (!overlayOn) { return; }

		// Set overlay Entities to be destroyed at the end of frame
		auto const& factory = m_coord.GetSystem<EntityFactory>();

		for (Entity e : m_overlayEntites) {
			factory->RemoveEntity(e);
		}
		m_overlayEntites.clear();
		overlayOn = false;
	}

	void SceneManager::SetOverlayMenu(std::string const& filename) {

	}

	// Take snapshot of current world
	void SceneManager::CacheLevel() {

		std::set<Entity> entities = m_coord.GetLivingEntities();
		if (entities.empty()) { return; }

		m_levelCache.clear();
		m_levelCache.reserve(entities.size());

		for (Entity e : entities) {
			m_levelCache.emplace_back(std::make_pair(
				m_coord.GetSystem<EntityFactory>()->GetEntityPrefabName(e),
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
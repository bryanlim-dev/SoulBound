#include "LevelManager.h"

namespace NightSnack::Scene {

	void LevelManager::LoadLevel(std::string const& filename) {

		size_t levelKey = 0;
		try {
			LOAD_RESULT result = m_coord.GetSystem<ResourceManager>()->LoadJson(filename, levelKey);
			if (result == LOAD_FAIL_BADPATH) {
				throw std::runtime_error("Failed to open archetypes file due to bad file path: " + filename);
			}
		}
		catch (json::parse_error& e) {
			throw std::runtime_error("JSON parse error at byte " + std::to_string(e.byte) + ": " + e.what());
		}

		m_currentLevel = m_coord.GetSystem<ResourceManager>()->GetJsonRaw(levelKey);

		for (auto const& item : m_currentLevel["Entities"]) {
			m_coord.GetSystem<EntityFactory>()->Create(item["prefab"], item["Transform"]);
		}
		for (Entity e : m_coord.GetLivingEntities()) {
			m_coord.BufferEntitySignature(e);
		}
	}

	void LevelManager::UnloadLevel() {

	}

}	// NightSnack
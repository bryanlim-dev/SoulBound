#include "SceneManager.h"

namespace NightSnack::Scene {
	
	void SceneManager::LoadLevel(std::string const& levelname) {

		size_t key = m_levelKeys[levelname];
		json const& level = m_coord.GetSystem<ResourceManager>()->GetJsonRaw(key);

		m_previousLevelKey = m_currentLevelKey;
		m_currentLevelKey = key;

		// Create all Entities
		m_coord.GetSystem<EntityFactory>()->CreateLevelEntities(level);
		// Play Background Music
		auto audio = m_coord.GetSystem<AudioSystem>();
		if (level.contains("BGM")) audio->PlayBGM(level["BGM"]);
		else audio->StopBGM();

		gameLoaded = true;
	}

	bool SceneManager::SaveLevel(std::string const& levelname, bool overwrite) {
		// Output file path
		std::filesystem::path outPath = std::filesystem::path(LEVELS_PATH) / (levelname + ".json");
		std::filesystem::path primePath = std::filesystem::path(PRIME_LEVELS_PATH) / (levelname + ".json");

		// Check if a prefab already exists with the same name
		if (std::filesystem::exists(primePath) && !overwrite) {
			std::cout << "There already exists a level named '" << levelname << "' at '" << "GameData/Levels" << '\n';
			return false;
		}

		// Output json object
		json level;
		level["Entities"] = json::array();

		// Save Level data;
		level["Entities"] = m_coord.GetSystem<EntityFactory>()->SaveLevelEntities();
		level["BGM"] = m_coord.GetSystem<AudioSystem>()->GetBGM();
		level["Name"] = levelname;

		// Write file -------------------------------------------------------
		std::ofstream pfs(primePath);
		if (!pfs.is_open()) {
			std::cerr << "Failed to write level file: " << primePath << "\n";
			return false;
		}
		pfs << std::setw(4) << level;
		pfs.close();

		std::ofstream ofs(outPath);
		if (!ofs.is_open()) {
			std::cerr << "Failed to write level file: " << outPath << '\n';
			return false;
		}
		ofs << std::setw(4) << level;
		ofs.close();

		// Update ResourceManager -------------------------------------------
		auto resource = m_coord.GetSystem<ResourceManager>();
		if (m_levelKeys.find(levelname) != m_levelKeys.end()) {
			// Level exists and was overwritten
			resource->UpdateJson(m_levelKeys[levelname], level);
			std::cerr << "[SceneManager] Updated existing level\n";
		}
		else {
			// New Level has been saved
			std::string loadPath = "../../../../assets/GameData/Levels/" + levelname + ".json";
			resource->LoadJson(loadPath, m_levelKeys[levelname]);
			std::cerr << "[SceneManager] Saved new level\n";
		}
		std::cout << "Saved level to: " << outPath << "\n";
		return true;
	}

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
				m_coord.GetSystem<EntityFactory>()->CacheEntityComponents(e),
				m_coord.GetComponent<Transform>(e)));
		}
	}

	// Restore scene to cached level
	void SceneManager::RestoreLevel() {

		if (m_levelCache.empty()) { return; }

		// Restore all entities to original
		auto const& factory = m_coord.GetSystem<EntityFactory>();
		factory->DestroyLevelEntities();
		for (auto const& unit : m_levelCache) {
			factory->Create(unit.first, unit.second);
		}
		
		// Restart audio
		auto const& audio = m_coord.GetSystem<AudioSystem>();
		std::string bgmkey = audio->GetBGM();
		audio->StopBGM();
		audio->PlayBGM(bgmkey);

		auto const& script = m_coord.GetSystem<Scripting::ScriptManager>();
		script->RefreshScripts();
	}

	// Reads all level files in assets folder
	void SceneManager::ReloadLevels() {

		// Clear level keys.
		m_levelKeys.clear();

		std::string fullpath = LEVELS_PATH;

		// Iterate through entries in directory.
		for (auto const& entry : std::filesystem::directory_iterator(fullpath))
		{
			size_t key = 0;

			std::string entrypath = "GameData/Levels/" + entry.path().filename().string();
			//std::cout << "[Scene Manager] Attempting to read: " << entrypath << '\n';

			LOAD_RESULT result = m_coord.GetSystem<ResourceManager>()->LoadJson(entrypath, key);
			//std::cout << "[Scene Manager] entry.path: " << entry.path() << '\n';
			if (result == LOAD_FAIL_BADPATH) {
				std::cerr << "Failed to read json file: " << entry.path() << '\n';
				continue;
			}
			m_levelKeys[entry.path().stem().string()] = key;
		}
	}

}	// NightSnack
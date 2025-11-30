/******************************************************************************
 * File:        SceneManager.cpp
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan (Primary, 100%)
 *
 * Description:
 * This file defines functions that assist in scene managment such as level
 * loading/saving, level caching when Play/Stop in Editor and GameUI interactions
 * like button clicks.
 * 
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
#include "SceneManager.h"

// Private helper function that checks mouse and button collision
static bool ButtonCollisionCheck(float mx, float my, NightSnack::Transform const& t, NightSnack::Collider const& c) {
	if (c.shape == NightSnack::ShapeType::Circle) {
		float distsq = mx * mx + my + my;
		if (c.radius * c.radius <= distsq) { return true; }
	}
	else {
		float halfWidth = c.width * 0.5f;
		float halfHeight = c.height * 0.5f;

		float left = t.pos.x - halfWidth;
		float right = t.pos.x + halfWidth;
		float bottom = t.pos.y - halfHeight;
		float top = t.pos.y + halfHeight;

		return (mx >= left && mx <= right && my >= bottom && my <= top);
	}
	return false;
}

namespace NightSnack::Scene {

	// Main update function to be called in core loop
	void SceneManager::Update() {
		// If somehow no level is loaded, return to default scene
		if (!m_gameLoaded) LoadLevel("MainMenu");

		if (m_restart) {
			RestartLevel();
			m_restart = false;
		}

		// In the event of mouseclick check collision of all buttons
		//if (m_mouseClicked) {
		//	m_mouseClicked = false;
		//	if (m_coord.IsPaused()) return;
		//	CheckButtonCollisions();
		//}
	}

	// Register all button actions and stop buttons into m_buttons
	void SceneManager::RegisterButtons() {
		Button loadButton(ButtonAction::LOADLEVEL, [](Coordinator& c, std::string const& s) {
			if (s.empty()) { return; }
			c.GetSystem<Scene::SceneManager>()->LoadLevel(s);
		});
		m_buttons.emplace(ButtonAction::LOADLEVEL, loadButton);

		Button NoAction(ButtonAction::NOACTION, [](Coordinator& c, std::string const& s) {
			(void)c;
			if (s.empty()) { return; }
		});
		m_buttons.emplace(ButtonAction::NOACTION, NoAction);

		//Button menuButton(ButtonAction::OPENMENU, [](Coordinator& c, std::string const& s) {
		//	if (s.empty()) { return; }
		//	c.GetSystem<Scene::SceneManager>()->OpenMenu(s);
		//	});
		//m_buttons[ButtonAction::OPENMENU] = menuButton;
	}

	// Check collision of all entities with button component,
	// Called when Mouse click event occurs.
	void SceneManager::CheckButtonCollisions() {
		float mx = m_coord.GetSystem<InputSystem>()->GetMouseWorldPosX();
		float my = m_coord.GetSystem<InputSystem>()->GetMouseWorldPosY();

		for (Entity e : m_entities) {
			// Get button info
			ButtonInfo const& b = m_coord.GetComponent<ButtonInfo>(e);
			if (b.type == INVALID_BUTTON) { continue; }

			Transform const& t = m_coord.GetComponent<Transform>(e);
			Collider const& c = m_coord.GetComponent<Collider>(e);

			// Check collision with mouse
			if (!ButtonCollisionCheck(mx, my, t, c)) { continue; }

			std::unordered_map<ButtonAction, Button>::iterator it = m_buttons.find(b.action);
			if (it != m_buttons.end()) {
				// Play audio if available
				m_coord.GetSystem<AudioSystem>()->PlayEntityAudio(e);
				// Invoke button Callable;
				it->second(m_coord, b.target);
				// Only interact with 1 button
				return;	
			}
			else {
				std::cerr << "[SceneManager] Button action of Entity: " << e << "not registered\n";
			}
		}
	}
	
	// Creates all entities required for a level through the entity factory
	// and sets up BGM for level, refreshes script manager.
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

		// Refresh Scripts for loaded level
		auto const& script = m_coord.GetSystem<Scripting::ScriptManager>();
		script->RefreshScripts();

		m_gameLoaded = true;
	}

	void SceneManager::RestartLevel() {

		json const& level = m_coord.GetSystem<ResourceManager>()->GetJsonRaw(m_currentLevelKey);
		// Create all Entities
		m_coord.GetSystem<EntityFactory>()->CreateLevelEntities(level);
		// Play Background Music
		auto audio = m_coord.GetSystem<AudioSystem>();
		if (level.contains("BGM")) audio->PlayBGM(level["BGM"]);
		else audio->StopBGM();

		// Refresh Scripts for loaded level
		auto const& script = m_coord.GetSystem<Scripting::ScriptManager>();
		script->RefreshScripts();

		std::cerr << "[SceneManager] Level restarted\n";

		m_gameLoaded = true;
	}

	// Saves information required for levels, including entity information, BGM and name
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

	// Currently Overlay Menus are unused and planned to be implemented in future milestones.
	// Spawn a menu that overlays current game
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
	// Destroy entities of overlay menu
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

	//void SceneManager::SetOverlayMenu(std::string const& filename) {

	//}

	// Take snapshot of current world -> cache when Play simulation in Editor
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

	// Restore scene to cached level -> restore after Stopping simulation in Editor
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

		std::cerr << "[SceneManager] Level restored\n";
	}

	// Reads all level files in assets folder -> "./assets/GameData/Levels/"
	void SceneManager::ReloadLevels() {

		// Clear level keys.
		for (auto [n, key] : m_levelKeys) {
			m_coord.GetSystem<ResourceManager>()->UnloadJson(key);
		}
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
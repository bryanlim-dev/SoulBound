/******************************************************************************
 * File:        SceneManager.cpp
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan (Primary, 60%)
 *				Bryan Lim Jun Jie (Secondary, 40%)
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
		if (!m_gameLoaded) LoadLevel("DefaultLevel");

		if (m_restart) {
			RestartLevel();
			CloseMenu();
			m_restart = false;
		}

		if (m_scheduledLevelLoad) {
			m_levelLoadTimer -= (1.0f / 60.0f); // to use actual deltaTime if available

			if (m_levelLoadTimer <= 0.0f) {
				// Load the new level
				LoadLevel(m_scheduledLevelName);

				// Fade in the new BGM
				m_coord.GetSystem<AudioSystem>()->FadeToVolume(0.2f, 1.0f); // 0.2f is bgm volume

				// Reset scheduled load
				m_scheduledLevelLoad = false;
				m_scheduledLevelName = "";
			}
		}

		if (m_scheduledExit) {
			m_exitTimer -= (1.0f / 60.0f);

			if (m_exitTimer <= 0.0f) {
				std::cerr << "[SceneManager] Exiting application\n";
				exit(0);
			}
		}
	}

	// Register all button actions and stop buttons into m_buttons
	void SceneManager::RegisterButtons() {
		Button loadButton(ButtonAction::LOADLEVEL, [](Coordinator& c, std::string const& s) {
			if (s.empty()) { return; }
			//c.GetSystem<Scene::SceneManager>()->LoadLevel(s);
			// 
			// loads level with fade, 1.5s fade out, 1.0s fade in
			c.GetSystem<Scene::SceneManager>()->LoadLevelWithFade(s, 1.5f, 1.0f);
		});
		m_buttons.emplace(ButtonAction::LOADLEVEL, loadButton);

		Button NoAction(ButtonAction::NOACTION, [](Coordinator& c, std::string const& s) {
			(void)c;
			if (s.empty()) { return; }
		});
		m_buttons.emplace(ButtonAction::NOACTION, NoAction);

		// open menu button
		Button menuButton(ButtonAction::OPENMENU, [](Coordinator& c, std::string const& s) {
			if (s.empty()) { return; }
			c.GetSystem<Scene::SceneManager>()->OpenMenu(s);
			});
		m_buttons.emplace(ButtonAction::OPENMENU, menuButton);

		// close menu button
		Button closeButton(ButtonAction::CLOSEMENU, [](Coordinator& c, std::string const& s) {
			(void)s; // Menu name not needed for close
			c.GetSystem<Scene::SceneManager>()->CloseMenu();
			});
		m_buttons.emplace(ButtonAction::CLOSEMENU, closeButton);

		// exit button
		Button exitButton(ButtonAction::EXIT, [](Coordinator& c, std::string const& s) {
			(void)s;
			// Fade out audio and schedule exit
			auto audio = c.GetSystem<AudioSystem>();
			audio->FadeToVolume(0.0f, 0.5f); // Fade out over 0.5s

			c.GetSystem<Scene::SceneManager>()->ScheduleExit(0.5f); // Exit after fade
			});
		m_buttons.emplace(ButtonAction::EXIT, exitButton);
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

			// only check menu buttons
			if (m_menuOpen) {
				// Check if this entity is in the menu entities list
				auto it = std::find(m_menuEntities.begin(), m_menuEntities.end(), e);

				// skip if not a menu button
				if (it == m_menuEntities.end()) { continue; }
			}

			Transform const& t = m_coord.GetComponent<Transform>(e);
			Collider const& c = m_coord.GetComponent<Collider>(e);

			// Check collision with mouse
			if (!ButtonCollisionCheck(mx, my, t, c)) {
				continue;
			}

			std::unordered_map<ButtonAction, Button>::iterator it = m_buttons.find(b.action);
			if (it != m_buttons.end()) {
				// Play audio if available
				m_coord.GetSystem<AudioSystem>()->PlayEntityAudio(e);
				// Invoke button Callable
				it->second(m_coord, b.target);
				// Only interact with 1 button
				return;
			}
			else {
				std::cerr << "[SceneManager] Button action of Entity: " << e << " not registered\n";
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

	void SceneManager::LoadLevelWithFade(std::string const& levelname, float fadeOutDuration, float fadeInDuration) {
		// Start fade out
		m_coord.GetSystem<AudioSystem>()->FadeToVolume(0.0f, fadeOutDuration);

		// Schedule level load
		m_scheduledLevelLoad = true;
		m_scheduledLevelName = levelname;
		m_levelLoadTimer = fadeOutDuration; // Wait for fade out to complete
	}

	void SceneManager::ScheduleExit(float delay) {
		m_scheduledExit = true;
		m_exitTimer = delay;
		std::cerr << "[SceneManager] Exit scheduled in " << delay << " seconds\n";
	}

	void SceneManager::RestartLevel() {

		//// close all menus
		//if (m_menuOpen) {
		//	std::cerr << "[SceneManager] Closing menu before restart\n";
		//	CloseMenu();
		//}

		json const& level = m_coord.GetSystem<ResourceManager>()->GetJsonRaw(m_currentLevelKey);
		// Create all Entities
		m_coord.GetSystem<EntityFactory>()->CreateLevelEntities(level);
		// Play Background Music
		auto audio = m_coord.GetSystem<AudioSystem>();
		if (level.contains("BGM")) audio->PlayBGM(level["BGM"]);
		else audio->StopBGM();

		// Refresh Scripts for loaded levelbutton
		auto const& script = m_coord.GetSystem<Scripting::ScriptManager>();
		script->RefreshScripts();

		std::cerr << "[SceneManager] Level restarted\n";

		m_gameLoaded = true;
	}

	// Saves information required for levels, including entity information, BGM and name
	bool SceneManager::SaveLevel(std::string const& levelname, bool overwrite) {
		// Output file path
		std::filesystem::path outPath = RESOURCE_PATH + ("GameData/Levels/" + (levelname + ".json"));

		//std::filesystem::path outPath = std::filesystem::path(LEVELS_PATH) / (levelname + ".json");
		//std::filesystem::path primePath = std::filesystem::path(PRIME_LEVELS_PATH) / (levelname + ".json");

		// Check if a level already exists with the same name
		if (std::filesystem::exists(outPath) && !overwrite) {
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
		std::ofstream pfs(outPath);
		if (!pfs.is_open()) {
			std::cerr << "Failed to write level file: " << outPath << "\n";
			return false;
		}
		pfs << std::setw(4) << level;
		pfs.close();

		// Update ResourceManager -------------------------------------------
		auto resource = m_coord.GetSystem<ResourceManager>();
		if (m_levelKeys.find(levelname) != m_levelKeys.end()) {
			// Level exists and was overwritten
			resource->UpdateJson(m_levelKeys[levelname], level);
			std::cerr << "[SceneManager] Updated existing level\n";
		}
		else {
			// New Level has been saved
			std::string loadPath = "GameData/Levels/" + levelname + ".json";
			resource->LoadJson(loadPath, m_levelKeys[levelname]);
			std::cerr << "[SceneManager] Saved new level\n";
		}
		std::cout << "Saved level to: " << outPath << "\n";
		return true;
	}

	//// Currently Overlay Menus are unused and planned to be implemented in future milestones.
	//// Spawn a menu that overlays current game
	//void SceneManager::CreateOverlayMenu() {

	//	if (m_overlayMenu.empty()) { return; }
	//	if (overlayOn) { return; }

	//	// Create Entities for overlay menu
	//	auto const& factory = m_coord.GetSystem<EntityFactory>();

	//	for (auto const& unit : m_overlayMenu) {
	//		Entity e = factory->Create(unit.first, unit.second);
	//		m_overlayEntites.push_back(e);
	//	}
	//	overlayOn = true;
	//}
	//// Destroy entities of overlay menu
	//void SceneManager::RemoveOverlayMenu() {

	//	if (!overlayOn) { return; }

	//	// Set overlay Entities to be destroyed at the end of frame
	//	auto const& factory = m_coord.GetSystem<EntityFactory>();

	//	for (Entity e : m_overlayEntites) {
	//		factory->RemoveEntity(e);
	//	}
	//	m_overlayEntites.clear();
	//	overlayOn = false;
	//}

	//void SceneManager::SetOverlayMenu(std::string const& filename) {

	//}

	// Take snapshot of current world -> cache when Play simulation in Editor
	void SceneManager::CacheLevel() {

		std::set<Entity> entities = m_coord.GetLivingEntities();
		if (entities.empty()) { return; }

		m_levelCache["Entities"] = m_coord.GetSystem<EntityFactory>()->CacheLevelEntities();
	}

	// Restore scene to cached level -> restore after Stopping simulation in Editor
	void SceneManager::RestoreLevel() {

		if (m_levelCache.empty()) { return; }
		// Reset menus
		m_menuEntities.clear();
		m_menuOpen = false;
		m_currentMenu.clear();

		// Restore all entities to original
		auto const& factory = m_coord.GetSystem<EntityFactory>();
		factory->DestroyLevelEntities();
		factory->CreateLevelEntities(m_levelCache);
		
		// Restart audio
		auto const& audio = m_coord.GetSystem<AudioSystem>();
		std::string bgmkey = audio->GetBGM();
		audio->StopBGM();
		audio->PlayBGM(bgmkey);

		auto const& script = m_coord.GetSystem<Scripting::ScriptManager>();
		script->RefreshScripts(); //	

		std::cerr << "[SceneManager] Level restored\n";
	}

	// Reads all level files in assets folder -> "./assets/GameData/Levels/"
	void SceneManager::ReloadLevels() {

		// Clear level keys.
		for (auto [n, key] : m_levelKeys) {
			m_coord.GetSystem<ResourceManager>()->UnloadJson(key);
		}
		m_levelKeys.clear();

		std::string fullpath = RESOURCE_PATH + std::string{ "GameData/Levels/" };

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

	void SceneManager::OpenMenu(std::string const& menuname) {
		if (m_menuOpen) {
			std::cerr << "[SceneManager] Menu already open, closing first\n";
			CloseMenu();
		}
		std::cerr << "[SceneManager] Opening menu: " << menuname << "\n";

		// Build the menu file path
		std::string menuPath = "GameData/Menus/" + menuname + ".json";

		// Check if menu is already loaded, if not load it
		size_t menuKey = 0;
		auto it = m_menuKeys.find(menuname);

		if (it != m_menuKeys.end()) {
			menuKey = it->second;
		}
		else {
			LOAD_RESULT result = m_coord.GetSystem<ResourceManager>()->LoadJson(menuPath, menuKey);

			if (result == LOAD_FAIL_BADPATH) {
				std::cerr << "[SceneManager] Failed to load menu: " << menuPath << '\n';
				return;
			}

			m_menuKeys[menuname] = menuKey;
		}

		json const& menuData = m_coord.GetSystem<ResourceManager>()->GetJsonRaw(menuKey);

		// get camera position and scale to center
		Entity mainCam = m_coord.GetSystem<GraphicsSystem>()->GetCamera();
		auto& camTransform = m_coord.GetComponent<Transform>(mainCam);
		auto& camera = m_coord.GetComponent<Camera>(mainCam);

		float camX = camTransform.pos.x;
		float camY = camTransform.pos.y;
		float zoom = camera.zoom;

		std::cerr << "[SceneManager] Centering menu on camera at (" << camX << ", " << camY
			<< ") with zoom: " << zoom << "\n";

		// Create menu entities
		auto const& factory = m_coord.GetSystem<EntityFactory>();

		if (menuData.contains("Entities")) {
			for (auto const& entityData : menuData["Entities"]) {
				Entity e = 0;
				std::cerr << "[Open Menu] Check 1\n";

				if (entityData.contains("prefab")) {
					std::string prefabName = entityData["prefab"];
					Transform transform;

					// Parse transform data if available
					if (entityData.contains("Transform")) {
						transform = entityData["Transform"];

						// Divide by zoom to make menu appear same size on screen
						transform.scale.x /= zoom;
						transform.scale.y /= zoom;

						// offset menu position by camera position
						transform.pos.x += camX;
						transform.pos.y += camY;
						transform.pos.z = 1.5f;

						std::cerr << "  Prefab: " << prefabName
							<< " positioned at (" << transform.pos.x << ", " << transform.pos.y << ", " << transform.pos.z << ")"
							<< " scaled to (" << transform.scale.x << ", " << transform.scale.y << ")\n";
					}

					std::cerr << "[Open Menu] Check 2 - Creating: " << prefabName << "\n";

					e = factory->Create(prefabName, transform);
				}

				if (e != INVALID_ENTITY) {
					m_menuEntities.push_back(e);

					// Check if entity is a button and add to system
					if (m_coord.HasComponent<ButtonInfo>(e) &&
						m_coord.HasComponent<Transform>(e) &&
						m_coord.HasComponent<Collider>(e)) {
						//m_entities.insert(e);
						std::cerr << "[SceneManager] Added button entity " << e << " to system\n";
					}
				}
			}
		}

		m_menuOpen = true;
		m_currentMenu = menuname;

		// only pause for in game menus
		if (menuname == "PauseMenu") {
			m_coord.Pause();
			std::cerr << "[SceneManager] Game paused\n";
		}

		std::cerr << "[SceneManager] Opened menu: " << menuname
			<< " with " << m_menuEntities.size() << " entities\n";
	}

	void SceneManager::CloseMenu() {
		if (!m_menuOpen) {
			return;
		}

		//std::cerr << "[SceneManager] Closing menu: " << m_currentMenu << '\n';

		// destroy all menu entities
		auto const& factory = m_coord.GetSystem<EntityFactory>();

		for (Entity e : m_menuEntities) {

			factory->RemoveEntity(e);
		}

		// unpause only if it was paused
		if (m_coord.IsPaused()) {
			m_coord.Unpause();
			//std::cerr << "[SceneManager] Game unpaused\n";
		}

		m_menuEntities.clear();
		m_menuOpen = false;
		m_currentMenu.clear();

		//std::cerr << "[SceneManager] Closed menu\n";
	}

}	// NightSnack
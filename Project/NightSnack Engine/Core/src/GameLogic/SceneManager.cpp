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
#include "Systems/Physics/Physics.h"
#include "Systems/PlayerController.h"
#include "Graphics/Camera.h"

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
		if (!m_gameLoaded) LoadLevel("Level1");

		if (m_restart) {
			RestartLevel();
			m_restart = false;
		}


		// Check if player is colliding with a CamTrigger -> if so, interpolate camera to new position;
		m_player = m_coord.GetSystem<PlayerControllerSystem>()->GetPlayer();

		if (m_player != INVALID_ENTITY) {
			auto const& physics = m_coord.GetSystem<PhysicsSystem>();
			for (auto& [a, b] : m_camFrames) {
				if (a == m_camIndex) continue;
				if (!m_coord.HasComponent<Collider>(a) || !m_coord.GetComponent<Collider>(a).colliding) continue;
				if (physics->EntitiesAreColliding(a, m_player)) {
					m_coord.GetSystem<CameraSystem>()->UpdateCamBounds(b);
					m_camIndex = a;
					break;
				}
			}
		}
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

	void SceneManager::ReadCameraKeyframes(json const& j) {
		m_camFrames.clear();
		auto const& factory = m_coord.GetSystem<EntityFactory>();

		for (auto const& item : j) {
			json c;
			// Camera data to be stored
			if (item.contains("minX")) c["minX"] = item["minX"];
			if (item.contains("maxX")) c["maxX"] = item["maxX"];

			if (item.contains("minY")) c["minY"] = item["minY"];
			if (item.contains("maxY")) c["maxY"] = item["maxY"];

			if (item.contains("zoom")) c["zoom"] = item["zoom"];

			// Create trigger collider entity
			Vec2 scale;
			if (!item.contains("scale")) scale = Vec2(10.f);
			else scale = item["scale"];

			Entity e = factory->Create();
			Transform t = {
				item["pos"], 0.f, Vec2{100.f, 100.f},
				Vec3{}, Vec2{},
				LayerType::GAMELAYER,
				false,
				true,
				"CamTrigger"	// Don't change name; else also edit EntityFactory::CacheLevelEntities;
			};
			m_coord.ModifyComponent<Transform>(e) = t;

			Collider col = {
				ShapeType::AABB, ColliderType::Trigger, true,
				scale.x, scale.y, scale.x / 2.f,
				false, false
			};
			m_coord.AddComponent<Collider>(e, col);

			// Invisible renderer component
			m_coord.AddComponent<Renderable>(e, Renderable{});
			m_coord.AddComponent<Color>(e, Color{});    // zero alpha
			// Required for Physics system
			m_coord.AddComponent<RigidBody>(e, RigidBody{});

			m_camFrames.insert({e, c});
			
			//// Create Triggers colliders for Cam Data
			//for (auto const& trig : item["triggers"]) {
			//	if (!trig.contains("pos")) continue;
			//	Vec2 scale;
			//	if (!trig.contains("scale")) scale = Vec2(10.f);
			//	else scale = trig["scale"];
			//	
			//	Entity e = factory->Create();
			//	Transform t = {
			//		trig["pos"], 0.f, scale,
			//		Vec3{}, Vec2{},
			//		LayerType::GAMELAYER,
			//		false,
			//		true,
			//		"CamTrigger"
			//	};
			//	m_coord.ModifyComponent<Transform>(e) = t;
			//	Collider col = {
			//		ShapeType::AABB, ColliderType::Trigger, true,
			//		scale.x, scale.y, scale.x / 2.f,
			//		false, false
			//	};
			//	m_coord.AddComponent<Collider>(e, col);
			//	// Invisible renderer component
			//	m_coord.AddComponent<Renderable>(e, Renderable{});
			//	m_coord.AddComponent<Color>(e, Color{});    // zero alpha
			//	// Required for Physics system
			//	m_coord.AddComponent<RigidBody>(e, RigidBody{});
			//	// entity mapped to cam data
			//	m_camFrames.push_back({ e, c });	
			//}
		}
	}
	json SceneManager::WriteCameraKeyframes() {
		json keyframes = json::array();

		for (auto& [a, b] : m_camFrames) {
			json j;
			
			j["minX"] = b["minX"]; j["maxX"] = b["maxX"];
			j["minY"] = b["minY"]; j["maxY"] = b["maxY"];
			j["zoom"] = b["zoom"];

			Transform const& t = m_coord.GetComponent<Transform>(a);
			Collider const& c = m_coord.GetComponent<Collider>(a);

			j["pos"] = t.pos;
			j["scale"] = Vec2(c.width, c.height);

			keyframes.push_back(j);

			//m_coord.DestroyEntity(a);
		}

		return keyframes;
	}
	
	// Creates all entities required for a level through the entity factory
	// and sets up BGM for level, refreshes script manager.
	void SceneManager::LoadLevel(std::string const& levelname) {

		size_t key = m_levelKeys[levelname];
		json const& level = m_coord.GetSystem<ResourceManager>()->GetJsonRaw(key);

		m_previousLevelKey = m_currentLevelKey;
		m_currentLevelKey = key;

		// Create all Entities
		auto const& factory = m_coord.GetSystem<EntityFactory>();
		factory->DestroyLevelEntities();

		// Camera
		ReadCameraKeyframes(level["MainCamera"]["KeyFrames"]);
		CreateSceneCamera(level);
		
		factory->CreateLevelEntities(level);

		// Play Background Music
		auto audio = m_coord.GetSystem<AudioSystem>();
		if (level.contains("BGM")) audio->PlayBGM(level["BGM"]);
		else audio->StopBGM();

		// Refresh Scripts for loaded level
		auto const& script = m_coord.GetSystem<Scripting::ScriptManager>();
		script->RefreshScripts();

		m_gameLoaded = true;
	}

	void SceneManager::CreateSceneCamera(json const& j) {
		if (!j.contains("MainCamera")) return;

		Entity e = m_coord.CreateEntity();
		json c;
		c = j["MainCamera"];

		Transform t;

		Camera cam = j["MainCamera"];
		//Camera cam;

		if (!m_camFrames.empty()) {
			cam.minX = (m_camFrames.begin()->second)["minX"];
			cam.maxX = (m_camFrames.begin()->second)["maxX"];
			cam.minY = (m_camFrames.begin()->second)["minY"];
			cam.maxY = (m_camFrames.begin()->second)["maxY"];
			
			m_camIndex = m_camFrames.begin()->first;
		}
		else {
			cam.minX = cam.maxX = cam.minY = cam.maxY = 0.f;
		}
		t.pos.x = t.pos.y = 0.f;
		t.pos.z = m_camDepth;
		t.layer = LayerType::GAMEUI;

		m_coord.AddComponent<Transform>(e, t);
		m_coord.AddComponent<Hierarchy>(e, Hierarchy{});
		m_coord.AddComponent<Camera>(e, cam);

		m_sceneCam = e;
	}

	void SceneManager::RestartLevel() {

		json const& level = m_coord.GetSystem<ResourceManager>()->GetJsonRaw(m_currentLevelKey);
		auto const& factory = m_coord.GetSystem<EntityFactory>();
		factory->DestroyLevelEntities();

		// Reset Camera
		ReadCameraKeyframes(level["MainCamera"]["KeyFrames"]);
		CreateSceneCamera(level);

		// Create all Entities
		factory->CreateLevelEntities(level);

		// Play Background Music
		//auto audio = m_coord.GetSystem<AudioSystem>();
		//if (level.contains("BGM")) audio->PlayBGM(level["BGM"]);
		//else audio->StopBGM();

		// Refresh Scripts for loaded level
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

		// Save Camera data;
		level["MainCamera"] = m_coord.GetComponent<Camera>(m_sceneCam);
		level["MainCamera"]["KeyFrames"] = WriteCameraKeyframes();
		Transform const& t = m_coord.GetComponent<Transform>(m_sceneCam);
		level["MainCamera"]["posX"] = t.pos.x;
		level["MainCamera"]["posY"] = t.pos.y;
		level["MainCamera"]["posZ"] = t.pos.z;

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

		m_levelCache["MainCamera"] = m_coord.GetComponent<Camera>(m_sceneCam);
		m_levelCache["MainCamera"]["KeyFrames"] = WriteCameraKeyframes();

		Transform const& t = m_coord.GetComponent<Transform>(m_sceneCam);
		m_levelCache["MainCamera"]["posX"] = t.pos.x;
		m_levelCache["MainCamera"]["posY"] = t.pos.y;
		m_levelCache["MainCamera"]["posZ"] = t.pos.z;

		m_levelCache["Entities"] = m_coord.GetSystem<EntityFactory>()->CacheLevelEntities();
	}

	// Restore scene to cached level -> restore after Stopping simulation in Editor
	void SceneManager::RestoreLevel() {

		if (m_levelCache.empty()) { return; }

		// Restore all entities to original
		auto const& factory = m_coord.GetSystem<EntityFactory>();
		factory->DestroyLevelEntities();

		ReadCameraKeyframes(m_levelCache["MainCamera"]["KeyFrames"]);
		CreateSceneCamera(m_levelCache);
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

}	// NightSnack
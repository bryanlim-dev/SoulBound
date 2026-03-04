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
 * Copyright � 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
#include "SceneManager.h"
#include "Systems/Physics/Physics.h"
#include "Systems/PlayerController.h"
#include "Graphics/Camera.h"
#include "Systems/ParticleSystem.h"
#include "Systems/CollectibleSystem.h"

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
			CloseMenu();
			m_restart = false;
		}

		CheckButtonCollisions();

		if (m_scheduledLevelLoad) {
			m_levelLoadTimer -= (1.0f / 60.0f); // to use actual deltaTime if available

			if (m_levelLoadTimer <= 0.0f) {
				// Load the new level
				LoadLevel(m_scheduledLevelName);

				// Fade in the new BGM
				//m_coord.GetSystem<AudioSystem>()->FadeToVolume(0.2f, 1.0f); // 0.2f is bgm volume

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
			if (m_coord.GetComponent<Collider>(m_winCollider).colliding && !winScreenOn) {
				if (physics->EntitiesAreColliding(m_winCollider, m_player)) {
					CreateWinScreen();
					m_coord.GetSystem<AudioSystem>()->PlaySFX("playerWin");
					m_coord.GetSystem<CollectibleSystem>()->ToggleCollectibleIcons(false);
					winScreenOn = true;
				}
			}
		}
		m_mouseClick = false;
	}

	// Register all button actions and stop buttons into m_buttons
	void SceneManager::RegisterButtons() {
		Button loadButton(ButtonAction::LOADLEVEL, [](Coordinator& c, std::string const& s, Entity e) {
			(void)e;
			if (s.empty()) { return; }
			//c.GetSystem<Scene::SceneManager>()->LoadLevel(s);
			// 
			// loads level with fade, 1.5s fade out, 1.0s fade in
			c.Unpause();
			c.GetSystem<Scene::SceneManager>()->LoadLevelWithFade(s, 1.5f, 1.0f);
		});
		m_buttons.emplace(ButtonAction::LOADLEVEL, loadButton);

		Button NoAction(ButtonAction::NOACTION, [](Coordinator& c, std::string const& s, Entity e) {
			(void)e;
			(void)c;
			if (s.empty()) { return; }
		});
		m_buttons.emplace(ButtonAction::NOACTION, NoAction);

		// open menu button
		Button menuButton(ButtonAction::OPENMENU, [](Coordinator& c, std::string const& s, Entity e) {
			(void)e;
			if (s.empty()) { return; }
			c.GetSystem<Scene::SceneManager>()->OpenMenu(s);
			});
		m_buttons.emplace(ButtonAction::OPENMENU, menuButton);

		// close menu button
		Button closeButton(ButtonAction::CLOSEMENU, [](Coordinator& c, std::string const& s, Entity e) {
			(void)e;
			(void)s; // Menu name not needed for close
			c.GetSystem<Scene::SceneManager>()->CloseMenu();
			});
		m_buttons.emplace(ButtonAction::CLOSEMENU, closeButton);

		// toggle fullscreen button
		Button fullscreenButton(ButtonAction::TOGGLEFULLSCREEN, [](Coordinator& c, std::string const& s, Entity e) {
			(void)s;
			auto const& a = c.GetSystem<ApplicationSystem>();
			auto& b = c.ModifyComponent<ButtonInfo>(e);

			if (a->m_appIsFullscreen) { 
				a->ExitFullscreen();
				b.defaultTex = b.clickTex = b.hoverTex = "Menu-Windowed";
				c.ModifyComponent<Renderable>(e).materialName = b.defaultTex;
			}
			else { 
				a->EnterFullscreen(); 
				b.defaultTex = b.clickTex = b.hoverTex = "Menu-Fullscreen";
				c.ModifyComponent<Renderable>(e).materialName = b.defaultTex;
			}
			});
		m_buttons.emplace(ButtonAction::TOGGLEFULLSCREEN, fullscreenButton);

		// exit button
		Button exitButton(ButtonAction::EXIT, [](Coordinator& c, std::string const& s, Entity e) {
			(void)s;
			(void)e;
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
			ButtonInfo& b = m_coord.ModifyComponent<ButtonInfo>(e);
			if (b.type == INVALID_BUTTON) { continue; }

			// only check menu buttons
			if (m_menuOpen) {
				// Check if this entity is in the menu entities list
				auto it = std::find(m_menuEntities.begin(), m_menuEntities.end(), e);

				// skip if not a menu button
				if (it == m_menuEntities.end()) { continue; }
				if (b.action == ButtonAction::TOGGLEFULLSCREEN) {
					b.defaultTex = b.hoverTex = b.clickTex = 
						(m_coord.GetSystem<ApplicationSystem>()->m_appIsFullscreen ? "Menu-Fullscreen" : "Menu-Windowed");
				}
			}

			Transform const& t = m_coord.GetComponent<Transform>(e);
			Collider const& c = m_coord.GetComponent<Collider>(e);

			// Check collision with mouse
			if (!ButtonCollisionCheck(mx, my, t, c)) {
				m_coord.ModifyComponent<Renderable>(e).materialName = b.defaultTex;

				// clear hover state if mouse left this button
				if (m_hoveredEntity == e) 
				{
					m_hoveredEntity = INVALID_ENTITY;
				}
				continue;
			}
			auto& m = m_coord.ModifyComponent<Renderable>(e).materialName;
			m = b.hoverTex;
			//std::cout << "changed texture of button: " << e << " to " << b.hoverTex << '\n';
			// only play hover sound when first entering the button
			if (m_hoveredEntity != e) 
			{
				m_hoveredEntity = e;
				m_coord.GetSystem<AudioSystem>()->PlayRandomVariant("buttonHover", 2);
			}

			if (m_mouseClick) {
				m = b.clickTex;

				std::unordered_map<ButtonAction, Button>::iterator it = m_buttons.find(b.action);
				if (it != m_buttons.end()) {
					// Play audio if available
					m_coord.GetSystem<AudioSystem>()->PlayEntityAudio(e);
					// Invoke button Callable
					it->second(m_coord, b.target, e);
					// Only interact with 1 button
					return;
				}
				else {
					std::cerr << "[SceneManager] Button action of Entity: " << e << " not registered\n";
				}
				m_mouseClick = false;
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
			//std::cout << "Created camtrigger at pos: " << item["pos"]["x"] << ", " << item["pos"]["y"] << '\n';
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

			Renderable r;
			r.materialName = "InvisMat";
			// Invisible renderer component
			m_coord.AddComponent<Renderable>(e, r);
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
	std::vector<std::pair<Entity, json>> SceneManager::GetCameraKeyframes() const
	{
		std::vector<std::pair<Entity, json>> result;
		for (const auto& [entity, bounds] : m_camFrames)
		{
			result.emplace_back(entity, bounds);
		}
		return result;
	}

	Entity SceneManager::CreateCamTrigger(const Vec3& pos, const Vec2& scale, const json& bounds)
	{
		auto const& factory = m_coord.GetSystem<EntityFactory>();
		Transform t = { pos, 0.f, scale, Vec3{}, Vec2{}, LayerType::GAMELAYER, false, true, "CamTrigger" };
		Collider col = { ShapeType::AABB, ColliderType::Trigger, true, scale.x, scale.y, scale.x / 2.f, false, false };
		Entity e = factory->Create();
		m_coord.ModifyComponent<Transform>(e) = t;
		m_coord.AddComponent<Collider>(e, col);
		m_coord.AddComponent<Renderable>(e, Renderable{ "InvisMat" });
		m_coord.AddComponent<Color>(e, Color{});
		m_coord.AddComponent<RigidBody>(e, RigidBody{});
		m_camFrames[e] = bounds;
		m_coord.BufferEntitySignature(e);
		return e;
	}

	void SceneManager::DeleteCamTrigger(Entity camTriggerEntity)
	{
		auto const& factory = m_coord.GetSystem<EntityFactory>();
		m_camFrames.erase(camTriggerEntity);
		factory->RemoveEntity(camTriggerEntity);
	}

	
	void SceneManager::CreateWinCollider(json const& level) {

		if (!level.contains("WinZone")) return;

		m_winCollider = m_coord.GetSystem<EntityFactory>()->Create();
		m_winScreen = level["WinZone"]["ConfigFile"];

		Collider col{
			ShapeType::AABB, ColliderType::Trigger, true,	// isStatic
			level["WinZone"]["Scale"]["x"], level["WinZone"]["Scale"]["y"],
			std::max(level["WinZone"]["Scale"]["x"], level["WinZone"]["Scale"]["y"]) / 2.f
		};
		m_coord.AddComponent<Collider>(m_winCollider, col);
		m_coord.AddComponent<RigidBody>(m_winCollider, RigidBody{});	// Required for physics system

		// Invisible renderer component
		//m_coord.AddComponent<Renderable>(m_winCollider, Renderable{});
		//m_coord.AddComponent<Color>(m_winCollider, Color{});    // zero alpha

		Transform& t = m_coord.ModifyComponent<Transform>(m_winCollider);
		t.pos = {
			level["WinZone"]["Position"]["x"],
			level["WinZone"]["Position"]["y"],
			1.f
		};
		t.name = "WinZone";
	}
	json SceneManager::CacheWinCollider() {
		Transform const& t = m_coord.GetComponent<Transform>(m_winCollider);
		Collider const& c = m_coord.GetComponent<Collider>(m_winCollider);

		json j;
		j["Position"]["x"] = t.pos.x;
		j["Position"]["y"] = t.pos.y;

		j["Scale"]["x"] = c.width;
		j["Scale"]["y"] = c.height;

		j["ConfigFile"] = m_winScreen;

		return j;
	}
	void SceneManager::CreateWinScreen() {

		json const& w = m_coord.GetSystem<ResourceManager>()->GetJsonRaw(m_levelKeys[m_winScreen]);

		Entity cam = m_coord.GetSystem<GraphicsSystem>()->GetCamera();
		Vec3 const& camPos = m_coord.GetComponent<Transform>(cam).pos;

		m_coord.GetSystem<EntityFactory>()->CreateLevelEntities(w, Vec2{camPos.x, camPos.y});
		m_coord.GetSystem<PlayerControllerSystem>()->PauseInputs();
	}
	
	// Creates all entities required for a level through the entity factory
	// and sets up BGM for level, refreshes script manager.
	void SceneManager::LoadLevel(std::string const& levelname) {

		size_t key = m_levelKeys[levelname];
		json const& level = m_coord.GetSystem<ResourceManager>()->GetJsonRaw(key);

		m_previousLevelKey = m_currentLevelKey;
		m_currentLevelKey = key;
		//std::cout << "Curr level key: " << key << std::endl;

		// Unload everything first
		auto const& factory = m_coord.GetSystem<EntityFactory>();
		factory->DestroyLevelEntities();
		m_coord.GetSystem<ParticleSystem>()->StopAllEmitters();
		m_coord.GetSystem<PlayerControllerSystem>()->Reset();
		m_coord.GetSystem<CollectibleSystem>()->SpawnCollectibleParticles();

		// Set up Camera
		CreateSceneCamera(level);
		ReadCameraKeyframes(level["MainCamera"]["KeyFrames"]);
		InitSceneCameraBounds();
		CreateWinCollider(level);
		winScreenOn = false;

		// Create new level entities
		factory->CreateLevelEntities(level);

		// Play Background Music
		auto audio = m_coord.GetSystem<AudioSystem>();

		audio->StopBGM(); // stop previous bgm
		audio->SetBGMVolume(0.0f); // start silent for fade-in

		if (level.contains("BGM"))
		{
			if (level["BGM"].is_array())
			{
				// Multiple BGMs
				for (const auto& bgm : level["BGM"])
				{
					audio->PlayBGM(bgm.get<std::string>());
				}
			}
			else
			{
				// Single BGM
				audio->PlayBGM(level["BGM"].get<std::string>());
			}
		}
		else
		{
			audio->StopBGM();
		}

		// Fade BGM volume back in
		audio->FadeToVolume(0.2f, 1.5f);

		// Refresh Scripts for loaded level
		auto const& script = m_coord.GetSystem<Scripting::ScriptManager>();
		script->RefreshScripts();

		m_gameLoaded = true;
		CacheLevel();

		// Start the camera panning at the very start of the level
		m_coord.GetSystem<CameraSystem>()->StartCamPan();
	}

	void SceneManager::LoadLevelWithFade(std::string const& levelname, float fadeOutDuration, float fadeInDuration) {
		(void)fadeInDuration;

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
			cam.maxX = cam.minX = 0.f;
			cam.minY = cam.maxY = 0.f;
		}
		t.pos.x = t.pos.y = 0.f;
		t.pos.z = m_camDepth;
		t.layer = LayerType::GAMEUI;

		m_coord.AddComponent<Transform>(e, t);
		m_coord.AddComponent<Hierarchy>(e, Hierarchy{});
		m_coord.AddComponent<Camera>(e, cam);

		m_sceneCam = e;
	}

	void SceneManager::InitSceneCameraBounds() {
		Camera& cam = m_coord.ModifyComponent<Camera>(m_sceneCam);

		if (!m_camFrames.empty()) {
			cam.minX = (m_camFrames.begin()->second)["minX"];
			cam.maxX = (m_camFrames.begin()->second)["maxX"];
			cam.minY = (m_camFrames.begin()->second)["minY"];
			cam.maxY = (m_camFrames.begin()->second)["maxY"];

			m_camIndex = m_camFrames.begin()->first;
		}
		else {
			cam.maxX = cam.minX = 0.f;
			cam.minY = cam.maxY = 0.f;
		}
	}

	void SceneManager::RestartLevel() {

		//// close all menus
		//if (m_menuOpen) {
		//	std::cerr << "[SceneManager] Closing menu before restart\n";
		//	CloseMenu();
		//}

		json const& level = m_coord.GetSystem<ResourceManager>()->GetJsonRaw(m_currentLevelKey);
		auto const& factory = m_coord.GetSystem<EntityFactory>();
		factory->DestroyLevelEntities();
		m_coord.GetSystem<ParticleSystem>()->StopAllEmitters();
		m_coord.GetSystem<PlayerControllerSystem>()->Reset();
		m_coord.GetSystem<CollectibleSystem>()->SpawnCollectibleParticles();

		// Reset Camera
		CreateSceneCamera(level);
		ReadCameraKeyframes(level["MainCamera"]["KeyFrames"]);
		InitSceneCameraBounds();
		CreateWinCollider(level);

		// Create all Entities
		factory->CreateLevelEntities(level);

		// Play Background Music
		//auto audio = m_coord.GetSystem<AudioSystem>();
		//if (level.contains("BGM")) audio->PlayBGM(level["BGM"]);
		//else audio->StopBGM();

		// Refresh Scripts for loaded levelbutton
		auto const& script = m_coord.GetSystem<Scripting::ScriptManager>();
		script->RefreshScripts();

		std::cerr << "[SceneManager] Level restarted\n";

		m_gameLoaded = true;
		winScreenOn = false;

		// Start the camera panning at the very start of the level
		m_coord.GetSystem<CameraSystem>()->StartCamPan();
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
		// Winzone data;
		level["WinZone"] = CacheWinCollider();

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

		m_levelCache["MainCamera"] = m_coord.GetComponent<Camera>(m_sceneCam);
		m_levelCache["MainCamera"]["KeyFrames"] = WriteCameraKeyframes();
		m_levelCache["WinZone"] = CacheWinCollider();

		Transform const& t = m_coord.GetComponent<Transform>(m_sceneCam);
		m_levelCache["MainCamera"]["posX"] = t.pos.x;
		m_levelCache["MainCamera"]["posY"] = t.pos.y;
		m_levelCache["MainCamera"]["posZ"] = t.pos.z;

		m_levelCache["Entities"] = m_coord.GetSystem<EntityFactory>()->CacheLevelEntities();
	}

	// Restore scene to cached level -> restore after Stopping simulation in Editor
	void SceneManager::RestoreLevel() {

		if (m_levelCache.empty()) { return; }
		// Reset menus
		m_menuEntities.clear();
		m_menuOpen = false;
		//m_currentMenu.clear();
		winScreenOn = false;

		// Restore all entities to original
		auto const& factory = m_coord.GetSystem<EntityFactory>();
		factory->DestroyLevelEntities();
		m_coord.GetSystem<ParticleSystem>()->StopAllEmitters();
		m_coord.GetSystem<PlayerControllerSystem>()->Reset();
		m_coord.GetSystem<CollectibleSystem>()->SpawnCollectibleParticles();

		CreateSceneCamera(m_levelCache);
		ReadCameraKeyframes(m_levelCache["MainCamera"]["KeyFrames"]);
		InitSceneCameraBounds();
		CreateWinCollider(m_levelCache);

		factory->CreateLevelEntities(m_levelCache);
		
		// Restart audio
		auto const& audio = m_coord.GetSystem<AudioSystem>();
		std::string bgmkey = audio->GetBGM();
		audio->PlayBGM(bgmkey);  // handles continuation automatically


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
			CloseMenu(true);
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

		//std::cerr << "[SceneManager] Centering menu on camera at (" << camX << ", " << camY
		//	<< ") with zoom: " << zoom << "\n";

		// Create menu entities
		auto const& factory = m_coord.GetSystem<EntityFactory>();

		if (menuData.contains("Entities")) {
			for (auto const& entityData : menuData["Entities"]) {
				Entity e = 0;
				//std::cerr << "[Open Menu] Check 1\n";

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
						transform.pos.z += (float)m_menus.size() * 2.f;

						/*std::cerr << "  Prefab: " << prefabName
							<< " positioned at (" << transform.pos.x << ", " << transform.pos.y << ", " << transform.pos.z << ")"
							<< " scaled to (" << transform.scale.x << ", " << transform.scale.y << ")\n";*/
					}

					//std::cerr << "[Open Menu] Check 2 - Creating: " << prefabName << "\n";

					e = factory->Create(prefabName, transform);
				}
				else if (entityData.contains("Components")) {
					//std::cerr << "[Open Menu] Check 3\n";
					e = factory->Create();
					Transform t = entityData["Transform"];
					t.pos += { camX, camY, 0.f };
					t.pos.z += (float)m_menus.size() * 2.f;
					// Divide by zoom to make menu appear same size on screen
					t.scale.x /= zoom;
					t.scale.y /= zoom;
					//std::cerr << "[SceneManager] Created menu entity " << e << " to system\n"
					//	<< "Position <" << t.pos.x << ',' << t.pos.y << ',' << t.pos.z << ">\n";
					m_coord.ModifyComponent<Transform>(e) = t;

					factory->UnpackEntityComponents(e, entityData["Components"]);
				}

				if (e != INVALID_ENTITY) {
					m_menuEntities.insert(e);

					// Check if entity is a button and add to system
					if (m_coord.HasComponent<ButtonInfo>(e) &&
						m_coord.HasComponent<Transform>(e) &&
						m_coord.HasComponent<Collider>(e)) {
						//m_entities.insert(e);
						std::cerr << "[SceneManager] Added button entity " << e << " to system\n";
					}
				}
			}
			m_menus.push(menuname);
		}
		m_menuOpen = true;

		// pause for in game menus
		if (m_dispatcher)
		{
			m_dispatcher->QueueEvent(
				std::make_unique<InputEvent>(InputEvent::InputAction::PhysicsDebugMode, 
											InputEvent::InputActionState::Pressed));
		}
		//std::cerr << "[SceneManager] Game paused\n";

		std::cerr << "[SceneManager] Opened menu: " << menuname
			<< " with " << m_menuEntities.size() << " entities\n";
	}

	void SceneManager::CloseMenu(bool opening) {
		if (!m_menuOpen) {
			return;
		}

		// destroy all menu entities ------------------------------------------------
		auto const& factory = m_coord.GetSystem<EntityFactory>();
		for (Entity e : m_menuEntities) {
			factory->RemoveEntity(e);
		}
		m_menuEntities.clear();
		// --------------------------------------------------------------------------
		if (!opening) {
			if (!m_menus.empty()) {
				m_menus.pop();
				if (!m_menus.empty()) {
					OpenMenu(m_menus.top());
					m_menus.pop();
				}
			}
		}
		if (m_menuEntities.empty()) {
			if (m_dispatcher)
			{
				m_dispatcher->QueueEvent(
					std::make_unique<InputEvent>(InputEvent::InputAction::PhysicsDebugMode,
						InputEvent::InputActionState::Pressed));
			}
			//std::cerr << "[SceneManager] Game unpaused\n";
			m_menuOpen = false;
		}
	}

}	// NightSnack
/******************************************************************************
 * File:        SceneManager.h
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan (Primary, 60%)
 *				Bryan Lim Jun Jie (Secondary, 40%)
 *
 * Description:
 * This file declares the SceneManager class that performs scene managment 
 * such as level loading/saving, level caching when Play/Stop in Editor and 
 * GameUI interactions like button clicks.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "Core/ECS/Coordinator.h"
#include "Systems/Application/IEvent.h"
#include "Systems/Application/Input.h"

#include "Factory/EntityFactory.h"
#include "Systems/AudioSystem.h"
#include "Scripting/ScriptManager.h"
#include "Systems/Graphics.h"

#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <stack>

namespace NightSnack::Scene {

	// Wrapper that contains a button's action as well as the callable to be invoked,
	// When the button is activated.
	class Button {
	public:
		/**
		 * @brief Constructs a button with an action and callable.
		 * @param a Button action enum value.
		 * @param func Function pointer to invoke when the button is triggered.
		 */
		Button(ButtonAction a, void(*func)(Coordinator& c, std::string const&))
			: m_action{ a }, m_callable{ func } {}
		Button(Button const& b) noexcept {
			m_action = b.m_action;
			m_callable = b.m_callable;
		}

		/**
		 * @brief Invokes the button's callable.
		 * @param c ECS coordinator reference.
		 * @param s String argument (e.g., level or menu name).
		 */
		void operator()(Coordinator& c, std::string const& s) { m_callable(c, s); }

	private:
		ButtonAction m_action;
		void(*m_callable)(Coordinator& c, std::string const&);

	};

	// SceneManager system that handles level saving/loading, caching/restoring and
	// GameUI elements like button presses
	class SceneManager : public ISystem {
	public:
		/**
		 * @brief Constructs the SceneManager system.
		 * @param coord Reference to ECS Coordinator.
		 * @param disp Reference to the global EventDispatcher.
		 */
		SceneManager(Coordinator& coord, EventDispatcher& disp)
			: m_coord{ coord }, m_dispatcher{ &disp } , 
			m_scheduledLevelLoad(false),
			m_scheduledLevelName(""),
			m_levelLoadTimer(0.0f),
			m_scheduledExit(false),
			m_exitTimer(0.0f)
		{

			// set system signature
			m_systemSignature.set(GetComponentTypeID<ButtonInfo>());
			m_systemSignature.set(GetComponentTypeID<Transform>());
			m_systemSignature.set(GetComponentTypeID<Collider>());

			m_dispatcher->Subscribe<InputEvent>([this, &coord](const InputEvent& event) {
				if (event.pressed == InputEvent::InputActionState::Pressed) {
					switch (event.inputAction) {
					// Pause toggle
					case InputEvent::InputAction::PauseGame:
						if (!overlayOn) {
							//CreateOverlayMenu();
							overlayOn = true;
							m_coord.Pause();
						}
						else if (overlayOn) { 
							//RemoveOverlayMenu();
							overlayOn = false;
							m_coord.Unpause();
						}
						break;
					// Restart current level
					case InputEvent::InputAction::RestartGame:
						m_restart = true;
						std::cerr << "[SceneManager] Queuing restart\n";
						break;
					// Used to signal a mouse press event
					case InputEvent::InputAction::ClickLMB:
						if (m_coord.IsPaused()) return;
						CheckButtonCollisions();
						break;
					}
				}
			});
			// Reads level files
			ReloadLevels();
			// Register buttons
			RegisterButtons();
		}
		
		/**
		 * @brief Per-frame update called in the core update loop.
		*/
		void Update() override;
		/**
		 * @brief Resets m_levelKeys and loads all json files in "GameData/Levels"
		 */
		void ReloadLevels();
		/**
		 * @brief Creates the scene camera from level data.
		 * @param levelJson JSON describing the camera configuration.
		 */
		void CreateSceneCamera(json const&);
		/**
		 * @brief Initializes the scene camera's max and min bounds.
		 */
		void InitSceneCameraBounds();

		/**
		 * @brief Reads camera animation keyframes from JSON.
		 * @param cameraJson JSON containing camera keyframe data.
		 */
		void ReadCameraKeyframes(json const&);
		/**
		 * @brief Serializes camera keyframes into JSON.
		 * @return JSON representation of camera keyframes.
		 */
		json WriteCameraKeyframes();

		// Spawn Overlay menu (Pause menu) (currently not in use)
		//void CreateOverlayMenu();
		//void RemoveOverlayMenu();
		
		//void SetOverlayMenu(std::string const& filename);
		
		/**
		 * @brief  Register button callable functions and store them in m_buttons
		 */
		void RegisterButtons();
		/**
		 * @brief Check for collision btw mouse and all button entities
		 */
		void CheckButtonCollisions();

		// Load Level - Sets up everything needed to spawn level
		// Calls factory to create entities
		// Sets up bgm and menus

		/**
		 * @brief Loads a level immediately.
		 * @param filename Level file to load.
		 */
		void LoadLevel(std::string const& filename);

		/**
		 * @brief Loads a level with fade-out and fade-in transitions.
		 * @param levelname Target level name.
		 * @param fadeOutDuration Fade-out duration in seconds.
		 * @param fadeInDuration Fade-in duration in seconds.
		 */
		void LoadLevelWithFade(std::string const& levelname, float fadeOutDuration, float fadeInDuration);
		/**
		 * @brief Schedules an application exit after a delay.
		 * @param delay Delay in seconds.
		 */
		void ScheduleExit(float delay);
		/**
		 * @brief Restarts the currently loaded level.
		 */
		void RestartLevel();
		/**
		 * @brief Saves the current level to a file.
		 * @param filename Level file to save to.
		 * @param overwrite Whether to overwrite existing file.
		 * @return True if save was successful, false otherwise.
		 */
		bool SaveLevel(std::string const& filename, bool overwrite = 0);

		// Play/Pause/Stop functionality

		/**
		 * @brief Caches the current level state.
		 */
		void CacheLevel();
		/**
		 * @brief Restores the previously cached level state.
		 */
		void RestoreLevel();

		/**
		 * @brief Opens a UI menu.
		 * @param menuname Menu identifier.
		 */
		void OpenMenu(std::string const& menuname);

		/**
		 * @brief Closes the currently open menu.
		 */
		void CloseMenu();

		/**
		 * @brief Checks if a menu is currently open.
		 * @return True if a menu is open.
		 */
		bool IsMenuOpen() const { return m_menuOpen; }

		// Getters
		const std::unordered_map<std::string, size_t>& GetAvailableLevels() const { return m_levelKeys; }

		// Required by ISystem base class;
		Signature GetSignature() const override { return m_systemSignature; }
		std::string GetName() const override { return "SceneManager"; }

	private:
		Signature m_systemSignature{};
		EventDispatcher* m_dispatcher = nullptr;
		Coordinator& m_coord;
		
		json m_levelCache;
		//std::vector<std::pair<json, std::pair<json, Transform>>> m_levelCache;
		std::vector<std::pair<std::string, Transform>> m_overlayMenu;
		std::unordered_map<ButtonAction, Button> m_buttons;

		std::unordered_map<std::string, size_t> m_levelKeys;
		size_t m_currentLevelKey{};
		size_t m_previousLevelKey{};

		Entity m_sceneCam{ INVALID_ENTITY };
		float m_camDepth = 10.f;
		// Camera frames - Read from level.json
		std::map<Entity, json> m_camFrames;

		std::vector<Entity> m_overlayEntites;

		// Overlay menu
		bool m_gameLoaded = false;
		bool overlayOn = false;
		bool m_mouseClicked = false;
		bool m_restart = false;

		bool m_menuOpen = false;
		std::stack<std::set<Entity>> m_menuEntities;
		//std::string m_currentMenu;
		std::unordered_map<std::string, size_t> m_menuKeys; // Cache menu JSON keys
		
		bool m_scheduledLevelLoad;
		std::string m_scheduledLevelName;
		float m_levelLoadTimer;

		bool m_scheduledExit;
		float m_exitTimer;
		Entity m_player = INVALID_ENTITY;
		Entity m_camIndex = INVALID_ENTITY;

		// Winzone
		Entity m_winCollider;
		std::string m_winScreen;
		bool winScreenOn = false;

		void CreateWinCollider(json const& level);
		json CacheWinCollider();
		void CreateWinScreen();
	};

}	// NightSnack::Scene

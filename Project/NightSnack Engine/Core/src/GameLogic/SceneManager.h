/******************************************************************************
 * File:        SceneManager.h
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan (Primary, 100%)
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

#include <fstream>
#include <string>
#include <iostream>
#include <vector>

namespace NightSnack::Scene {

	// Wrapper that contains a button's action as well as the callable to be invoked,
	// When the button is activated.
	class Button {
	public:
		// Construct button with action and callable
		Button(ButtonAction a, void(*func)(Coordinator& c, std::string const&))
			: m_action{ a }, m_callable{ func } {
		}
		Button(Button const& b) noexcept {
			m_action = b.m_action;
			m_callable = b.m_callable;
		}
		// Invoke callable, passing a string target
		void operator()(Coordinator& c, std::string const& s) { m_callable(c, s); }

	private:
		ButtonAction m_action;
		void(*m_callable)(Coordinator& c, std::string const&);
	};

	// SceneManager system that handles level saving/loading, caching/restoring and
	// GameUI elements like button presses
	class SceneManager : public ISystem {
	public:
		SceneManager(Coordinator& coord, EventDispatcher& disp)
			: m_coord{ coord }, m_dispatcher{ &disp } {

			// set system signature
			m_systemSignature.set(GetComponentTypeID<ButtonInfo>());
			m_systemSignature.set(GetComponentTypeID<Transform>());
			m_systemSignature.set(GetComponentTypeID<Collider>());

			m_dispatcher->Subscribe<InputEvent>([this, &coord](const InputEvent& event) {
				if (event.pressed == InputEvent::InputActionState::Pressed) {
					switch (event.inputAction) {
					// Currently not in use
					case InputEvent::InputAction::PauseGame:
						if (!overlayOn) {
							CreateOverlayMenu();
							m_coord.Pause();
						}
						else { 
							RemoveOverlayMenu();
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

		// Main function to be called in Core Update loop
		void Update() override;
		// Resets m_levelKeys and loads all json files in "GameData/Levels"
		void ReloadLevels();

		// Spawn Overlay menu (Pause menu) (currently not in use)
		void CreateOverlayMenu();
		void RemoveOverlayMenu();
		
		//void SetOverlayMenu(std::string const& filename);
		
		// Register button callable functions and store them in m_buttons
		void RegisterButtons();
		// Check for collision btw mouse and all button entities
		void CheckButtonCollisions();

		// Load Level - Sets up everything needed to spawn level
		// Calls factory to create entities
		// Sets up bgm and menus
		void LoadLevel(std::string const& filename);
		void RestartLevel();
		bool SaveLevel(std::string const& filename, bool overwrite = 0);

		// Play/Pause/Stop functionality
		void CacheLevel();
		void RestoreLevel();

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

		std::vector<Entity> m_overlayEntites;

		// Overlay menu
		bool m_gameLoaded = false;
		bool overlayOn = false;
		bool m_mouseClicked = false;
		bool m_restart = false;
	};

}	// NightSnack::Scene

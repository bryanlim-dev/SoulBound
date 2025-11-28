#pragma once

#include "Core/ECS/Coordinator.h"
#include "Systems/Application/IEvent.h"
#include "Factory/EntityFactory.h"
#include "Systems/AudioSystem.h"
#include "Scripting/ScriptManager.h"

#include <fstream>
#include <string>
#include <iostream>
#include <vector>

namespace NightSnack::Scene {

	class SceneManager : public ISystem {
	public:
		SceneManager(Coordinator& coord, EventDispatcher& disp)
			: m_coord{ coord }, m_dispatcher{ &disp } {

			m_dispatcher->Subscribe<InputEvent>([this, &coord](const InputEvent& event) {
				if (event.pressed == InputEvent::InputActionState::Pressed) {
					switch (event.inputAction) {
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
					}
				}
			});

			// Reads level files
			ReloadLevels();
			// Create default level
			LoadLevel("DefaultLevel");
		}

		void Update() override {}

		// Resets m_levelKeys and loads all json files in "GameData/Levels"
		void ReloadLevels();

		// Spawn Overlay menu (Pause menu)
		void CreateOverlayMenu();
		void RemoveOverlayMenu();
		
		void SetOverlayMenu(std::string const& filename);

		// Load Level - Sets up everything needed to spawn level
		// Calls factory to create entities
		// Sets up bgm and menus
		void LoadLevel(std::string const& filename);
		bool SaveLevel(std::string const& filename, bool overwrite = 0);

		// Play/Pause functionality
		void CacheLevel();
		void RestoreLevel();

		// Required by ISystem base class;
		Signature GetSignature() const override { return m_systemSignature; }
		std::string GetName() const override { return "SceneManager"; }

	private:
		Signature m_systemSignature{};
		EventDispatcher* m_dispatcher = nullptr;

		Coordinator& m_coord;

		std::vector<std::pair<json, Transform>> m_levelCache;
		std::vector<std::pair<std::string, Transform>> m_overlayMenu;

		std::unordered_map<std::string, size_t> m_levelKeys;
		size_t m_currentLevelKey{};
		size_t m_previousLevelKey{};

		std::vector<Entity> m_overlayEntites;

		// Overlay menu
		bool inGame = false;
		bool overlayOn = false;

	};

	// LEVELS
	// Ability to Load from json
	// save to json

	// Level management in game
	// Swap scenes

	

}	// NightSnack::Scene

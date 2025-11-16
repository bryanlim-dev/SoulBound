#pragma once

#include "Core/ECS/Coordinator.h"
#include "Systems/Application/IEvent.h"
#include "Factory/EntityFactory.h"

#include <fstream>
#include <string>
#include <iostream>
#include <json.hpp>
using json = nlohmann::json;

namespace NightSnack::Serializer {

	// SaveLevelToFile
	// LoadLevelFromFile
	// Default naming convention

	// SavePrefab
	// LoadPrefab


}	// NightSnack::Serializer


namespace NightSnack::Scene {

	class LevelManager : public ISystem {

		LevelManager(Coordinator& coord, EventDispatcher& disp) 
			: m_coord{ coord }, m_dispatcher{ &disp } {
		
			m_dispatcher->Subscribe<InputEvent>([this, &coord](const InputEvent& event) {
				if (event.pressed == InputEvent::InputActionState::Pressed) {
					/*switch (event.inputAction) {
					case InputEvent::InputAction::NextLevel:
						break;
					case InputEvent::InputAction::PreviousLevel:
						break;
					case InputEvent::InputAction::RestartLevel:
						break;
					}*/
				}
			});

		}

		void Update(int steps) override {}

		void LoadLevel(std::string const& filename);
		void RestartLevel() {}
		void PauseGame() {}

		void UnloadLevel();

		bool SaveLevel() {}

		// Required by ISystem base class;
		Signature GetSignature() const override { return m_systemSignature; }
		std::string GetName() const override { return "LevelManager"; }

	private:
		Signature m_systemSignature{};
		EventDispatcher* m_dispatcher = nullptr;
		Coordinator& m_coord;

		json m_currentLevel{};
	};

	// LEVELS
	// Ability to Load from json
	// save to json

	// Level management in game
	// Swap scenes

	// in Editor
	// Load create new level from default
	// Save level to file
	// Load level from file






}	// NightSnack::Scene

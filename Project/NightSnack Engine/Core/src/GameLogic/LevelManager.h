#pragma once

#include "Core/ECS/Coordinator.h"
#include "Systems/Application/IEvent.h"
#include "Factory/EntityFactory.h"

#include <string>
#include <json.hpp>

namespace NightSnack {

	class LevelManager : public ISystem {

		LevelManager(Coordinator& coord, EventDispatcher& disp) 
			: m_coord{ coord }, m_dispatcher{ &disp } {
		
			m_dispatcher->Subscribe<InputEvent>([this, &coord](const InputEvent& event) {
				if (event.pressed == InputEvent::InputActionState::Pressed) {
					switch (event.inputAction) {
					case InputEvent::InputAction::PhysicsDebugMode:
						break;
					}
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







}	// NightSnack

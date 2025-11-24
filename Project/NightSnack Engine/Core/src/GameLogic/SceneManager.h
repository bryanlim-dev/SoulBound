#pragma once

#include "Core/ECS/Coordinator.h"
#include "Systems/Application/IEvent.h"
#include "Factory/EntityFactory.h"

#include <fstream>
#include <string>
#include <iostream>

namespace NightSnack::Scene {

	class SceneManager : public ISystem {
	public:
		SceneManager(Coordinator& coord, EventDispatcher& disp)
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

		void Update() override {}

		void LoadLevel(std::string const& filename);
		void RestartLevel() {}
		void PauseGame() {}

		void UnloadLevel();

		bool SaveLevel() {}

		void CacheLevel();
		void RestoreLevel();

		// Required by ISystem base class;
		Signature GetSignature() const override { return m_systemSignature; }
		std::string GetName() const override { return "LevelManager"; }

	private:
		Signature m_systemSignature{};
		EventDispatcher* m_dispatcher = nullptr;
		Coordinator& m_coord;

		std::vector<std::pair<std::string, Transform>> m_levelCache;

		json m_currentLevel{};
	};

	// LEVELS
	// Ability to Load from json
	// save to json

	// Level management in game
	// Swap scenes

	// Take snapshot of current world

	inline void SceneManager::CacheLevel() {

		std::set<Entity> entities = m_coord.GetLivingEntities();
		if (entities.empty()) { return; }

		m_levelCache.clear();
		m_levelCache.reserve(entities.size());
		
		for (Entity e : entities) {
			m_levelCache.emplace_back(std::make_pair(
				m_coord.GetSystem<EntityFactory>()->GetEntityPrefabName(e),
				m_coord.GetComponent<Transform>(e)));
		}
	}

	inline void SceneManager::RestoreLevel() {

		if (m_levelCache.empty()) { return; }

		auto const& factory = m_coord.GetSystem<EntityFactory>();
		factory->UnloadLevel();

		for (auto const& unit : m_levelCache) {
			factory->Create(unit.first, unit.second);
		}
	}

}	// NightSnack::Scene

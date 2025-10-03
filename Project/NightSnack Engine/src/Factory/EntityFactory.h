/******************************************************************************
 * File:        EntityFactory.h
 * Project:     NightSnack
 * Author(s):
 *
 * Description:
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#ifndef ENTITYFACTORY_H
#define ENTITYFACTORY_H

#include <unordered_map>
#include <string>
#include <json.hpp>
#include <fstream>

#include "ECS/Entity.h"
#include "ECS/ISystem.h"
#include "ECS/Coordinator.h"
#include "ECS/Component.h"
#include "Factory/PrefabManager.h"
#include "Systems/Input.h"
#include "Systems/IEvent.h"

using json = nlohmann::json;

class EntityFactory : public ISystem {

public:

	EntityFactory(Coordinator& coord, EventDispatcher& disp, std::string const& archetypes_filename)
		: m_coord{ coord }, m_dispatcher{ &disp }, m_prefabManager{ coord } {

		m_prefabManager.Initialize();
		LoadJsonFile(archetypes_filename);

		// Setup input events;
		m_dispatcher->Subscribe<InputEvent>([this, &coord](const InputEvent& event) {
			if (event.pressed == InputEvent::InputActionState::Pressed) {
				switch (event.inputAction) {
					case InputEvent::InputAction::CreateSquare:
						Create("Square");
						//std::cout << "Create Square!\n";
						break;

					case InputEvent::InputAction::CreateCircle:
						Create("Circle");
						//std::cout << "Create Circle!\n";
						break;

					case InputEvent::InputAction::CreateBananaCircle:
						Create("Player");
						//std::cout << "Create Banana!\n";
						break;

					case InputEvent::InputAction::DoubleEntities:
						DoubleEntities();
						//std::cout << "Doubled Entities!\n";
						break;

					case InputEvent::InputAction::HalfEntities:
						HalfEntities();
						//std::cout << "Removed Entities!\n";
						break;
				}
			}

			});
	}

	void LoadJsonFile(std::string const& filename) {
		std::ifstream file(filename);
		if (!file.is_open()) {
			throw std::runtime_error("Failed to open archetypes file: " + filename);
		}

		try {
			file >> m_archetypes;
		}
		catch (json::parse_error& e) {
			throw std::runtime_error("JSON parse error at byte " + std::to_string(e.byte) + ": " + e.what());
		}
	}

	void Update(float dt, Coordinator& coord) override;

	Signature GetSignature() const override { return Signature{}; }
	std::string GetName() const override { return "EntityFactory"; }

	Entity Create(std::string const& name);
	Entity Create(std::string const& name, Transform trans);

	Entity Clone(Entity e, Transform trans);
	void DoubleEntities();
	void HalfEntities();

	void Remove(Entity e);

	std::vector<Entity> m_toBeDestroyed;

private:
	Coordinator& m_coord;
	PrefabManager m_prefabManager;
	EventDispatcher* m_dispatcher;
	json m_archetypes{};

	// map of prefab name to entity;
	std::unordered_map<Entity, std::string> entityToPrefab;
};

// First obtains all archetypes through initialization;

// LoadLevel() -> SetComponents to create entities with specified transforms and components;



#endif // ENTITYFACTORY_H
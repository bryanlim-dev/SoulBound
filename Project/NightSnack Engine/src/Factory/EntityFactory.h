/******************************************************************************
 * File:        EntityFactory.h
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan
 *
 * Description:
 * Declares EntityFactory class that creates entities from prefabs/archetypes
 * read from a json file.
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

#include "Core/ECS/Coordinator.h"
#include "Factory/PrefabManager.h"
#include "Systems/Application/Input.h"
#include "Systems/Application/IEvent.h"

namespace NightSnack {

	class EntityFactory : public ISystem {

	public:
		/************************************************************************/
		/*!
		\brief
		Constructor for Factory, requires coordinator, event dispatcher and file
		of archetypes.
		Creates the prefabManager that will register all component loaders that
		will help with entity creation.
		*/
		/************************************************************************/
		EntityFactory(Coordinator& coord, EventDispatcher& disp, std::string const& archetypes_filename, std::string const& level0_file)
			: m_coord{ coord }, m_dispatcher{ &disp }, m_prefabManager{ coord } {

			m_prefabManager.Initialize();
			LoadArchetypes(archetypes_filename);
			LoadLevel(level0_file);

			// Setup input events;
			m_dispatcher->Subscribe<InputEvent>([this, &coord](const InputEvent& event) {
				if (event.pressed == InputEvent::InputActionState::Pressed) {
					switch (event.inputAction) {
					case InputEvent::InputAction::CreateSquare:
						if (coord.GetLivingEntities().size() >= MAX_ENTITIES) { break; }
						Create("Square");
						//std::cout << "Create Square!\n";
						break;

					case InputEvent::InputAction::CreateCircle:
						if (coord.GetLivingEntities().size() >= MAX_ENTITIES) { break; }
						Create("Circle");
						//std::cout << "Create Circle!\n";
						break;

					//case InputEvent::InputAction::CreateBananaCircle:
					//	if (coord.GetLivingEntities().size() >= MAX_ENTITIES) { break; }
					//	Create("Player");
					//	//std::cout << "Create Banana!\n";
					//	break;

					case InputEvent::InputAction::DoubleEntities:
						DoubleEntities();
						//std::cout << "Doubled Entities!\n";
						break;

					case InputEvent::InputAction::HalfEntities:
						HalfEntities();
						//std::cout << "Removed Entities!\n";
						break;
					case InputEvent::InputAction::CheckEntityNum:
						std::cout << "Entity Num: " << coord.GetLivingEntityCount() << '\n';
						break;
					}
						
				}

				});
		}

		// For now the Factory will read the json file by itself, in M2 will transfer responsibility to ResourceManager;
		void LoadArchetypes(std::string const& filename) {
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

		// Read a Level file and creates Entities from prefabs in m_archetypes;
		void LoadLevel(std::string const& levelfile) {
			std::ifstream file(levelfile);
			if (!file.is_open()) {
				throw std::runtime_error("Failed to open level file: " + levelfile);
			}

			try {
				file >> m_currentLevel;
			}
			catch (json::parse_error& e) {
				throw std::runtime_error("JSON parse error at byte " + std::to_string(e.byte) + ": " + e.what());
			}

			for (auto const& item : m_currentLevel["Entities"]) {
				Entity e = Create(item["prefab"], item["Transform"]);
				if (item.contains("Layer")) {
					if (item["Layer"] == "Background")
					m_coord.ModifyComponent<Transform>(e).layer = LayerType::BACKGROUND;
					if (item["Layer"] == "GameUI")
					m_coord.ModifyComponent<Transform>(e).layer = LayerType::GAMEUI;
				}
				else {
					m_coord.ModifyComponent<Transform>(e).layer = LayerType::GAMELAYER;
				}
			}
		}

		/************************************************************************/
		/*!
		\brief
		Currently does not do anything, but is required to be defined by ISystem
		*/
		/************************************************************************/
		void Update(int steps) override;

		// Required functions from base class ISystem.
		Signature GetSignature() const override { return Signature{}; }
		std::string GetName() const override { return "EntityFactory"; }

		/************************************************************************/
		/*!
		\brief
		Creates an entity with the name of desired prefab.

		\param name
		Name of prefab.

		\return
		Returns ID of entity created.
		*/
		/************************************************************************/
		Entity Create(std::string const& name);

		/************************************************************************/
		/*!
		\brief
		Creates an entity with the name of desired prefab and with a specific
		transformation to be applied on the Entity.

		\param name
		Name of prefab.
		\param trans
		Transformation to be applied.

		\return
		Returns ID of entity created.
		*/
		/************************************************************************/
		Entity Create(std::string const& name, Transform trans);

		/************************************************************************/
		/*!
		\brief
		Creates an entity by clone from a specific entity's prefab and applying a
		transformation on the created entity.

		\param e
		Entity to be cloned from.
		\param trans
		Transform of new entity.

		\return
		Returns ID of entity cloned.
		*/
		/************************************************************************/
		Entity Clone(Entity e, Transform trans);

		/************************************************************************/
		/*!
		\brief
		Creates entites with random transforms. until the number of living entities
		has doubled.
		*/
		/************************************************************************/
		void DoubleEntities();

		/************************************************************************/
		/*!
		\brief
		Removes entities until half the current living entities (except for default
		testing scene).
		*/
		/************************************************************************/
		void HalfEntities();

		/************************************************************************/
		/*!
		\brief
		Adds entity to m_toBeDestroyed (These entities will be destroyed at the
		end of the core loop).
		*/
		/************************************************************************/
		void Remove(Entity e);

		std::set<Entity> m_toBeDestroyed;

	private:
		Coordinator& m_coord;
		PrefabManager m_prefabManager;
		EventDispatcher* m_dispatcher;

		json m_archetypes{};
		json m_currentLevel{};

		// map of prefab name to entity;
		std::unordered_map<Entity, std::string> entityToPrefab;
	};

}	// NightSnack

#endif // ENTITYFACTORY_H
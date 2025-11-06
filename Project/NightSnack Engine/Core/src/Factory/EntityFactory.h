/******************************************************************************
 * File:        EntityFactory.h
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan
 *              Clement Ang Yang Ying (Secondary, 5%) - Changed loading to use RM
 * 
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
#include "Systems/Graphics.h"

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
		EntityFactory(Coordinator& coord, EventDispatcher& disp, std::string const& archetypes_filename, std::string const& archetype_states_filename, std::string const& level0_file)
			: m_coord{ coord }, m_dispatcher{ &disp }, m_prefabManager{ coord } {

			(void)archetype_states_filename;
			m_prefabManager.Initialize();
			LoadArchetypes(archetypes_filename);
			LoadLevel(level0_file);

			m_stress = false;

			// Setup input events;
			m_dispatcher->Subscribe<InputEvent>([this, &coord](const InputEvent& event) {
				if (event.pressed == InputEvent::InputActionState::Pressed) {
					switch (event.inputAction) {

					case InputEvent::InputAction::StressTest:
						if (!m_stress) {
							UnLoadLevel();
							StressTestCreate(LayerType::GAMELAYER, "StressTest", 69, Vec2{5.f, 5.f});
							m_stress = true;
						}
						break;

					case InputEvent::InputAction::LayerTest:
						if (m_stress && !m_layer) {
							StressTestCreate(LayerType::BACKGROUND, "LayerTest", 420, Vec2{ 10.f, 10.f });
							m_layer = true;
						}
						break;

					case InputEvent::InputAction::EndStressTest:
						if (m_stress) {
							StressTestEnd();
							m_stress = false;
							m_layer = false;
						}
						break;
					}
				}

				});
		}

		void LoadArchetypes(std::string const& filename) {

			try {
				LOAD_RESULT result = m_coord.GetSystem<ResourceManager>()->LoadJson(filename, m_archetypesKey);
				if (result == LOAD_RESULT::LOAD_SUCCESS || result == LOAD_RESULT::LOAD_SUCCESS_STOPPED_UNLOAD)
					return;
				else if (result == LOAD_FAIL_BADPATH)
					throw std::runtime_error("Failed to open archetypes file due to bad file path: " + filename);
			}
			catch (json::parse_error& e) {
				throw std::runtime_error("JSON parse error at byte " + std::to_string(e.byte) + ": " + e.what());
			}

			//std::ifstream file(filename);
			//if (!file.is_open()) {
			//	throw std::runtime_error("Failed to open archetypes file: " + filename);
			//}

			//try {
			//	file >> m_archetypes;
			//}
			//catch (json::parse_error& e) {
			//	throw std::runtime_error("JSON parse error at byte " + std::to_string(e.byte) + ": " + e.what());
			//}
		}

		// Read a Level file and creates Entities from prefabs in m_archetypes;
		void LoadLevel(std::string const& levelfile) {
			//std::ifstream file(levelfile);
			//if (!file.is_open()) {
			//	throw std::runtime_error("Failed to open level file: " + levelfile);
			//}

			//try {
			//	file >> m_currentLevel;
			//}
			//catch (json::parse_error& e) {
			//	throw std::runtime_error("JSON parse error at byte " + std::to_string(e.byte) + ": " + e.what());
			//}

			try {
				LOAD_RESULT result = m_coord.GetSystem<ResourceManager>()->LoadJson(levelfile, m_currentLevelLey);
				if (result == LOAD_RESULT::LOAD_SUCCESS || result == LOAD_RESULT::LOAD_SUCCESS_STOPPED_UNLOAD) {/* Empty by design*/}
				else if (result == LOAD_FAIL_BADPATH)
					throw std::runtime_error("Failed to open archetypes file due to bad file path: " + levelfile);
			}
			catch (json::parse_error& e) {
				throw std::runtime_error("JSON parse error at byte " + std::to_string(e.byte) + ": " + e.what());
			}

			m_currentLevel = m_coord.GetSystem<ResourceManager>()->GetJsonRaw(m_currentLevelLey);


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
			for (Entity e : m_coord.GetLivingEntities()) {
				m_coord.BufferEntitySignature(e);
			}
		}

		void UnLoadLevel() {
			for (Entity e : m_coord.GetLivingEntities()) {
				if (m_coord.HasComponent<Camera>(e)) { continue; }
				if (e == m_coord.GetSystem<GraphicsSystem>()->FindCamera()) { continue; }

				RemoveEntity(e);
			}
		}

		/************************************************************************/
		/*!
		\brief
		Currently does not do anything, but is required to be defined by ISystem
		*/
		/************************************************************************/
		void Update(int steps) override;

		//void EditPrefab(std::string const& s) {
		//	if (m_archetypes[s].contains("RigidBody")) {

		//	}
		//	if (m_archetypes[s].contains("Renderable")) {

		//	}
		//}

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
		Create 1000 entities for stress test - Combination 1;
		*/
		/************************************************************************/
		void StressTestCreate(LayerType, std::string const&, int, Vec2 const&);
		bool m_stress{ false };
		bool m_layer{ false };

		/************************************************************************/
		/*!
		\brief
		Removes entities created for stress test;
		*/
		/************************************************************************/
		void StressTestEnd();

		/************************************************************************/
		/*!
		\brief
		Adds entity to m_toBeDestroyed (These entities will be destroyed at the
		end of the core loop).
		*/
		/************************************************************************/
		void RemoveEntity(Entity e);

		void PrintPrefabNames() const;
		std::unordered_map<std::string, std::string> GetPrefabMaterials() const;
		std::vector<std::string> const& GetPrefabNames() const;
		std::string GetEntityPrefabName(Entity e) const;
		const json& GetArchetypes() const;

		std::set<Entity> m_toBeDestroyed;

	private:
		Coordinator& m_coord;
		PrefabManager m_prefabManager;
		EventDispatcher* m_dispatcher;

		//json m_archetypes{};
		//json m_archetypesStates{};
		json m_currentLevel{}; //Copy yo not effect the orginal

		size_t m_archetypesKey{};
		//size_t m_archetypesStatesKey{};
		size_t m_currentLevelLey{};

		// map of prefab name to entity;
		std::unordered_map<Entity, std::string> entityToPrefab;
	};

}	// NightSnack

#endif // ENTITYFACTORY_H
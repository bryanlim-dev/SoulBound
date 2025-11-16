/******************************************************************************
 * File:        EntityFactory.h
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan
 *              Clement Ang Yang Ying (Secondary, 5%) - Changed loading to use RM
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
#include <filesystem>

#include "Core/ECS/Coordinator.h"
#include "Factory/PrefabManager.h"
#include "Systems/Application/Input.h"
#include "Systems/Application/IEvent.h"
#include "ResourceManager/ResourceManager.h"

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
		EntityFactory(Coordinator& coord, EventDispatcher& disp, std::string const& gameDataPath)
			: m_coord{ coord }, m_dispatcher{ &disp }, m_prefabManager{ coord }, GAMEDATA_DIR{ gameDataPath } {

			m_prefabManager.Initialize();

			CreateFolders();

			LoadArchetypes("../assets/GameData/Archetypes/Archetypes.json");
			LoadLevel("../assets/GameData/DefaultLevel.json");

			SaveLevel("Level0", true);

			// Setup input events;
			m_dispatcher->Subscribe<InputEvent>([this, &coord](const InputEvent& event) {
				if (event.pressed == InputEvent::InputActionState::Pressed) {
					switch (event.inputAction) {

					case InputEvent::InputAction::StressTest:
						break;
					}
				}
			});
		}

		void LoadArchetypes(std::string const& filename) {

			try {
				LOAD_RESULT result = m_coord.GetSystem<ResourceManager>()->LoadJson(filename, m_archetypesKey);
				if (result == LOAD_FAIL_BADPATH) {
					throw std::runtime_error("Failed to open archetypes file due to bad file path: " + filename);
				}
			}
			catch (json::parse_error& e) {
				throw std::runtime_error("JSON parse error at byte " + std::to_string(e.byte) + ": " + e.what());
			}

			m_prefabNames.clear();

			for (auto& [name, data] : GetArchetypes().items()) {
				m_prefabNames.emplace_back(name);
			}

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

		// Saves all entities in the current level
		bool SaveLevel(std::string const& levelName, bool overwrite);

		// Read a Level file and creates Entities from prefabs in m_archetypes;
		void LoadLevel(std::string const& levelfile) {

			m_coord.GetSystem<ResourceManager>()->UnloadJson(m_currentLevelKey);

			try {
				LOAD_RESULT result = m_coord.GetSystem<ResourceManager>()->LoadJson(levelfile, m_currentLevelKey);
				if (result == LOAD_FAIL_BADPATH) {
					throw std::runtime_error("Failed to open archetypes file due to bad file path: " + levelfile);
				}
			}
			catch (json::parse_error& e) {
				throw std::runtime_error("JSON parse error at byte " + std::to_string(e.byte) + ": " + e.what());
			}

			m_currentLevel = m_coord.GetSystem<ResourceManager>()->GetJsonRaw(m_currentLevelKey);

			for (auto const& item : m_currentLevel["Entities"]) {
				Create(item["prefab"], item["Transform"]);
			}
			for (Entity e : m_coord.GetLivingEntities()) {
				m_coord.BufferEntitySignature(e);
			}
		}

		void UnloadLevel();

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
		Adds entity to m_toBeDestroyed (These entities will be destroyed at the
		end of the core loop).
		*/
		/************************************************************************/
		void RemoveEntity(Entity e);

		bool SaveAsPrefab(Entity e, std::string const& prefabName, bool overwrite);

		void CreateFolders() const;
		void TransferPrefabs();

		void PostUpdate();

		void PrintPrefabNames() const;
		std::unordered_map<std::string, std::string> GetPrefabMaterials() const;
		std::vector<std::string> const& GetPrefabNames() const;
		std::string GetEntityPrefabName(Entity e) const;
		const json& GetArchetypes() const;


	private:
		Coordinator& m_coord;
		PrefabManager m_prefabManager;
		EventDispatcher* m_dispatcher;

		// Entities that have been marked for deletion
		std::set<Entity> m_toBeDestroyed;

		json m_archetypes{};
		//json m_archetypesStates{};
		json m_currentLevel{}; //Copy yo not effect the orginal

		size_t m_archetypesKey{};
		//size_t m_archetypesStatesKey{};
		size_t m_currentLevelKey{};

		// map of prefab name to entity;
		std::unordered_map<Entity, std::string> entityToPrefab;
		std::vector<std::string> m_prefabNames;

		std::filesystem::path GAMEDATA_DIR;
	};

}	// NightSnack

#endif // ENTITYFACTORY_H
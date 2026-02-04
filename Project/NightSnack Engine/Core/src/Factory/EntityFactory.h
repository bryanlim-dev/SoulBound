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
		Constructor for Factory, requires coordinator and event dispatcher
		Creates the prefabManager that will register all component loaders that
		will help with entity creation.
		*/
		/************************************************************************/
		EntityFactory(Coordinator& coord, EventDispatcher& disp)
			: m_coord{ coord }, m_dispatcher{ &disp }, m_prefabManager{ coord } {

			// Register component loaders in PrefabManager.
			m_prefabManager.Initialize();
			// Create "Prefabs" and "Levels" folder in "assets/GameData" if they don't exist.
			CreateFolders();
			// Read Level and Prefab json files.
			ReloadPrefabs();

			// Default entity with no components.
			m_prefabKeys["++Default"] = 0;
		}

		/************************************************************************/
		/*!
		\brief
		Resets m_prefabKeys and loads all json files in "GameData/Prefabs"
		*/
		/************************************************************************/
		void ReloadPrefabs();

		/************************************************************************/
		/*!
		\brief
		Reloads a specific prefab -> Updates json stored by the RM
		\param prefab
		Name of prefab to reload.
		*/
		/************************************************************************/
		void UpdatePrefab(std::string const& prefab);

		/************************************************************************/
		/*!
		\brief
		Saves all entities in the current level into a json file and returns it
		\return
		Level json, now containing all entity information.
		*/
		/************************************************************************/
		json SaveLevelEntities();
		void UnpackEntityComponents(Entity e, json const&);

		json CacheLevelEntities();
		json CacheEntityChildren(Hierarchy const&);
		void CreateEntityChildren(json const&, Entity);
		std::string EnsurePrefabExists(Entity e, std::string const& prefabName, bool isModified);

		/************************************************************************/
		/*!
		\brief
		Creates entities found in a level from a level json file.
		\param
		Level json, containing information of entities to create.
		*/
		/************************************************************************/
		void CreateLevelEntities(json const& level);

		/************************************************************************/
		/*!
		\brief
		Destroys all entities and Resets all ECS managers.
		*/
		/************************************************************************/
		void DestroyLevelEntities();

		/************************************************************************/
		/*!
		\brief
		Currently does not do anything, but is required to be defined by ISystem.
		*/
		/************************************************************************/
		void Update() override;

		/************************************************************************/
		/*!
		\brief
		Performs necessary operations at end of Core update Loop like destroying
		entities marked for deletion and unloading jsons.
		*/
		/************************************************************************/
		void PostUpdate();

		/************************************************************************/
		/*!
		\brief
		Creates an entity with no components.

		\return
		Returns ID of entity created.
		*/
		/************************************************************************/
		Entity Create();

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
		Creates an entity with a json storing component data.

		\param prefab
		Json containing component data to be used for entity creation.

		\return
		Returns ID of entity created.
		*/
		/************************************************************************/
		Entity Create(json const& prefab, Transform const& trans);

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
		Entity Create(std::string const& name, Transform const& trans);

		/************************************************************************/
		/*!
		\brief
		Creates an entity by clone from a specific entity's prefab.

		\param e
		Entity to be cloned from.

		\return
		Returns ID of entity cloned.
		*/
		/************************************************************************/
		Entity Clone(Entity e, Vec2 const& pos);

		Entity CreateCollider(Collider const&, Transform const&);
		Entity CreateSprite(Renderable const& r, Transform const& t);

		/************************************************************************/
		/*!
		\brief
		Adds entity to m_toBeDestroyed (These entities will be destroyed at the
		end of the core loop).
		*/
		/************************************************************************/
		void RemoveEntity(Entity e);

		/************************************************************************/
		/*!
		\brief
		Saves the component data of an entity to a prefab named "prefabName".

		\param prefabName
		Name of output prefab.

		\param overwrite
		Whether to overwrite an existing prefab, if 0 and existing, function will
		not save and return false.
		*/
		/************************************************************************/
		bool SaveAsPrefab(Entity e, std::string const& prefabName, bool overwrite = 0);

		/************************************************************************/
		/*!
		\brief
		Store all entity component information into a json and returns it.
		\return
		Json containing the entity information.
		*/
		/************************************************************************/
		json CacheEntityComponents(Entity e) const;

		/************************************************************************/
		/*!
		\brief
		Create "Prefabs" and "Levels" folder in "assets/GameData" if they don't exist.
		*/
		/************************************************************************/
		void CreateFolders() const;

		/************************************************************************/
		/*!
		\brief
		Print names of loaded prefabs
		*/
		/************************************************************************/
		void PrintPrefabNames() const;

		/************************************************************************/
		/*!
		\brief
		Obtain names of loaded prefabs
		*/
		/************************************************************************/
		std::vector<std::string> GetPrefabNames() const;

		/************************************************************************/
		/*!
		\brief
		Obtain the prefab name of a specific entity
		*/
		/************************************************************************/
		std::string GetEntityPrefabName(Entity e) const;

		/************************************************************************/
		/*!
		\brief
		Obtain materials of prefabs if they have a "Renderable" component
		*/
		/************************************************************************/
		std::unordered_map<std::string, std::string> GetPrefabMaterials() const;

		// Required functions from base class ISystem.
		Signature GetSignature() const override { return Signature{}; }
		std::string GetName() const override { return "EntityFactory"; }

		void MapEntityPrefab(Entity e, std::string const&);

	private:
		Coordinator& m_coord;
		PrefabManager m_prefabManager;
		EventDispatcher* m_dispatcher;

		// Entities that have been marked for deletion
		std::set<Entity> m_toBeDestroyed;

		std::unordered_map<std::string, size_t> m_prefabKeys;
		//std::unordered_map<std::string, size_t> m_levelKeys;

		std::vector<size_t> m_toBeUnloaded;
		std::vector<std::pair<std::string, Transform>> m_prefabCache;

		// map of prefab name to entity;
		std::unordered_map<Entity, std::string> entityToPrefab;
		std::unordered_multimap<std::string, Entity> prefabToEntities;	// Hot reloading prefabs;
		std::vector<std::string> m_prefabNames;

		// ---------------------------------------------------------------------
		// Split Monolithic Json file (containing all archetypes) into individual prefab json files.
		void TransferPrefabs(std::string const& monolith);
		// Rewrites all entity prefab jsons, to be used if need to update format of all json files 
		void GlobalPrefabReload();

		// Reads all json files from a given directory
		// Stores the json keys in the map, with the filenames as accessors (Level1, PlayerPrefab, etc...)
		void LoadJsonFolder(std::unordered_map<std::string, size_t>& map, std::string const& path);

		// Obtain a json file from ResourceManager using a given key.
		json const& GetArchetypes(size_t key) const;
	};

}	// NightSnack

#endif // ENTITYFACTORY_H
/******************************************************************************
 * File:        PrefabManager.h
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan
 *
 * Description:
 * Defines PrefabManager class that stores component loaders that assist in
 * creation of entities from prefabs.
 *
 * Copyright � 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include <functional>
#include <json.hpp>
#include <string>
#include <unordered_map>
#include <iostream>

#include "Core/ECS/Coordinator.h"
#include "../Systems/StateMachine/StateBase.h"

namespace NightSnack {

	using ComponentLoader = std::function<void(Entity, json const&, Coordinator&)>;

	class PrefabManager {
	public:
		/************************************************************************/
		/*!
		\brief
		Constructor that calls Initialize() to register all component loaders and
		stores them in m_componentLoaders.
		They are mapped from string to component loader type ("Position" -> <Postion>).
		*/
		/************************************************************************/
		PrefabManager(Coordinator& coord) : m_coord{ coord } { Initialize(); }

		void Initialize() {
			RegisterComponentLoader<Transform>("Transform");
			RegisterComponentLoader<RigidBody>("RigidBody");
			RegisterComponentLoader<Collider>("Collider");

			RegisterComponentLoader<Sprite>("Sprite");
			RegisterComponentLoader<Color>("Color");
			RegisterComponentLoader<Renderable>("Renderable");
			RegisterComponentLoader<Animator>("Animator");
			RegisterComponentLoader<Font>("Font");
			RegisterComponentLoader<Camera>("Camera");
			RegisterComponentLoader<Audio>("Audio");

			RegisterComponentLoader<ButtonInfo>("ButtonInfo");

			RegisterComponentLoader<States>("States");
			RegisterComponentLoader<Script>("Script");
			RegisterComponentLoader<PlayerController>("PlayerController");
		}

		// Helper template function which adds component with data loaded from json;
		template <typename T>
		void RegisterComponentLoader(std::string const& name) {
			m_componentLoaders[name] = [](Entity e, json const& data, Coordinator& coord) {
				T component{};
				from_json(data, component);

				if (coord.HasComponent<T>(e))
					coord.ModifyComponent<T>(e) = component;

				else
					coord.AddComponent<T>(e, component);
				};
		}

		/************************************************************************/
		/*!
		\brief
		Create entity with prefabname and json of archetypes
		e.g: CreateFromPrefab("Player", archetypes)
		Adds all components of prefab to created entity with the usage of component
		loaders.

		\param prefabName
		Name of desired prefab.
		\param archetypes
		Container of prefabs.

		\return
		ID of created entity.
		*/
		/************************************************************************/
		Entity CreateFromPrefab(json const& prefab) {
			// Create a new entity using coordinator;
			Entity e = m_coord.CreateEntity();
			//std::cerr << "Created Entity: " << e << '\n';

			if (e == INVALID_ENTITY) { return e; }

			m_coord.AddComponent<Transform>(e, Transform{});	// All entities should have a transform;
			//std::cerr << "Added Transform to Entity\n";

			// Adds all components from prefab definition;
			for (auto& [compName, compData] : prefab.items()) {
				// Look for component loader with std::string component name;
				auto it = m_componentLoaders.find(compName);
				// If found, use lambda function to insert data;
				if (it != m_componentLoaders.end()) {
					it->second(e, compData, m_coord);
					//std::cerr << "Added Component [" << compName << "] to Entity: " << e << '\n';

				}
				else {
					std::cerr << "Unknown component: " << compName << "\n";
				}
			}
			return e;
		}

		void UpdateComponents(Entity e, json const& prefab) {
			// Update all components from prefab definition;
			for (auto& [compName, compData] : prefab.items()) {
				// Look for component loader with std::string component name;
				auto it = m_componentLoaders.find(compName);
				// If found, use lambda function to insert data;
				if (it != m_componentLoaders.end()) {
					it->second(e, compData, m_coord);
					std::cerr << "Updated Component [" << compName << "] to Entity: " << e << '\n';
					for (auto c : compData) {
						std::cerr << c << "\n";
					}
					std::cerr << "\n";
				}
				else {
					std::cerr << "Unknown component: " << compName << "\n";
				}
			}
		}

	private:
		Coordinator& m_coord;
		std::unordered_map<std::string, ComponentLoader> m_componentLoaders;
	};

}	// NightSnack

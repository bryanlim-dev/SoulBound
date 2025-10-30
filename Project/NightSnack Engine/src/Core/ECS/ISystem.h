/******************************************************************************
 * File:        ISystem.h
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan
 *
 * Description:
 * Defines two classes: Base system class "ISystem" and SystemManager.
 * 
 * SystemManager handles all operations relating to Systems in the ECS core.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#ifndef ISYSTEM_HPP
#define ISYSTEM_HPP

#include <set>				// ISystem -> Entities
#include <unordered_map>	// SystemManger -> Signatures, Systems
#include <map>
#include <memory>

#include "Entity.h"
#include "Layer.h"
#include "Component.h"

namespace NightSnack {

	// Forward declaration of Coordinator class - Do not remove;
	class Coordinator;

	class ISystem {

	public:

		virtual std::set<Entity>& entities() { return m_entities; }
		// Every system must define an Update function;
		virtual void Update(int steps) = 0;
		// Every system must expose its signature;
		virtual Signature GetSignature() const = 0;
		// Retrieve name of system
		virtual std::string GetName() const = 0;

		// Layer Interface;
		virtual void AddLayer(Layer&&);
		virtual void RemoveLayer(LayerID id);

		virtual void AddToLayer(Entity e, LayerType type);
		virtual void RemoveFromLayers(Entity e);

	public:
		// All entities that the system can access;
		std::set<Entity> m_entities;
		// Entities separated and sorted into multimap of Layers;
		std::vector<Layer> m_layers;

		std::unordered_map<LayerID, size_t> m_idToIndex;
		std::unordered_map<size_t, LayerID> m_indexToId;

	};

	class SystemManager {

	public:
		SystemManager(Coordinator& coord) : m_coord{ coord } {}

		/************************************************************************/
		/*!
		\brief
		Constructs a system with args and inserts it and its signature into
		m_systems and m_signatures respectively.

		\return
		Returns a shared_ptr to the constructed system.
		*/
		/************************************************************************/
		template <typename T, typename... Args>
		std::shared_ptr<T> RegisterSystem(Args&&... args);

		/************************************************************************/
		/*!
		\brief
		Iterates through all systems and remove Entity e from their set of
		m_entities (the entities that they work on).

		\param e
		Entity
		*/
		/************************************************************************/
		void EntityDestroyed(Entity e);

		/************************************************************************/
		/*!
		\brief
		Updates all systems' m_entities after an Entity's signature has been
		changed (systems should only work on entities that have all required components).

		\param e
		Entity
		\param entitySig
		Signature of entity e.
		*/
		/************************************************************************/
		void EntitySignatureChanged(Entity e, Signature entitySig, LayerType l);

		/************************************************************************/
		/*!
		\brief
		Obtains the signature of an individual system.

		\return
		Signature of system.
		*/
		/************************************************************************/
		template <typename T>
		Signature GetSystemSignature();

		/************************************************************************/
		/*!
		\brief
		Obtains a shared pointer to a system of type T.
		*/
		/************************************************************************/
		template <typename T>
		std::shared_ptr<T> GetSystem();

		template <typename T>
		std::string GetSystemName() { 
			// Obtain type index for mapping;
			SystemID typeIdx = GetSystemID<T>();
			return static_cast<T>(m_systems[typeIdx]).GetName();
		}

	private:
		std::unordered_map<SystemID, Signature> m_signatures;
		std::unordered_map<SystemID, std::shared_ptr<ISystem>> m_systems;

		Coordinator& m_coord;
	};

	// HELPER FUNCTIONS  -----------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------------
	// Get Unique ID for System upon Registering System;
	// Inline is to prevent multiple definition error, as this function is defined
	// in a header file;
	inline SystemID GetUniqueSystemID() {

		// Static counter that lasts for the entire program lifetime;
		static SystemID lastID = 0u;
		// Return ID then increment for next call;
		return lastID++;

	}

	// Obtain SystemID for a specific System of type T;
	template <typename T>
	inline SystemID GetSystemID() noexcept {

		// static: typeID is only created once, so T System will maintain the same ID
		// for the program lifetime;
		static SystemID typeID = GetUniqueSystemID();
		return typeID;
	}


	// ISYSTEM ---------------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------------
	inline void ISystem::AddLayer(Layer&& l) {
		m_layers.emplace_back(l);
		std::sort(m_layers.begin(), m_layers.end());

		// update mappings;
		for (size_t idx{}; idx < m_layers.size(); ++idx) {
			m_idToIndex[m_layers[idx].ID()] = idx;
			m_indexToId[idx] = m_layers[idx].ID();
		}
	}
	inline void ISystem::RemoveLayer(LayerID id) {
		// iterate through Layers to find matching ID to erase;
		for (std::vector<Layer>::iterator it = m_layers.begin(); it != m_layers.end(); ++it) {
			if (it->ID() == id) {
				m_layers.erase(it);
				return;
			}
		}
		// update mappings;
		for (size_t idx{}; idx < m_layers.size(); ++idx) {
			m_idToIndex[m_layers[idx].ID()] = idx;
			m_indexToId[idx] = m_layers[idx].ID();
		}
	}

	inline void ISystem::AddToLayer(Entity e, LayerType type) {
		for (Layer& l : m_layers) {
			if (l.Type() == type) {
				l.Insert(e);
				break;
			}
		}
	}
	inline void ISystem::RemoveFromLayers(Entity e) {
		//int cnt{};
		for (Layer& l : m_layers) {
			if (l.Has(e)) {
				l.Remove(e);
				//++cnt;
			}
		}
		//if (!cnt) { std::cerr << "No Layers in " << GetName() << " contained Entity: " << e << '\n'; }
	}


	// SYSTEM MANAGER --------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------------
	// Register a System of type T into the SystemManager;
	template <typename T, typename... Args>
	std::shared_ptr<T> SystemManager::RegisterSystem(Args&&... args) {

		// Obtain type index for mapping;
		SystemID typeIdx = GetSystemID<T>();
		// Shared pointer of type System;
		auto system = std::make_shared<T>(std::forward<Args>(args)...);
		// Insert system into Map of systems;
		m_systems[typeIdx] = system;

		// SetUp System Signature in Signatures map;
		m_signatures[typeIdx] = system->GetSignature();

		// Return a smart pointer to system registered;
		return system;

	}

	// If an Entity is destroy, this function's purpose is to remove the destroyed Entity
	// from all Systems;
	inline void SystemManager::EntityDestroyed(Entity e) {

		// Iterate through unordered Map of Systems, remove Entity e from set of 
		// Entities that the Systems can access, so they do not work on destroyed Entities;
		for (auto& pair : m_systems) {
			pair.second->entities().erase(e);
			pair.second->RemoveFromLayers(e);
		}

	}

	// In the event of an Entity's System Signature being changed, update every
	// Systems' set of Entities, adding and removing where necessary;
	inline void SystemManager::EntitySignatureChanged(Entity e, Signature entitySig, LayerType l) {

		// Iterate through unordered Map of Systems;
		for (auto const& pair : m_systems) {
			// Signature of System in current iteration;
			auto const& systemSig = m_signatures[pair.first];
			// Skip if System has no Signature;
			if (systemSig == 0) { continue; }

			// Pointer to System stored in unordered map, of current iteration;
			auto& system = pair.second;

			// If Entity contains all components matching with current System;
			if ((entitySig & systemSig) == systemSig) {
				// Insert Entity into System set of Entities;
				system->entities().insert(e);
				// Insert Entity into appropriate Layer;
				system->AddToLayer(e, l);
			}
			else {
				// Else remove Entity from set of Entities;
				system->entities().erase(e);
				// Remove Entity from all Layer;
				system->RemoveFromLayers(e);
			}
		}
	}

	template <typename T>
	Signature SystemManager::GetSystemSignature() {

		// Obtain type index for mapping;
		SystemID typeIdx = GetSystemID<T>();
		// Obtain Signature of System of type T in map;
		return m_signatures[typeIdx];
	}

	template <typename T>
	std::shared_ptr<T> SystemManager::GetSystem() {

		// Obtain type index for mapping;
		SystemID typeIdx = GetSystemID<T>();
		// Find pointer to desired System in map;
		return std::static_pointer_cast<T>(m_systems[typeIdx]);
	}

}	// NightSnack

#endif // ISYSTEM_HPP
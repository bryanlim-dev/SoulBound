/******************************************************************************
 * File:        EntityManager.cpp
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan
 *
 * Description:
 * Defines member functions of EntityManager class.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include <iostream>
#include <algorithm>

#include "EntityManager.h"

namespace NightSnack {

	// Constructor to initialize the EntityManager;
	EntityManager::EntityManager() {

		// Initialize the queue with all possible entity IDs;
		for (Entity e = 0; e < MAX_ENTITIES; ++e) {
			m_availableEntities.push(e);
		}
	}

	// Create a new Entity by obtaining an ID from the queue of available entities;
	Entity EntityManager::CreateEntity() {

		// Ensure there are available entities to create;
		if (m_livingEntityCount >= MAX_ENTITIES) {
			std::cerr << "Too many entities in existence, unable to create Entity.\n";
			return INVALID_ENTITY;
		}

		// Obtain an available Entity ID;
		Entity id = m_availableEntities.front();
		m_availableEntities.pop();
		m_livingEntities.insert(id);

		// Initialize Signature (No Componenets);
		m_signatures[id] = 0;

		// Update number of living entities;
		++m_livingEntityCount;

		return id;
	}

	// Destroy an Entity by returning its ID to the queue of available entities;
	void EntityManager::DestroyEntity(Entity e) {

		// Validate entity ID
		if (e == INVALID_ENTITY) { 
			std::cerr << "Error: Trying to destroy Invalid Entity.\n";
			return; 
		}
		if (e >= MAX_ENTITIES) {
			std::cerr << "Error: Invalid entity ID: " << e << ", unable to destroy Entity.\n";
			return;
		}

		// Clear Component signatures for destroyed Entity ID;
		m_signatures[e].reset();

		// Return the Entity ID to the pool of available entities;
		m_availableEntities.push(e);
		m_livingEntities.erase(e);
		--m_livingEntityCount;
	}

	// Set signature for an entity with bounds checking;
	void EntityManager::SetSignature(Entity e, Signature sig) {
		if (e >= MAX_ENTITIES) {
			std::cerr << "Entity ID out of range, unable to set Signature.\n";
			return;
		}
		m_signatures[e] = sig;
	}

	// Get signature for an entity with bounds checking;
	Signature& EntityManager::GetSignature(Entity e) {
		if (e >= MAX_ENTITIES) {
			std::cerr << "Entity ID out of range, unable to obtain Signature.\n";
			return m_signatures[MAX_ENTITIES];
		}
		return m_signatures[e];
	}

	void EntityManager::Reset() {

		m_livingEntities.clear();
		m_livingEntityCount = 0;

		while (!m_availableEntities.empty()) {
			m_availableEntities.pop(); 
		}

		for (Entity e = 0; e < MAX_ENTITIES; ++e) {
			m_availableEntities.push(e);
		}
		for (Signature& s : m_signatures) {
			s.reset();
		}
	}

	bool EntityManager::EntityIsAlive(Entity e) const {
		if (m_livingEntities.find(e) != m_livingEntities.end()) {
			return true;
		}
		return false;
	}

}	// NightSnack

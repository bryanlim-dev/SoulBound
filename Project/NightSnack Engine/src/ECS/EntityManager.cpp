#include <iostream>

#include "ECS/EntityManager.h"

// Constructor to initialize the EntityManager;
EntityManager::EntityManager() {

	// Initialize the queue with all possible entity IDs;
	for (Entity e = 0; e < MAX_ENTITIES; ++e) {
		availableEntities.push(e);
	}

}

// Create a new Entity by obtaining an ID from the queue of available entities;
Entity EntityManager::CreateEntity() {

	// Ensure there are available entities to create;
	if (livingEntityCount >= MAX_ENTITIES) {
		std::cerr << "Too many entities in existence, unable to create Entity.\n";
		return INVALID_ENTITY;
	}
	
	// Obtain an available Entity ID;
	Entity id = availableEntities.front();
	availableEntities.pop();
	livingEntities.insert(id);

	// Initialize Signature (No Componenets);
	Signatures[id] = 0;
	
	// Update number of living entities;
	++livingEntityCount;

	return id;
}

// Destroy an Entity by returning its ID to the queue of available entities;
void EntityManager::DestroyEntity(Entity e) {

	// Validate entity ID
	if (e >= MAX_ENTITIES) {
		std::cerr << "Error: Invalid entity ID: " << e << ", unable to destroy Entity.\n";
		return;
	}

	// Clear Component signatures for destroyed Entity ID;
	Signatures[e] = 0;

	// Return the Entity ID to the pool of available entities;
	availableEntities.push(e);
	livingEntities.erase(e);
	--livingEntityCount;

}

// Set signature for an entity with bounds checking;
void EntityManager::SetSignature(Entity e, Signature sig) {
	if (e >= MAX_ENTITIES) {
		std::cerr << "Entity ID out of range, unable to set Signature.\n";
		return;
	}
	Signatures[e] = sig;
}

// Get signature for an entity with bounds checking;
Signature& EntityManager::GetSignature(Entity e) {
	if (e >= MAX_ENTITIES) {
		std::cerr << "Entity ID out of range, unable to get Signature.\n";
	}
	return Signatures[e];
}
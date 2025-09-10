#include "ECS/Entity.h"
#include <iostream>

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
		std::cerr << "Too many entities in existence.";
	}
	
	// Obtain an available Entity ID;
	Entity id = availableEntities.front();
	availableEntities.pop();
	
	// Update number of living entities;
	++livingEntityCount;

	return id;
}

// Destroy an Entity by returning its ID to the queue of available entities;
void EntityManager::DestroyEntity(Entity e) {

	// Check if the Entity is valid;
	if (e >= MAX_ENTITIES) {
		std::cerr << "Entity out of range.";
		return;
	}

	// Return the Entity ID to the pool of available entities;
	availableEntities.push(e);
	--livingEntityCount;

}
#include "ECS/Coordinator.h"

void Coordinator::Initialize() {
	// Create ECS Managers;
	Entity_M = std::make_unique<EntityManager>();
	System_M = std::make_unique<SystemManager>();
	Component_M = std::make_unique<ComponentManager>();
}

//void Coordinator::Update(double deltaTime) {
//	// Update all Systems through System Manager;
//	for (auto& pair : System_M->Systems) {
//		pair.second->Update(deltaTime);
//	}
//}

// ------------------------------------------------------------------------------------------------
// ECS INTERFACE;
// ------------------------------------------------------------------------------------------------

Entity Coordinator::CreateEntity() {
	// Returns the ID of created Entity;
	return Entity_M->CreateEntity();
}

// Destroy an Entity, remove it from all Systems and ComponentArrays;
void Coordinator::DestroyEntity(Entity e) {
	// Remove from Live Entites;
	Entity_M->DestroyEntity(e);
	// Remove Entity from Components;
	Component_M->EntityDestroyed(e);
	// Remove Entity from all Systems;
	System_M->EntityDestroyed(e);
}
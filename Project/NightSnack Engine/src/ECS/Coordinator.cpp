#include "ECS/Coordinator.h"

void Coordinator::Initialize() {
	// Create ECS Managers;
	m_entityManager = std::make_unique<EntityManager>();
	m_systemManager = std::make_unique<SystemManager>();
	m_componentManager = std::make_unique<ComponentManager>();
}

//void Coordinator::Update(float deltaTime) {
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
	return m_entityManager->CreateEntity();
}

// Destroy an Entity, remove it from all Systems and ComponentArrays;
void Coordinator::DestroyEntity(Entity e) {
	// Remove from Live Entites;
	m_entityManager->DestroyEntity(e);
	// Remove Entity from Components;
	m_componentManager->EntityDestroyed(e);
	// Remove Entity from all Systems;
	m_systemManager->EntityDestroyed(e);
}
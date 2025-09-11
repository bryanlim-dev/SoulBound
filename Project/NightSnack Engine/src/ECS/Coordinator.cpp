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

// Register a Component into the ComponentManager;
template <typename T>
void Coordinator::RegisterComponent() {
	// Call ComponentManager's member function to register a component
	// of type T into its unordered Map of ComponentArrays; Keys are type_index;
	Component_M->RegisterComponent<T>();
}

// Add a Component of type T to and Entity, updates the Entity's signature,
// making sure to also inform the ComponentManager and SystemManager;
template <typename T>
void Coordinator::AddComponent(Entity e, T component) {
	// Add component
	Component_M->AddComponent(e, component);

	// Obtain the Entity's component Signature;
	Signature Sig = Entity_M->GetSignature();
	// Modify the Signature to contain specified Component of type T;
	Sig.set(GetComponentID<T>(), true);

	// Update the actual Signature of Entity e;
	Entity_M->SetSignature(e, Sig);
	// Inform the SystemManager that Entity e's Signature has changed;
	System_M->EntitySignatureChanged(e, Sig);

}

template <typename T>
void Coordinator::RemoveComponent(Entity e) {
	// Remove component;
	Component_M->RemoveComponent(e);

	// Obtain the Entity's component Signature;
	Signature Sig = Entity_M->GetSignature();
	// Modify the Signature to remove specified Component of type T;
	Sig.set(GetComponentID<T>(), false);

	// Update the actual Signature of Entity e;
	Entity_M->SetSignature(e, Sig);
	// Inform the SystemManager that Entity e's Signature has changed;
	System_M->EntitySignatureChanged(e, Sig);
}

template <typename T>
T& Coordinator::GetComponent(Entity e) {
	return Component_M->GetComponent<T>(e);
}

template<typename T>
ComponentType Coordinator::GetComponentType() {
	return Component_M->GetComponentType<T>();
}

template <typename T>
std::shared_ptr<T> Coordinator::RegisterSystem() {
	return System_M->RegisterSystem<T>();
}

template <typename T>
void Coordinator::SetSystemSignature(Signature Sig) {
	System_M->SetSignature(Sig);
}
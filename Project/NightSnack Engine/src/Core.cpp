#include "Core.h"

// ------------------------------------------------------------------------------------------------
// CORE
// ------------------------------------------------------------------------------------------------

void CoreEngine::Initialize() {
	// Empty for now...
}

void CoreEngine::Terminate() {
	// Empty for now...
}

void CoreEngine::Update(double deltaTime) {

	// Update all systems by calling member functions;
	for (unsigned i = 0; i < Systems.size(); ++i)
		Systems[i]->Update(deltaTime);

}

void CoreEngine::AddSystem(System* system) {

	// Add the system to the vector of systems;
	Systems.push_back(system);

}

void CoreEngine::RemoveSystem(System* system) {

	// Move the system to be removed to the end of the vector and erase it;
	Systems.erase(std::remove(Systems.begin(), Systems.end(), system), Systems.end());

}

// ------------------------------------------------------------------------------------------------
// ECS INTERFACE
// ------------------------------------------------------------------------------------------------

Entity CoreEngine::CreateEntity() {
	// Returns the ID of created Entity;
	return Entity_M.CreateEntity();
}

void CoreEngine::DestroyEntity(Entity e) {
	// Remove from Live Entites;
	Entity_M.DestroyEntity(e);
	// Remove Entity from all Systems;
	System_M.EntityDestroyed(e);
}

// Register a Component into the ComponentManager;
template <typename T>
void CoreEngine::RegisterComponent() {
	// Call ComponentManager's member function to register a component
	// of type T into its unordered Map of ComponentArrays; Keys are type_index;
	Component_M.RegisterComponent<T>();
}

// Add a Component of type T to and Entity, updates the Entity's signature,
// making sure to also inform the ComponentManager and SystemManager;
template <typename T>
void CoreEngine::AddComponent(Entity e, T component) {
	// Add component
	Component_M.AddComponent(e, component);

	// Obtain the Entity's component Signature;
	Signature Sig = Entity_M.GetSignature();
	// Modify the Signature to contain specified Component of type T;
	Sig.set(GetComponentID<T>(), true);

	// Update the actual Signature of Entity e;
	Entity_M.SetSignature(e, Sig);
	// Inform the SystemManager that Entity e's Signature has changed;
	System_M.EntitySignatureChanged(e, Sig);

}

template <typename T>
T& CoreEngine::GetComponent(Entity e) {
	return Component_M.GetComponent<T>(e);
}

template <typename T>
std::shared_ptr<T> CoreEngine::RegisterSystem() {
	return System_M.RegisterSystem<T>();
}

template <typename T>
void CoreEngine::SetSystemSignature(Signature Sig) {
	System_M.SetSignature(Sig);
}
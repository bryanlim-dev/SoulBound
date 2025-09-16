#ifndef COORD_HPP
#define COORD_HPP

#include "ECS/Entity.h"
#include "ECS/EntityManager.h"
#include "ECS/ComponentManager.h"
#include "ECS/ISystem.h"

// Coordinator manages the ECS Managers;
// Provides an interface that can access all of ECS Managers' functions and data;
class Coordinator {

public:
	void Initialize();
	//void Update(double deltaTime);
	
	// ECS Interface;
	Entity CreateEntity();
	void DestroyEntity(Entity e);

	template <typename T>
	void RegisterComponent();

	template <typename T>
	void AddComponent(Entity e, T component);
	template <typename T>
	void RemoveComponent(Entity e);

	template <typename T>
	T& GetComponent(Entity e);

	Signature& GetEntitySignature(Entity e) { return Entity_M->GetSignature(e); }

	std::set<Entity> const& GetLivingEntities() const { return Entity_M->GetLivingEntities(); }

	template<typename T>
	std::shared_ptr<T> RegisterSystem();

	template<typename T>
	void SetSystemSignature(Signature sig);

	size_t GetEntityCount() const { return Entity_M->GetEntityCount(); }

private:
	// ECS Managers;
	std::unique_ptr<EntityManager> Entity_M;
	std::unique_ptr<ComponentManager> Component_M;
	std::unique_ptr<SystemManager> System_M;
};

// ------------------------------------------------------------------------------------------------
// TEMPLATE FUNCTIONS
// ------------------------------------------------------------------------------------------------


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
	Signature Sig = Entity_M->GetSignature(e);
	// Modify the Signature to contain specified Component of type T;
	Sig.set(GetComponentTypeID<T>(), true);

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
	Sig.set(GetComponentTypeID<T>(), false);

	// Update the actual Signature of Entity e;
	Entity_M->SetSignature(e, Sig);
	// Inform the SystemManager that Entity e's Signature has changed;
	System_M->EntitySignatureChanged(e, Sig);
}

template <typename T>
T& Coordinator::GetComponent(Entity e) {
	return Component_M->GetComponent<T>(e);
}

template <typename T>
std::shared_ptr<T> Coordinator::RegisterSystem() {
	return System_M->RegisterSystem<T>();
}

template <typename T>
void Coordinator::SetSystemSignature(Signature Sig) {
	System_M->SetSignature<T>(Sig);
}

#endif // COORD_HPP
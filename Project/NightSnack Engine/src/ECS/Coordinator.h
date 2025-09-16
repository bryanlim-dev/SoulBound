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

	Signature& GetEntitySignature(Entity e) { return m_entityManager->GetSignature(e); }

	std::set<Entity> const& GetLivingEntities() const { return m_entityManager->GetLivingEntities(); }

	template<typename T>
	std::shared_ptr<T> RegisterSystem();

	template<typename T>
	void SetSystemSignature(Signature sig);

private:
	// ECS Managers;
	std::unique_ptr<EntityManager> m_entityManager;
	std::unique_ptr<ComponentManager> m_componentManager;
	std::unique_ptr<SystemManager> m_systemManager;
};

// ------------------------------------------------------------------------------------------------
// TEMPLATE FUNCTIONS
// ------------------------------------------------------------------------------------------------


// Register a Component into the ComponentManager;
template <typename T>
void Coordinator::RegisterComponent() {
	// Call ComponentManager's member function to register a component
	// of type T into its unordered Map of ComponentArrays; Keys are type_index;
	m_componentManager->RegisterComponent<T>();
}

// Add a Component of type T to and Entity, updates the Entity's signature,
// making sure to also inform the ComponentManager and SystemManager;
template <typename T>
void Coordinator::AddComponent(Entity e, T component) {
	// Add component
	m_componentManager->AddComponent(e, component);

	// Obtain the Entity's component Signature;
	Signature sig = m_entityManager->GetSignature(e);
	// Modify the Signature to contain specified Component of type T;
	sig.set(GetComponentTypeID<T>(), true);

	// Update the actual Signature of Entity e;
	m_entityManager->SetSignature(e, sig);
	// Inform the SystemManager that Entity e's Signature has changed;
	m_systemManager->EntitySignatureChanged(e, sig);

}

template <typename T>
void Coordinator::RemoveComponent(Entity e) {
	// Remove component;
	m_componentManager->RemoveComponent(e);

	// Obtain the Entity's component Signature;
	Signature sig = m_entityManager->GetSignature();
	// Modify the Signature to remove specified Component of type T;
	sig.set(GetComponentTypeID<T>(), false);

	// Update the actual Signature of Entity e;
	m_entityManager->SetSignature(e, sig);
	// Inform the SystemManager that Entity e's Signature has changed;
	m_systemManager->EntitySignatureChanged(e, sig);
}

template <typename T>
T& Coordinator::GetComponent(Entity e) {
	return m_componentManager->GetComponent<T>(e);
}

template <typename T>
std::shared_ptr<T> Coordinator::RegisterSystem() {
	return m_systemManager->RegisterSystem<T>();
}

template <typename T>
void Coordinator::SetSystemSignature(Signature sig) {
	m_systemManager->SetSignature<T>(sig);
}

#endif // COORD_HPP
/******************************************************************************
 * File:        Coordinator.h
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan (Primary, 100%)
 *
 * Description:
 * This file declares class Coordinator, an interface that assists in interacting
 * with the ECS core.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#ifndef COORD_HPP
#define COORD_HPP

#include "ECS/Entity.h"
#include "ECS/Component.h"
#include "ECS/EntityManager.h"
#include "ECS/ComponentManager.h"
#include "ECS/ISystem.h"
#include "./Systems/Application.h"

// Coordinator manages the ECS Managers;
// Provides an interface that can access all of ECS Managers' functions and data;
class Coordinator {

public:
	/************************************************************************/
	/*!
	\brief
	Creates EntityManager, ComponentManager and SystemManager.
	*/
	/************************************************************************/
	void Initialize();

	/************************************************************************/
	/*!
	\brief
	Creates an entity and returns its ID (type Entity).

	\return
	Entity ID.
	*/
	/************************************************************************/
	Entity CreateEntity();

	/************************************************************************/
	/*!
	\brief
	Destroys an entity and by calling upon functionality of individual managers,
	will update all background linking (entities to components etc...).
	*/
	/************************************************************************/
	void DestroyEntity(Entity e);

	/************************************************************************/
	/*!
	\brief
	Obtains a count of all living entities.

	\return
	Count of number of living entities.
	*/
	/************************************************************************/
	size_t GetEntityNum() { return m_entityManager->GetLivingEntities().size(); }	// number of living entities;

	/************************************************************************/
	/*!
	\brief
	Registers a component into the ECS core.
	*/
	/************************************************************************/
	template <typename T>
	void RegisterComponent();

	/************************************************************************/
	/*!
	\brief
	Adds a component to an entity.

	\param e
	Entity
	\param component
	Component desired to be added.
	*/
	/************************************************************************/
	template <typename T>
	void AddComponent(Entity e, T component);

	/************************************************************************/
	/*!
	\brief
	Remove a component from an entity.

	\param e
	Entity
	*/
	/************************************************************************/
	template <typename T>
	void RemoveComponent(Entity e);

	/************************************************************************/
	/*!
	\brief
	Checks whether an entity has a specific component added.

	\param e
	Entity

	\return
	Returns true if the entity has the specified component, false otherwise.
	*/
	/************************************************************************/
	template<typename T>
	bool HasComponent(Entity e) const;

	/************************************************************************/
	/*!
	\brief
	Obtains a const reference to a specific component of an entity.
	(Accessor)

	\param e
	Entity

	\return
	Const reference to desired component.
	*/
	/************************************************************************/
	template <typename T>
	T const& GetComponent(Entity e) const;

	/************************************************************************/
	/*!
	\brief
	Obtains a reference to a specific component of an entity.
	(Modifier)

	\param e
	Entity

	\return
	Reference to desired component.
	*/
	/************************************************************************/
	template <typename T>
	T& ModifyComponent(Entity e);
	// specialization for Renderable;
	template <>
	Renderable& ModifyComponent<Renderable>(Entity e);

	/************************************************************************/
	/*!
	\brief
	Obtains the signature of an entity.

	\param e
	Entity

	\return
	Reference to the Entity's signature.
	*/
	/************************************************************************/
	Signature& GetEntitySignature(Entity e) { return m_entityManager->GetSignature(e); }

	/************************************************************************/
	/*!
	\brief
	Obtains the set of living entities.

	\return
	Set of living entities.
	*/
	/************************************************************************/
	std::set<Entity> const& GetLivingEntities() const { return m_entityManager->GetLivingEntities(); }

	/************************************************************************/
	/*!
	\brief
	Registers a system into the core. Arguments are forwarded down till the
	individual constructors of each system.

	\param args
	May take any number of arguments, but must match the parameters of the
	actual system's constructor.

	\return
	Returns a shared_ptr to the system that was constructed.
	*/
	/************************************************************************/
	template <typename T, typename... Args>
	std::shared_ptr<T> RegisterSystem(Args&&... args);

	/************************************************************************/
	/*!
	\brief
	Obtains a shared_ptr of a specific system.

	\return
	shared_ptr of a specific system.
	*/
	/************************************************************************/
	template <typename T>
	std::shared_ptr<T> GetSystem() { return m_systemManager->GetSystem<T>(); }

	/************************************************************************/
	/*!
	\brief
	Obtains the height of the application window.
	*/
	/************************************************************************/
	int GetWindowHeight();
	/************************************************************************/
	/*!
	\brief
	Obtains the width of the application window.
	*/
	/************************************************************************/
	int GetWindowWidth();

private:
	// ECS Managers;
	std::unique_ptr<EntityManager> m_entityManager;
	std::unique_ptr<ComponentManager> m_componentManager;
	std::unique_ptr<SystemManager> m_systemManager;

	// Event Dispatcher;
	// EventDispatcher& m_dispather;
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
T const& Coordinator::GetComponent(Entity e) const {
	return m_componentManager->GetComponent<T>(e);
}

template <typename T>
bool Coordinator::HasComponent(Entity e) const {
	return m_componentManager->HasComponent<T>(e);
}

template <typename T>
T& Coordinator::ModifyComponent(Entity e) {

	return m_componentManager->GetComponent<T>(e);
}
template <>	// specialization for Renderable;
inline Renderable& Coordinator::ModifyComponent<Renderable>(Entity e) {

	auto& r = m_componentManager->GetComponent<Renderable>(e);
	// To enable GraphicsSystem to know that RenderCache must be updated
	r.wasModified = true;
	return r;
}

template <typename T, typename... Args>
std::shared_ptr<T> Coordinator::RegisterSystem(Args&&... args) {
	return m_systemManager->RegisterSystem<T>(std::forward<Args>(args)...);
}

#endif // COORD_HPP
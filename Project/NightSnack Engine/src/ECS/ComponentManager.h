/******************************************************************************
 * File:        ComponentManager.h
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan (Primary, 100%)
 *
 * Description:
 * This file defines 3 classes and their member functions.
 * - IComponentArray: Base class for all ComponentArrays.
 * 
 * - ComponentArray<T>: Templated class that stores an array of components and
 *		uses two unordered-maps that assist in mapping entities to components.
 * 
 * - ComponentManager: Manages all ComponentArrays, creates the ComponentArray
 *		when a new component is registered.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#ifndef COMPMAN_HPP
#define COMPMAN_HPP

#include <unordered_map>
#include <memory>
#include <cassert>
#include <string>

#include "ECS/Entity.h"

// ------------------------------------------------------------------------------------------------
// ComponentArray interface to allow storage of multiple different ComponentArrays in ComponentManager;
class IComponentArray {
public:
	/************************************************************************/
	/*!
	\brief
	Destructor can be default, as the ComponentManager stores ComponentArrays
	with smart pointers.
	*/
	/************************************************************************/
	virtual ~IComponentArray() = default;

	/************************************************************************/
	/*!
	\brief
	All ComponentArrays must update their unordered-maps when an entity is
	destroyed, to ensure no removed components are accessed.

	\param entity
	The entity that was destroyed.
	*/
	/************************************************************************/
	virtual void EntityDestroyed(Entity entity) = 0;
};

// Component Array class to store components of type T
// in a densely packed array for efficient access and iteration.
template <typename T>
class ComponentArray : public IComponentArray {

public:
	/************************************************************************/
	/*!
	\brief
	Inserts Entity e into maps and updates m_components at specific index with
	desired component.
	e.g: Insert<Positon>(Player);

	\param e
	Entity to add component to.

	\param component
	Desired component that has already been set.
	*/
	/************************************************************************/
	void Insert(Entity e, T const& component);

	/************************************************************************/
	/*!
	\brief
	When a component is removed from an entity, updates maps and Array
	accordingly.
	e.g: Remove<Positon>(Player);

	\param e
	The entity to remove component from.
	*/
	/************************************************************************/
	void Remove(Entity e);

	/************************************************************************/
	/*!
	\brief
	Checks whether an entity had been added with a component.
	e.g: HasComponent<Positon>(Player);

	\param e
	The entity to check.

	\return
	Returns true if the entity has the specific component, false otherwise.
	*/
	/************************************************************************/
	bool HasComponent(Entity e);

	/************************************************************************/
	/*!
	\brief
	Obtains a reference to a component for a specific entity.
	e.g: Get<Position>(Player);

	\param e
	The entity that owns the component.

	\return
	Reference to desired component.
	*/
	/************************************************************************/
	T& Get(Entity e);

	/************************************************************************/
	/*!
	\brief
	Update unordered-maps when an entity is destroyed, to ensure no removed 
	components are accessed.

	\param e
	The entity that was destroyed.
	*/
	/************************************************************************/
	void EntityDestroyed(Entity e) override;

private:
	// Actual storage of components.
	std::array<T, MAX_ENTITIES> m_components;

	// Helper maps that link entities to their specific index in m_components.
	std::unordered_map<Entity, size_t> m_entityToIndex;
	std::unordered_map<size_t, Entity> m_indexToEntity;

	// Number of live components.
	size_t m_size = 0;

};

// ------------------------------------------------------------------------------------------------
// Manager for all Component Arrays;
class ComponentManager {

public:
	/************************************************************************/
	/*!
	\brief
	Creates the ComponentArray, "registering" the component into the system.
	Inserts it into m_componentArrays with a key of ComponentType.
	e.g: RegisterComponent<Position>();
	*/
	/************************************************************************/
	template <typename T>
	void RegisterComponent();

	/************************************************************************/
	/*!
	\brief
	Calls ComponentArray's Insert() function to add a component to an entity.

	\param e
	Entity to be added to.

	\param component
	The desired settings of type Component.
	*/
	/************************************************************************/
	template <typename T>
	void AddComponent(Entity e, T const& component);

	/************************************************************************/
	/*!
	\brief
	Calls ComponentArray's member function to check if an entity has a specific
	component.

	\param e
	Entity to be checked.

	\return
	Returns True if e has the specified component, false otherwise.
	*/
	/************************************************************************/
	template <typename T>
	bool HasComponent(Entity e);

	/************************************************************************/
	/*!
	\brief
	Removes a component from an entity by calling ComponentArray's member
	function

	\param e
	Entity to remove component from.
	*/
	/************************************************************************/
	template <typename T>
	void RemoveComponent(Entity e);

	/************************************************************************/
	/*!
	\brief
	Obtains a reference of a specific component that belongs to a specific
	entity.

	\param e
	Entity that owns the component.
	*/
	/************************************************************************/
	template <typename T>
	T& GetComponent(Entity e);

	/************************************************************************/
	/*!
	\brief
	Obtains a shared_ptr to ComponentArray of specified type T;
	e.g: auto CompArrayPtr = GetComponentArray<Position>();

	\return
	shared_ptr to desired ComponentArray.
	*/
	/************************************************************************/
	template <typename T>
	std::shared_ptr<ComponentArray<T>> GetComponentArray();

	/************************************************************************/
	/*!
	\brief
	Alerts all ComponentArrays that an entity has been destroyed and that
	they need to update their mappings.

	\param e
	Entity that was destroyed.
	*/
	/************************************************************************/
	void EntityDestroyed(Entity e);

private:
	// Mapping of all Component Arrays of any type;
	std::unordered_map<ComponentType, std::shared_ptr<IComponentArray>> m_componentArrays;
};


// ------------------------------------------------------------------------------------------------
// HELPER FUNCTIONS
// ------------------------------------------------------------------------------------------------

// Get Unique ID for Component Type upon creation of ComponentArray;
// Inline is to prevent multiple definition error, as this function is defined
// in a header file;
inline ComponentType GetUniqueComponentTypeID() {

	// Static counter that lasts for the entire program lifetime;
	static ComponentType lastID = 0u;
	// Return ID then increment for next call;
	return lastID++;

}

// Obtain Component Type ID for a specific Component Type T;
template <typename T>
inline ComponentType GetComponentTypeID() noexcept {

	// static: typeID is only created once, so T component will maintain the same ID
	// for the program lifetime;
	static ComponentType typeID = GetUniqueComponentTypeID();
	return typeID;
}

// ------------------------------------------------------------------------------------------------
// COMPONENT ARRAY
// ------------------------------------------------------------------------------------------------

// Size starts at 0;
// When inserting Components,
// Map the location(size) of the Component to the Entity ID;
// Update size of the Array;
template <typename T>
void ComponentArray<T>::Insert(Entity e, T const& component) {

	// Mapping Entity ID to Component index -> Last index currently;
	m_entityToIndex[e] = m_size;
	m_indexToEntity[m_size] = e;

	// Insert specified Component Data at the end of the array;
	m_components[m_size] = component;

	// Update size;
	++m_size;

}

// Find the index of an Entity's Component;
// Replace it with the last Component of the array (to maintain a densely packed array);
// Update the mapping of the moved Component to point to its new index;
// Decrement the size of the Array.
template<typename T>
void ComponentArray<T>::Remove(Entity e) {

	// Ensure entity actually exists in this array
	auto it = m_entityToIndex.find(e);
	if (it == m_entityToIndex.end()) {
		std::cout << "WARNING: Tried to remove non-existent component "
			<< std::to_string(GetComponentTypeID<T>()) << " from entity " << e << std::endl;
		return;
	}

	if (m_size <= 0) { 
		std::cout << "ERROR: Remove called on empty ComponentArray! "
			<< "ComponentID=" << std::to_string(GetComponentTypeID<T>())
			<< " Entity=" << e << std::endl;
		return; 
	}

	//assert(m_size > 0 && "Removing from empty ComponentArray!\n");

	size_t indexOfRemoved = it->second;
	size_t indexOfLast = m_size - 1;

	if (indexOfRemoved != indexOfLast) {
		// Move the last component into the removed slot
		m_components[indexOfRemoved] = m_components[indexOfLast];

		// Update entity mappings
		Entity entityOfLast = m_indexToEntity[indexOfLast];
		m_entityToIndex[entityOfLast] = indexOfRemoved;
		m_indexToEntity[indexOfRemoved] = entityOfLast;
	}

	// Erase mappings for removed entity
	m_entityToIndex.erase(e);
	m_indexToEntity.erase(indexOfLast);

	--m_size;
}

template <typename T>
bool ComponentArray<T>::HasComponent(Entity e) {

	auto it = m_entityToIndex.find(e);
	if (it == m_entityToIndex.end()) {
		return false;
	}
	return true;

}

// Find the index of an Entity's Component and return a reference to it.
template <typename T>
T& ComponentArray<T>::Get(Entity e) {

	return m_components[m_entityToIndex[e]];

}

template <typename T>
void ComponentArray<T>::EntityDestroyed(Entity e) {

	// If destroyed Entity e is found inside the Array, remove it;
	if (m_entityToIndex.find(e) != m_entityToIndex.end()) {
		Remove(e);
	}

}

// ------------------------------------------------------------------------------------------------
// COMPONENT MANAGER
// ------------------------------------------------------------------------------------------------

template <typename T>
void ComponentManager::RegisterComponent() {

	// Obtain Component TypeID for mapping;
	ComponentType typeIdx = GetComponentTypeID<T>();
	// Map now contains -> ComponentType -8- Shared_ptr to ComponentArray;
	m_componentArrays[typeIdx] = std::make_shared<ComponentArray<T>>();

}

template <typename T>
void ComponentManager::AddComponent(Entity e, T const& component) {

	// Call member function of ComponentArray to insert Component;
	GetComponentArray<T>()->Insert(e, component);

}

template <typename T>
bool ComponentManager::HasComponent(Entity e) {
	return GetComponentArray<T>()->HasComponent(e);
}

template <typename T>
void ComponentManager::RemoveComponent(Entity e) {

	// Call member function of ComponentArray to remove Component;
	GetComponentArray<T>()->Remove(e);

}

template <typename T>
T& ComponentManager::GetComponent(Entity e) {

	// Call member function to obtain a reference to an Entity's Component of type T;
	return GetComponentArray<T>()->Get(e);

}

// Inline to prevent multiple definition error;
inline void ComponentManager::EntityDestroyed(Entity e) {

	// Remove Entity from all Component Arrays;
	for (auto& pair : m_componentArrays) {
		pair.second->EntityDestroyed(e);
	}

}

template <typename T>
std::shared_ptr<ComponentArray<T>> ComponentManager::GetComponentArray() {

	// Obtain Component TypeID to locate pointer to ComponentArray in Map;
	ComponentType typeIdx = GetComponentTypeID<T>();
	// Return a pointer to requested ComponentArray using smart pointer;
	return std::static_pointer_cast<ComponentArray<T>>(m_componentArrays[typeIdx]);

}

#endif // COMPMAN_HPP

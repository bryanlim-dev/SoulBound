#ifndef COMPMAN_HPP
#define COMPMAN_HPP

#include <unordered_map>
#include <memory>
#include <cassert>

#include "ECS/Entity.h"

// ComponentArray interface to allow storage of multiple different ComponentArrays in ComponentManager;
class IComponentArray {
public:
	virtual ~IComponentArray() = default;
	virtual void EntityDestroyed(Entity entity) = 0; // allows cleanup
};

// Component Array class to store components of type T
// in a densely packed array for efficient access and iteration.
template <typename T>
class ComponentArray : public IComponentArray {

public:
	void Insert(Entity e, T component);
	void Remove(Entity e);
	T& Get(Entity e);
	void EntityDestroyed(Entity e) override;

private:
	std::array<T, MAX_ENTITIES> components;
	std::unordered_map<Entity, size_t> entityToIndex;
	std::unordered_map<size_t, Entity> indexToEntity;
	size_t size = 0;

};

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

class ComponentManager {

public:
	template <typename T>
	void RegisterComponent();

	template <typename T>
	void AddComponent(Entity e, T component);

	template <typename T>
	void RemoveComponent(Entity e);

	template <typename T>
	T& GetComponent(Entity e);

	// Obtains a shared_ptr to ComponentArray of specified type T;
	// Use case: auto CompArrayPtr = GetComponentArray<Position>();
	template <typename T>
	std::shared_ptr<ComponentArray<T>> GetComponentArray();

	void EntityDestroyed(Entity e);

private:
	// Mapping of all Component Arrays of any type;
	std::unordered_map<ComponentType, std::shared_ptr<IComponentArray>> ComponentArrays;
};

// ------------------------------------------------------------------------------------------------
// COMPONENT ARRAY
// ------------------------------------------------------------------------------------------------

// Size starts at 0;
// When inserting Components,
// Map the location(size) of the Component to the Entity ID;
// Update size of the Array;
template <typename T>
void ComponentArray<T>::Insert(Entity e, T component) {

	// Mapping Entity ID to Component index -> Last index currently;
	entityToIndex[e] = size;
	indexToEntity[size] = e;

	// Insert specified Component Data at the end of the array;
	components[size] = component;

	// Update size;
	++size;

}

// Find the index of an Entity's Component;
// Replace it with the last Component of the array (to maintain a densely packed array);
// Update the mapping of the moved Component to point to its new index;
// Decrement the size of the Array.
template<typename T>
void ComponentArray<T>::Remove(Entity e) {

	// Find indices of Component to be removed and last Component in array;
	size_t index = entityToIndex[e];
	size_t lastIndex = size - 1;

	// Move the last component into the removed component's slot;
	components[index] = components[lastIndex];

	// Update mapping of moved components to new indices;
	Entity lastEntity = indexToEntity[lastIndex];
	entityToIndex[lastEntity] = index;
	indexToEntity[index] = lastEntity;

	// Erase mapping of removed Components;
	entityToIndex.erase(e);
	indexToEntity.erase(lastIndex);

	// Update size;
	--size;

}

// Find the index of an Entity's Component and return a reference to it.
template <typename T>
T& ComponentArray<T>::Get(Entity e) {

	return components[entityToIndex[e]];

}

template <typename T>
void ComponentArray<T>::EntityDestroyed(Entity e) {

	// If destroyed Entity e is found inside the Array, remove it;
	if (entityToIndex.find(e) != entityToIndex.end()) {
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
	ComponentArrays[typeIdx] = std::make_shared<ComponentArray<T>>();

}

template <typename T>
void ComponentManager::AddComponent(Entity e, T component) {

	// Call member function of ComponentArray to insert Component;
	GetComponentArray<T>()->Insert(e, component);

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

void ComponentManager::EntityDestroyed(Entity e) {

	// Remove Entity from all Component Arrays;
	for (auto& pair : ComponentArrays) {
		pair.second->EntityDestroyed(e);
	}

}

template <typename T>
std::shared_ptr<ComponentArray<T>> ComponentManager::GetComponentArray() {

	// Obtain Component TypeID to locate pointer to ComponentArray in Map;
	ComponentType typeIdx = GetComponentTypeID<T>();
	// Return a pointer to requested ComponentArray using smart pointer;
	return std::static_pointer_cast<ComponentArray<T>>(ComponentArrays[typeIdx]);

}

#endif // COMPMAN_HPP

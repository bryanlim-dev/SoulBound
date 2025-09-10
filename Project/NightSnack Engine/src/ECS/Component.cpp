#include "ECS/Component.h"

// Size starts at 0;
// When inserting Components,
// Map the location(size) of the Component to the Entity ID;
// Update size of the Array;
template <typename T>
void ComponentArray<T>::Insert(Entity e, T component) {

	// Mapping Entity ID to Component index;
	entityToIndex[e] = size;
	indexToEntity[size] = e;

	// Insert Component at the end of the array;
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

	// Update size;
	--size;

}

// Find the index of an Entity's Component and return a reference to it.
template<typename T>
T& ComponentArray<T>::Get(Entity e) {

	return components[entityToIndex[e]];

}
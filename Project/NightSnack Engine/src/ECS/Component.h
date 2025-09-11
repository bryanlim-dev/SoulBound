#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include "ECS/Types.h"

// Component Array class to store components of type T
// in a densely packed array for efficient access and iteration.
template <typename T>
class ComponentArray {

public:
  void Insert(Entity e, T component);
  void Remove(Entity e);
  T& Get(Entity e);

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

private:
	// Mapping of all Component Arrays of any type;
	std::unordered_map<std::type_index, std::shared_ptr<void>> ComponentArrays;

	template <typename T>
	std::shared_ptr<ComponentArray<T>> GetComponentArray();
};

// ================================================================================================
// COMPONENTS
// ================================================================================================
// Add new Components here if necessary;

struct Position {
  float x, y;
};

struct Velocity {
  float dx, dy;
};

struct Sprite {
  std::string TextureName;
};


#endif // COMPONENT_HPP

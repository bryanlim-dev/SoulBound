#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include "ECS/Entity.h"
#include <unordered_map>
#include <string>

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

// ================================================================================================
// COMPONENTS
// ================================================================================================
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

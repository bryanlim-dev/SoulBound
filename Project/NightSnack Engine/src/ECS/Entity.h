#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <cstdint>
#include <queue>

using Entity = std::uint32_t;

constexpr size_t MAX_ENTITIES = 5000;

class EntityManager {

public:
  EntityManager() {

  }

  Entity CreateEntity();
  void DestroyEntity(Entity e);

private:
  std::queue<Entity> availableEntities;

};

#endif // ENTITY_HPP

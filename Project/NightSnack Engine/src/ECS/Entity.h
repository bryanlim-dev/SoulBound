#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <cstdint>
#include <queue>

// Maximum number of entities in the system.
constexpr size_t MAX_ENTITIES = 5000;

// Entities are essentially just unique IDs.
using Entity = std::uint32_t;

// Entity Manager class to handle creation and destruction of entities.
class EntityManager {

public:
  EntityManager();

  Entity CreateEntity();
  void DestroyEntity(Entity e);

private:
  std::queue<Entity> availableEntities;
	size_t livingEntityCount = 0;

};

#endif // ENTITY_HPP

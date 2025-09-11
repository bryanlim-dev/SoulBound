#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <cstdint>
#include <queue>
#include "ECS/Types.h"


// Entity Manager class to handle creation and destruction of entities.
class EntityManager {

public:
  EntityManager();

  Entity CreateEntity();
  void DestroyEntity(Entity e);

  void SetSignature(Entity e, Signature sig) { Signatures.at(e) = sig; }
  Signature GetSignature(Entity e) { return Signatures.at(e); }

private:
  std::queue<Entity> availableEntities;
  size_t livingEntityCount = 0;
  std::array<Signature, MAX_ENTITIES> Signatures;

};

#endif // ENTITY_HPP

#ifndef ENTMAN_HPP
#define ENTMAN_HPP

#include <queue>
#include <array>

#include "Entity.h"

// Entity Manager class to handle creation and destruction of entities.
class EntityManager {

public:
	EntityManager();

	Entity CreateEntity();
	void DestroyEntity(Entity e);

	void SetSignature(Entity e, Signature sig);
	Signature& GetSignature(Entity e);

	size_t GetEntityCount() const { return livingEntityCount; }

private:
	std::queue<Entity> availableEntities;
	size_t livingEntityCount = 0;
	std::array<Signature, MAX_ENTITIES> Signatures;

};

#endif // ENTMAN_HPP
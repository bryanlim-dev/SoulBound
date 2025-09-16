#ifndef ENTMAN_HPP
#define ENTMAN_HPP

#include <queue>
#include <set>
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

	std::set<Entity> const& GetLivingEntities() const { return livingEntities; }

private:
	std::queue<Entity> availableEntities;
	std::set<Entity> livingEntities;

	size_t livingEntityCount = 0;
	std::array<Signature, MAX_ENTITIES> Signatures;

};

#endif // ENTMAN_HPP
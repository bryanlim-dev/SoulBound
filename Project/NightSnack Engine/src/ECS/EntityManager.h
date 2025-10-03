/******************************************************************************
 * File:        EntityManager.h
 * Project:     NightSnack
 * Author(s):
 *
 * Description:
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

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

	std::set<Entity> const& GetLivingEntities() const { return m_livingEntities; }

private:
	std::queue<Entity> m_availableEntities;
	std::set<Entity> m_livingEntities;

	size_t m_livingEntityCount = 0;
	std::array<Signature, MAX_ENTITIES> m_signatures;

};

#endif // ENTMAN_HPP
/******************************************************************************
 * File:        EntityManager.h
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan
 *
 * Description:
 * Declares EntityManager class that controls all operations relating to Entities
 * and their signatures.
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

namespace NightSnack {

	// Entity Manager class to handle creation and destruction of entities.
	class EntityManager {

	public:
		/************************************************************************/
		/*!
		\brief
		Default constructor, populates m_availableEntites with unique Entity IDs
		from 0 to MAX_ENTITIES.
		*/
		/************************************************************************/
		EntityManager();

		/************************************************************************/
		/*!
		\brief
		Creates an entity and returns its ID.
		*/
		/************************************************************************/
		Entity CreateEntity();

		/************************************************************************/
		/*!
		\brief
		Removes an entity from m_livingEntities and adds its ID back into
		m_availableEntities.

		/param e
		Entity to be destroyed.
		*/
		/************************************************************************/
		void DestroyEntity(Entity e);

		/************************************************************************/
		/*!
		\brief
		Updates the signature stored in m_signatures at index (Entity ID).

		/param e
		Entity
		/param
		New signature value.
		*/
		/************************************************************************/
		void SetSignature(Entity e, Signature sig);

		/************************************************************************/
		/*!
		\brief
		Obtains a reference to an entity's signature.

		\param e
		Entity

		\return
		Reference to the desired signature.
		*/
		/************************************************************************/
		Signature& GetSignature(Entity e);

		/************************************************************************/
		/*!
		\brief
		Returns the set of living entities.
		*/
		/************************************************************************/
		std::set<Entity> const& GetLivingEntities() const { return m_livingEntities; }

		size_t GetLivingEntityCount() const { return m_livingEntityCount; }

		bool EntityIsAlive(Entity e) const;

		void Reset();

	private:
		std::queue<Entity> m_availableEntities;
		std::set<Entity> m_livingEntities;

		size_t m_livingEntityCount = 0;
		std::array<Signature, MAX_ENTITIES + 1> m_signatures;

	};

}	// NightSnack

#endif // ENTMAN_HPP
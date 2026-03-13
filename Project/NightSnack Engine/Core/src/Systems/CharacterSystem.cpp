/******************************************************************************
 * File:        CharacterSystem.cpp
 * Project:     NightSnack
 * Author(s):	Clement Ang Yang Ying (Primary, 100%)
 *
 * Description:
 * This file defines the the class CharacterSystem and its functions, it
 * is a system that tracks and store CharacterProperties on entities and updates them as needed
 * 
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
#include "CharacterSystem.h"
#include "Physics/Physics.h"
#include "Physics/Collision.h"
#include "PlayerController.h"

namespace NightSnack {



	void CharacterSystem::Update()
	{
		UpdateEntityPropertyMap();
	}

	void CharacterSystem::UpdateEntityPropertyMap()
	{

		for (int prop = static_cast<int>(START_PROPERTIES); prop < static_cast<int>(MAX_PROPERTIES); prop++)
		{	
			m_entityPropertyArr[prop].clear();
		}

		Entity player = m_coord.GetSystem<PlayerControllerSystem>()->GetPlayer();

		Transform& playerTransform = m_coord.ModifyComponent<Transform>(player);
		bool playerHasCollider = m_coord.HasComponent<Collider>(player);


		for (Entity const& e : m_coord.GetLivingEntities())
		{
			if (m_coord.HasComponent<Character>(e))
			{
				std::vector<CharacterProperties>const& props = m_coord.ModifyComponent<Character>(e).m_properties;

				bool entityHasCollider = m_coord.HasComponent<Collider>(e);
				Transform& entityTransform = m_coord.ModifyComponent<Transform>(e);

				for (CharacterProperties const& property : props)
				{
					m_entityPropertyArr[property].push_back(e);

					if (property == CHECKPOINT)
					{
						if (entityHasCollider && playerHasCollider)
						{
							Collider& playerCol = m_coord.ModifyComponent<Collider>(player);
							Collider& entityCol = m_coord.ModifyComponent<Collider>(e);

							if (CheckEntityCollision(playerTransform.pos, playerCol, entityTransform.pos, entityCol) && m_dispatcher)
							{
								m_dispatcher->QueueEvent(std::make_unique<CollisionEvent>(player, e));
							}
						}
					}
				}
			}
		}
	}

	std::list<Entity> CharacterSystem::FindEntityWithPropertities(CharacterProperties propertities)
	{
		if (propertities < MAX_PROPERTIES && propertities > START_PROPERTIES)
			return m_entityPropertyArr[propertities];

		return std::list<Entity>();
	}
}
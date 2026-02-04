#include "CharacterSystem.h"
#include "Physics/Physics.h"
#include "Physics/Collision.h"

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
							int circleCount = 0;
							bool playerCircle = false;
							Collider& playerCol = m_coord.ModifyComponent<Collider>(player);
							Collider& entityCol = m_coord.ModifyComponent<Collider>(e);

							if (playerCol.shape == ShapeType::Circle)
							{
								circleCount++;
								playerCircle = true;
							}
							if (entityCol.shape == ShapeType::Circle)
								circleCount++;


							bool colliding = false;
							if (circleCount == 0) //Both AABB
								colliding = CheckAABBvsAABB(playerTransform.pos, playerCol, entityTransform.pos, entityCol);
							else if (circleCount == 1) //1 is circle 1 is AABB
							{
								if (playerCircle)
									colliding = CheckCirclevsAABB(playerTransform.pos, playerCol, entityTransform.pos, entityCol);
								else
									colliding = CheckCirclevsAABB(entityTransform.pos, entityCol, playerTransform.pos, playerCol);
							}
							else //Both is circle
								colliding = CheckCirclevsCircle(entityTransform.pos, entityCol, playerTransform.pos, playerCol);

							if (colliding && m_dispatcher)
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
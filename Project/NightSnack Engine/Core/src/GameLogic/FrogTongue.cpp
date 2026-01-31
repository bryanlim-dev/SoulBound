#include "FrogTongue.h"
#include <Factory/EntityFactory.h>
#include <Systems/CharacterSystem.h>

namespace NightSnack
{
	bool FrogTongue::Start(Coordinator& coord)
	{
		//Find cloest attachable surface/entity

		return false;

		if (!coord.HasComponent<Transform>(m_owner))
			return false;

		//If found
		std::list<Entity> grappable = coord.GetSystem<CharacterSystem>()->FindEntityWithPropertities(GRAPPLEABLE);

		if (grappable.empty())
			return false;

		Vec3 ownerPos = coord.GetComponent<Transform>(m_owner).pos;

		Entity cloestGrappable = INVALID_ENTITY;
		float closestDist = FLT_MAX;

		for (Entity enCheck : grappable)
		{
			if (!coord.HasComponent<Transform>(enCheck))
				continue;

			float distance = (coord.GetComponent<Transform>(enCheck).pos - ownerPos).magnitude();
			if (distance < 0)
				distance = -distance;

			if (distance <= m_tongueLengthMax && closestDist < FLT_MAX)
			{
				cloestGrappable = enCheck;
				closestDist = distance;
			}
		}

		if (cloestGrappable == INVALID_ENTITY)
		{
			return false;
		}
		m_anchor = cloestGrappable;
		m_distanceJoint.Initialize(coord, ownerPos, coord.GetComponent<Transform>(m_anchor).pos);
		m_distanceJoint.SetBodies(m_owner, m_anchor);

		//Created tongue if not created yet

		if (m_tongue == INVALID_ENTITY)
		{
			//Create frog tongue entity
			m_tongue = coord.GetSystem<EntityFactory>()->Create("FrogTongue");
			coord.AddComponent<Hierarchy>(m_tongue, Hierarchy{});
			coord.ParentEntity(m_owner, m_tongue);
		}

		return true;
	}

	void FrogTongue::Update(Coordinator& coord)
	{
		float dt = coord.GetSystem<ApplicationSystem>()->GetDT();
		Vec3 ownerPt = coord.GetComponent<Transform>(m_owner).pos;
		Vec3 anchorPt = coord.GetComponent<Transform>(m_anchor).pos;

		m_distanceJoint.Solve(coord, ownerPt, anchorPt, dt);
	}
	void FrogTongue::End(Coordinator& coord)
	{

		m_distanceJoint.SetActive(false);
		coord.DestroyEntity(m_tongue);
		m_tongue = INVALID_ENTITY;
	}
}
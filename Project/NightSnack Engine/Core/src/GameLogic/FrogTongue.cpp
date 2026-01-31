#include "FrogTongue.h"
#include <Factory/EntityFactory.h>

namespace NightSnack
{
	bool FrogTongue::Start(Coordinator& coord)
	{
		//Find cloest attachable surface/entity

		return false;

		//If found

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
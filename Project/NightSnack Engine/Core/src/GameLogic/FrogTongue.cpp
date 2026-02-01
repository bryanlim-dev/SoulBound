#include "FrogTongue.h"
#include <Factory/EntityFactory.h>
#include <Systems/CharacterSystem.h>

namespace NightSnack
{
	bool FrogTongue::Start(Coordinator& coord)
	{
		//Find cloest attachable surface/entity
		if (!coord.HasComponent<Transform>(m_owner))
			return false;

		//If found
		std::list<Entity> grappable = coord.GetSystem<CharacterSystem>()->FindEntityWithPropertities(GRAPPLEABLE);

		if (grappable.empty())
			return false;

		Vec3 ownerPos = coord.GetComponent<Transform>(m_owner).pos;

		Entity cloestGrappable = INVALID_ENTITY;
		float closestDist = FLT_MAX;

		
		//Check each entity for closeest grappable
		for (Entity enCheck : grappable)
		{
			if (!coord.HasComponent<Transform>(enCheck))
				continue;

			float distance = (coord.GetComponent<Transform>(enCheck).pos - ownerPos).magnitude();
			if (distance < 0)
				distance = -distance;

			if (distance <= m_tongueLengthMax && distance < closestDist)
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

		//std::cout << "[Frog Grappble] Closest Grappable: " + std::to_string(m_anchor) + ", distance: " + std::to_string(closestDist) + "!" << std::endl;

		//Created tongue if not created yet

		if (m_tongue == INVALID_ENTITY || !coord.EntityIsAlive(m_tongue))
		{
			//Create frog tongue entity
			m_tongue = coord.GetSystem<EntityFactory>()->Create("FrogTongue");
			if (!coord.HasComponent<Hierarchy>(m_tongue))
				coord.AddComponent<Hierarchy>(m_tongue, Hierarchy{});

			coord.ParentEntity(m_tongue, m_owner);
		}

		UpdateTonguePos(coord);

		return true;
	}

	bool FrogTongue::Update(Coordinator& coord)
	{
		float dt = coord.GetSystem<ApplicationSystem>()->GetDT();
		Vec3 ownerPt = coord.GetComponent<Transform>(m_owner).pos;
		Vec3 anchorPt = coord.GetComponent<Transform>(m_anchor).pos;

		if (DistanceToAnchor(ownerPt, anchorPt) >= m_tongueLengthMax)
			return false;

		m_distanceJoint.Solve(coord, ownerPt, anchorPt, dt);
		UpdateTonguePos(coord);

		return true;
	}

	void FrogTongue::End(Coordinator& coord)
	{

		m_distanceJoint.SetActive(false);
		coord.DestroyEntity(m_tongue);
		m_tongue = INVALID_ENTITY;
	}
	void FrogTongue::UpdateTonguePos(Coordinator& coord)
	{
		if (m_tongue == INVALID_ENTITY || !coord.HasComponent<Transform>(m_tongue) || !coord.HasComponent<Transform>(m_owner) || !coord.HasComponent<Transform>(m_anchor))
			return;

		Transform& trans = coord.ModifyComponent<Transform>(m_tongue);

		//==Set Scale==
		trans.scale.x = m_tongueWidth;

		Vec3 ownerPt = coord.GetComponent<Transform>(m_owner).pos;
		Vec3 anchorPt = coord.GetComponent<Transform>(m_anchor).pos;

		Vec3 dirVec = (anchorPt - ownerPt);

		trans.scale.y = DistanceToAnchor(ownerPt, anchorPt);

		//==Set Rotation==
		trans.rot = atan2(dirVec.y, dirVec.x) * (180.0 / PI) + 90;
		//std::cout << "[Frog Grappble] Tongue Rot: " + std::to_string(trans.rot) + "!" << std::endl;

		//==Set Transform==
		trans.pos = (ownerPt + anchorPt) / 2;
	}

	float FrogTongue::DistanceToAnchor(Vec3 ownerPos, Vec3 anchorPos)
	{
		Vec3 dirVec = (anchorPos - ownerPos);
		float distance = dirVec.magnitude();

		return distance >= 0 ? distance : -distance;
	}
}
/******************************************************************************
 * File:        FrogTongue.cpp
 * Project:     NightSnack
 * Author(s):	Clement Ang Yang Ying (Primary, 100%)
 *
 * Description:
 * This file defines the the class FrogTongue and its functions
 * When triggered it finds grappbleable points and connects the owner to that point
 * via DistanceJoint2D and draws a line/tongue between them
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "FrogTongue.h"
#include <Factory/EntityFactory.h>
#include <Systems/CharacterSystem.h>
#include <Systems/PlayerController.h>
#include "Bounce.h"

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
		Vec2 ownerScale = coord.GetComponent<Transform>(m_owner).scale;

		Entity cloestGrappable = INVALID_ENTITY;
		float closestDist = FLT_MAX;

		
		//Check each entity for closeest grappable
		for (Entity enCheck : grappable)
		{
			if (!coord.HasComponent<Transform>(enCheck))
				continue;

			Vec3 enPos = coord.GetComponent<Transform>(enCheck).pos;
			float distance = (enPos - ownerPos).magnitude();
			if (distance < 0)
				distance = -distance;

			if (distance <= m_tongueLengthMax && distance < closestDist)
			{
				//Grappble point CANNOT be above the entity
				//with some leway
				if (enPos.y + (ownerScale.y /2) >= ownerPos.y)
				{
					cloestGrappable = enCheck;
					closestDist = distance;
				}
			}
		}

		if (cloestGrappable == INVALID_ENTITY)
		{
			return false;
		}

		m_anchor = cloestGrappable;
		m_distanceJoint.SetAutoConfigure(true);
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

		m_distanceJoint.SetActive(true);
		m_distanceJoint.EnableSpring(true);
		m_distanceJoint.LockBodyB(true);
		UpdateTonguePos(coord);
		m_active = true;
		
		//If tongue ownwer is player
		if (m_owner == coord.GetSystem<PlayerControllerSystem>()->GetPlayer())
		{
			coord.GetSystem<PlayerControllerSystem>()->GetPlayerFlags().disableMovement = true;
		}

		return true;
	}

	bool FrogTongue::Update(Coordinator& coord)
	{
		float dt = coord.GetSystem<ApplicationSystem>()->GetDT();
		Vec3 ownerPt = coord.GetComponent<Transform>(m_owner).pos;
		Vec3 anchorPt = coord.GetComponent<Transform>(m_anchor).pos;

		//Removed max dist check its messing with the extend/retract
		//if (DistanceToAnchor(ownerPt, anchorPt) >= m_tongueLengthMax)
		//	return false;

		m_distanceJoint.Solve(coord, ownerPt, anchorPt, dt);
		UpdateTonguePos(coord);

		return true;
	}

	void FrogTongue::End(Coordinator& coord)
	{

		if (m_owner == coord.GetSystem<PlayerControllerSystem>()->GetPlayer())
		{
			coord.GetSystem<PlayerControllerSystem>()->GetPlayerFlags().disableMovement = false;
		}

		m_distanceJoint.SetActive(false);
		coord.DestroyEntity(m_tongue);
		m_tongue = INVALID_ENTITY;
		m_active = false;


	}
	void FrogTongue::SwingActivated(Coordinator& coord, bool isRight)
	{
		if (!m_active || m_owner != coord.GetSystem<PlayerControllerSystem>()->GetPlayer() || !coord.HasComponent<RigidBody>(m_owner))
			return;

		//coord.GetSystem<PlayerControllerSystem>()->GetPlayerFlags().
		//std::cout << "Swing!" << std::endl;

		if (isRight)
			m_distanceJoint.ApplySwingForce(coord, m_swingForce);
		else
			m_distanceJoint.ApplySwingForce(coord, -m_swingForce);

		UpdateTonguePos(coord);
	}

	void FrogTongue::ExtendRetractTongue(Coordinator& coord, bool isExtend)
	{
		if (!m_active || m_tongue == INVALID_ENTITY)
			return;

		float dt = coord.GetSystem<ApplicationSystem>()->GetDT();

		float newLength = m_distanceJoint.GetDistance();

		if (isExtend)
			newLength += (m_extendRate * dt);
		else
			newLength -= (m_extendRate * dt);

		//Make sure it dosnt over or undershoot
		newLength = std::clamp(newLength, 0.0f, m_tongueLengthMax);

		m_distanceJoint.SetDistance(newLength);

		UpdateTonguePos(coord);
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
		trans.rot = atan2(dirVec.y, dirVec.x) * (float)(180.0f / PI) + 90;
		//std::cout << "[Frog Grappble] Tongue Rot: " + std::to_string(trans.rot) + "!" << std::endl;

		//==Set Transform==
		trans.pos = (ownerPt + anchorPt) / 2;

		//std::cout << "Tongue Length: " << DistanceToAnchor(ownerPt, anchorPt) << std::endl;
	}

	void FrogTongue::UpdateOwner(Entity e) {
		m_owner = e;
	}



	float FrogTongue::DistanceToAnchor(Vec3 ownerPos, Vec3 anchorPos)
	{
		Vec3 dirVec = (anchorPos - ownerPos);
		float distance = dirVec.magnitude();

		return distance >= 0 ? distance : -distance;
	}
}
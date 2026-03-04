/******************************************************************************
 * File:        Bounce.cpp
 * Project:     NightSnack
 * Author(s):	Clement Ang Yang Ying (Primary, 100%)
 *
 * Description:
 * This file defines the class Bounce and its functions
 * When triggered it begins targeting power and when released it send the
 * entity flying and allows them to bounce off walls
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "Bounce.h"
#include <Factory/EntityFactory.h>
#include <Systems/CharacterSystem.h>
#include <Systems/PlayerController.h>

namespace NightSnack
{
	bool Bounce::Start(Coordinator& coord)
	{
		if (!coord.GetSystem<PlayerControllerSystem>()->GetPlayerFlags().isGrounded)
			return false;

		m_currState = BOUNCE_CHARGING;
		m_chargeTime = 0.0f;
		m_powerIncreasing = true;
		
		m_facingRight = true;
		m_launchAngle = 45.0f;

		m_active = true;
		m_bounceDelayTime = 0;
		m_ownerInitalRestitution = 0;

		if (coord.HasComponent<RigidBody>(m_owner))
		{
			m_ownerInitalRestitution = coord.GetComponent<RigidBody>(m_owner).restitution;
			coord.ModifyComponent<RigidBody>(m_owner).restitution = 1;
		}

		return true;
	}

	bool Bounce::Update(Coordinator& coord)
	{
		if (!m_active)
			return false;

		switch (m_currState)
		{
		case BOUNCE_CHARGING:
		{
			if (m_powerIncreasing)
			{
				m_chargeTime += coord.GetSystem<ApplicationSystem>()->GetDT();
				if (m_chargeTime >= m_maxChargeTime)
				{
					m_powerIncreasing = false;
					m_chargeTime = m_maxChargeTime;
				}
			}
			else
			{
				m_chargeTime -= coord.GetSystem<ApplicationSystem>()->GetDT();
				if (m_chargeTime <= 0)
				{
					m_powerIncreasing = true;
					m_chargeTime = 0;
				}
			}

			std::cout << "Bounce! Charge Time = " << m_chargeTime << "/" << m_maxChargeTime << std::endl;
			std::cout << "Bounce! Angle = " << m_launchAngle << std::endl;

			break;
		}
		case BOUNCE_RELEASED:
		{
			m_bounceDelayTime += coord.GetSystem<ApplicationSystem>()->GetDT();
			if (m_bounceDelayTime >= m_maxGroundedDelayTime)
			{
				Collider& col = coord.ModifyComponent<Collider>(m_owner);
				//Check if gounded, end bounce
				if (coord.GetComponent<Collider>(m_owner).isGrounded)
				{
					//Reset vel
					coord.ModifyComponent<RigidBody>(m_owner).vel = Vec2();
					coord.ModifyComponent<RigidBody>(m_owner).force.isActive = false;
					coord.ModifyComponent<RigidBody>(m_owner).restitution = m_ownerInitalRestitution;
					std::cout << "Grounded!" << std::endl;
					return false;
				}
				else if (col.colliding)
				{
					Entity other = INVALID_ENTITY;
					if (col.collidedGround != INVALID_ENTITY)
						other = col.collidedGround;
					else
						other = col.collidedSolid;

					Collided(coord, other);
					m_bounceDelayTime = 0;
				}
			}
			else
			{
				std::cout << "Bounce CD" << std::endl;
			}

			break;
		}
		}

		return true;
	}

	void Bounce::End(Coordinator& coord)
	{
		m_chargeTime = 0.0f;
		m_currState = BOUNCE_END;
		
		DeleteDisplay(coord);
	}

	void Bounce::UpdateOwner(Entity e) {
		m_owner = e;
	}

	void Bounce::SetFacingDirection(bool isFacingRight)
	{
		m_facingRight = isFacingRight;
	}

	void Bounce::Launch(Coordinator& coord)
	{
		if (m_currState != BOUNCE_CHARGING)
			return;

		std::cout << "Bounce! LAUNCH!" << std::endl;

		m_currState = BOUNCE_RELEASED;

		DeleteDisplay(coord);

		
		if (coord.HasComponent<RigidBody>(m_owner))
		{
			//Launch force based on how long its charged
			float launchPower = m_maxLaunchForce / m_maxChargeTime * m_chargeTime;

			int dir = m_facingRight ? 1 : -1;

			coord.ModifyComponent<RigidBody>(m_owner).vel.x += launchPower * cos(m_launchAngle * PI/180) * dir;
			coord.ModifyComponent<RigidBody>(m_owner).vel.y += launchPower * sin(m_launchAngle * PI/180);
			std::cout << "Bounce Launch! X Vel = " << coord.ModifyComponent<RigidBody>(m_owner).vel.x << ", " 
				<< "Y Vel = " << coord.ModifyComponent<RigidBody>(m_owner).vel.y << std::endl;

		}
	}

	void Bounce::ChangeAngle(Coordinator& coord, bool isAngleIncrease)
	{
		if (m_currState != BOUNCE_CHARGING)
			return;

		if (isAngleIncrease)
			m_launchAngle += m_angleChangeSpeed * coord.GetSystem<ApplicationSystem>()->GetDT();
		else
			m_launchAngle -= m_angleChangeSpeed * coord.GetSystem<ApplicationSystem>()->GetDT();

		m_launchAngle = std::clamp(m_launchAngle, 0.0f, m_maxLaunchAngle);
	}

	void Bounce::UpdateDisplays(Coordinator& coord)
	{
	}

	void Bounce::DeleteDisplay(Coordinator& coord)
	{
		if (m_targetLine != INVALID_ENTITY)
			coord.DestroyEntity(m_targetLine);

		if (m_chargeBar != INVALID_ENTITY)
			coord.DestroyEntity(m_chargeBar);

		m_targetLine = INVALID_ENTITY;
		m_chargeBar = INVALID_ENTITY;
	}
	void Bounce::Collided(Coordinator& coord, Entity other)
	{
		if (!m_active || other == INVALID_ENTITY)
			return;

		if (coord.HasComponent<Transform>(m_owner) &&
			coord.HasComponent<RigidBody>(m_owner) &&
			coord.HasComponent<Collider>(m_owner) &&
			coord.HasComponent<Transform>(other) &&
			coord.HasComponent<RigidBody>(other) &&
			coord.HasComponent<Collider>(other))
		{
			coord.ModifyComponent<RigidBody>(m_owner).force.isActive = true;

			coord.GetSystem<PhysicsSystem>()->TransferForce(
				coord.ModifyComponent<Transform>(m_owner),
				coord.ModifyComponent<Transform>(other),
				coord.ModifyComponent<RigidBody>(m_owner),
				coord.ModifyComponent<RigidBody>(other),
				coord.ModifyComponent<Collider>(m_owner),
				coord.ModifyComponent<Collider>(other)
			);
			Transform& tra = coord.ModifyComponent<Transform>(m_owner);
			Transform& traWall = coord.ModifyComponent<Transform>(other);
			RigidBody& bod = coord.ModifyComponent<RigidBody>(m_owner);
			std::cout << "BOING! Entity pos: " << tra.pos << ", Wall/Floor Pos: " << traWall.pos 
				<< ", Entity Vel: " << bod.vel << ", Entity Force Mag: " << bod.force.magnitude << ", Entity Force Dir: " << bod.force.direction << std::endl;
		}
	}
}
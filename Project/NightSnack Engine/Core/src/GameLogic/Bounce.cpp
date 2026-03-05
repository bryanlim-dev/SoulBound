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
			coord.ModifyComponent<RigidBody>(m_owner).vel = Vec2();
		}


		CreateDisplay(coord);

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

			UpdateDisplays(coord);

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
		if (!m_active || m_currState != BOUNCE_CHARGING)
			return;

		std::cout << "Bounce! LAUNCH!" << std::endl;

		m_currState = BOUNCE_RELEASED;

		DeleteDisplay(coord);

		
		if (coord.HasComponent<RigidBody>(m_owner))
		{
			//Launch force based on how long its charged
			float launchPower = m_maxLaunchForce / m_maxChargeTime * m_chargeTime;
			//float launchPower = m_maxLaunchForce; //For testing

			int dir = m_facingRight ? 1 : -1;

			coord.ModifyComponent<RigidBody>(m_owner).vel.x = launchPower * (float)cos(m_launchAngle * PI/180) * dir;
			coord.ModifyComponent<RigidBody>(m_owner).vel.y = launchPower * (float)sin(m_launchAngle * PI/180);
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

	void Bounce::CreateDisplay(Coordinator& coord)
	{
		for (int x = 0; x < m_guideDotnum; ++x)
		{
			Entity dot = coord.GetSystem<EntityFactory>()->Create(m_guideDotPrefab);
			Transform& dotTrans = coord.ModifyComponent<Transform>(dot);
			dotTrans.scale = Vec2(m_guideDotSize, m_guideDotSize);

			if (coord.HasComponent<Collider>(dot))
			{
				coord.ModifyComponent<Collider>(dot).width = m_guideDotSize;
				coord.ModifyComponent<Collider>(dot).height = m_guideDotSize;
				coord.ModifyComponent<Collider>(dot).radius = m_guideDotSize;
			}

			m_guideDots.push_back(dot);
		}

		if (m_guideDots.size() > 0)
			if (coord.HasComponent<Color>(m_guideDots[0]))
				m_guideDotAlpha = coord.GetComponent<Color>(m_guideDots[0]).a;

		//Create Power Bar
		m_powerBar = coord.GetSystem<EntityFactory>()->Create(m_guideDotPrefab);
		Transform& powerTrans = coord.ModifyComponent<Transform>(m_powerBar);
		powerTrans.scale = m_powerBarScale;
		powerTrans.pos = coord.GetComponent<Transform>(m_owner).pos + m_powerBarOffsetPos;

		UpdateDisplays(coord);
	}

	void Bounce::UpdateDisplays(Coordinator& coord)
	{
		float chargeRatio = m_chargeTime / m_maxChargeTime;
		//Update power bar
		//Change scale to x if horziontal bar
		float newScale = ((m_powerBarScale.y - m_minPowerBarScale) * chargeRatio) + m_minPowerBarScale; 
		coord.ModifyComponent<Transform>(m_powerBar).scale.y = newScale;
		coord.ModifyComponent<Transform>(m_powerBar).pos.y = (coord.GetComponent<Transform>(m_owner).pos + m_powerBarOffsetPos).y + (newScale/2);
		;


		//Update dots
		Vec3 playerPos = coord.GetComponent<Transform>(m_owner).pos;
		RigidBody& rb = coord.ModifyComponent<RigidBody>(m_owner);

		int dir = m_facingRight ? 1 : -1;

		float rad = m_launchAngle * PI / 180.0f;

		// Compute initial launch velocity (same as Launch())
		float launchPower = m_maxLaunchForce * chargeRatio;
		//float launchPower = m_maxLaunchForce ;

		Vec2 simVel;
		simVel.x = launchPower * cos(rad) * dir;
		simVel.y = launchPower * sin(rad);

		Vec2 simPos = { playerPos.x, playerPos.y };

		float dt = coord.GetSystem<ApplicationSystem>()->GetDT(); 
		Vec2 gravity = { 0.0f, coord.GetSystem<PhysicsSystem>()->GetGravity() * rb.gravitymult };
		
		Collider c1;
		c1 = coord.GetComponent<Collider>(m_owner);
		c1.width = m_guideDotSize;
		c1.height = m_guideDotSize;
		c1.radius = m_guideDotSize;

		for (int i = 0; i < m_guideDots.size(); ++i)
		{
			if (coord.HasComponent<Collider>(m_guideDots[i]))
			{
				c1 = coord.GetComponent<Collider>(m_guideDots[i]);
			}

			// Apply gravity
			simVel += gravity * dt;

			// Move
			simPos += simVel * dt;

			bool collided = false;

			for (Entity e : coord.GetLivingEntities())
			{
				if (e == INVALID_ENTITY)
					break;

				if (e == m_owner || !coord.HasComponent<Collider>(e) || coord.GetComponent<Collider>(e).type == ColliderType::Trigger) continue;

				Transform& t2 = coord.ModifyComponent<Transform>(e);
				Collider& c2 = coord.ModifyComponent<Collider>(e);


				Transform fakeT1;
				fakeT1.pos = Vec3(simPos.x, simPos.y, 0);

				Vec2 normal;
				float depth;


				if (coord.GetSystem<PhysicsSystem>()->CheckCollision(fakeT1, t2, c1, c2, normal, depth))
				{
					collided = true;
					break;

					//simPos += normal * (depth + 0.5f);
					//
					//Transform& dotTrans = coord.ModifyComponent<Transform>(m_guideDots[i]);
					//dotTrans.pos = Vec3(simPos.x, simPos.y, 11);
				}
			}

			if (collided)
			{
				// Hide remaining dots
				for (int j = i; j < m_guideDots.size(); ++j)
					if (coord.HasComponent<Color>(m_guideDots[j]))
					{
						coord.ModifyComponent<Color>(m_guideDots[j]).a = 0;
						coord.ModifyComponent<Transform>(m_guideDots[j]).pos.z = -1000;
					}

				break;
			}

			// Place normal dot
			Transform& dotTrans = coord.ModifyComponent<Transform>(m_guideDots[i]);
			dotTrans.pos = Vec3(simPos.x, simPos.y, 11);

			if (coord.HasComponent<Color>(m_guideDots[i]))
			{
				coord.ModifyComponent<Color>(m_guideDots[i]).a = m_guideDotAlpha;
				coord.ModifyComponent<Transform>(m_guideDots[i]).pos.z = 11;

			}
		}
	}

	void Bounce::DeleteDisplay(Coordinator& coord)
	{
		if (!m_guideDots.empty())
		{
			for (Entity dot : m_guideDots)
			{
				coord.DestroyEntity(dot);
			}

			m_guideDots.clear();
		}

		if (m_powerBar != INVALID_ENTITY)
			coord.DestroyEntity(m_powerBar);

		m_powerBar = INVALID_ENTITY;
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
/******************************************************************************
 * File:        Bounce.cpp
 * Project:     NightSnack
 * Author(s):	Clement Ang Yang Ying (Primary, 100%)
 *
 * Description:
 * This file defines the class Bounce and its functions
 * When triggered it begins charging power + shows an indicator and when released it send the
 * entity flying and allows them to bounce off walls
 *
 * Copyright � 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "Bounce.h"
#include <Factory/EntityFactory.h>
#include <Systems/CharacterSystem.h>
#include <Systems/PlayerController.h>
#include <Systems/AudioSystem.h>

namespace NightSnack
{
	bool Bounce::Start(Coordinator& coord)
	{
		if (!coord.GetSystem<PlayerControllerSystem>()->GetPlayerFlags().isGrounded)
			return false;

		m_bounceFlags = BounceFlags();

		m_currState = BOUNCE_CHARGING;
		m_chargeTime = 0.0f;
		m_powerIncreasing = true;
		
		m_facingRight = coord.GetSystem<PlayerControllerSystem>()->GetPlayerFlags().facingRight;

		if (m_facingRight)
			m_launchAngle = 45.0f;
		else
			m_launchAngle = 135.0f;

		m_active = true;
		m_bounceDelayTime = 0;
		m_ownerInitalRestitution = 0;

		m_currLaunchPower = m_currLaunchPower/2;
		m_currLaunchPower = std::clamp(m_currLaunchPower, m_minLaunchForce, m_maxLaunchForce);

		m_currVel0Time = 0;

		if (coord.HasComponent<RigidBody>(m_owner))
		{
			m_ownerInitalRestitution = coord.GetComponent<RigidBody>(m_owner).restitution;
			coord.ModifyComponent<RigidBody>(m_owner).restitution = 1;
			coord.ModifyComponent<RigidBody>(m_owner).vel = Vec2();
		}

		if (coord.HasComponent<Renderable>(m_owner))
		{
			coord.ModifyComponent<Renderable>(m_owner).materialName = "SnekAbility_4_1";
		}

		if (!m_createdDisplays)
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
			UpdateCharging(coord);

			break;
		}
		case BOUNCE_RELEASED:
		{
			//Failsafe incase player is stuck, if Y vel == 0 for too long ocnsidered grounded
			if (coord.GetComponent<RigidBody>(m_owner).vel.y == 0)
			{
				m_currVel0Time += coord.GetSystem<ApplicationSystem>()->GetDT();
				if (m_currVel0Time >= m_maxVel0Time)
				{
					//Reset vel
					coord.ModifyComponent<RigidBody>(m_owner).vel = Vec2();
					coord.ModifyComponent<RigidBody>(m_owner).force.isActive = false;
					coord.ModifyComponent<RigidBody>(m_owner).restitution = m_ownerInitalRestitution;
					return false;
				}
			}

			//std::cout << coord.GetComponent<Transform>(m_owner).pos << std::endl;
			m_bounceDelayTime += coord.GetSystem<ApplicationSystem>()->GetDT();
			if (m_bounceDelayTime >= m_maxGroundedDelayTime)
			{
				if (!coord.HasComponent<Transform>(m_owner) ||
					!coord.HasComponent<RigidBody>(m_owner) ||
					!coord.HasComponent<Collider>(m_owner))
				{
					break;
				}

				Collider& col = coord.ModifyComponent<Collider>(m_owner);
				//Check if gounded, end bounce
				if (coord.GetComponent<Collider>(m_owner).isGrounded)
				{
					//Reset vel
					coord.ModifyComponent<RigidBody>(m_owner).vel = Vec2();
					coord.ModifyComponent<RigidBody>(m_owner).force.isActive = false;
					coord.ModifyComponent<RigidBody>(m_owner).restitution = m_ownerInitalRestitution;
					//std::cout << "Grounded!" << std::endl;

					//std::cout << "Landed! Dots pos:" << std::endl;
					//
					//for (Entity dot : m_guideDots)
					//	std::cout << coord.GetComponent<Transform>(dot).pos << std::endl;

					// Snek land sound
					coord.GetSystem<AudioSystem>()->PlaySFXWithVolume("SnekLand", 0.55f);


					return false;
				}
				else if (col.colliding && (col.collidedGroundCount > 0 || col.collidedSolidCount > 0))
				{
					  for (Entity other : col.collidedEntities)
					{
						if (other == INVALID_ENTITY || !coord.HasComponent<Collider>(other))
							continue;

						Collided(coord, other);
					}

					m_bounceDelayTime = 0;
				}

				if (coord.HasComponent<RigidBody>(m_owner))
					coord.GetSystem<PlayerControllerSystem>()->GetPlayerFlags().facingRight = m_facingRight = coord.GetComponent<RigidBody>(m_owner).vel.x > 0 ? true : false;
			}
			else
			{
				//std::cout << "Bounce CD" << std::endl;
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
		
		HideDisplay(coord);

		if (coord.HasComponent<Renderable>(m_owner))
		{
			coord.ModifyComponent<Renderable>(m_owner).materialName = "SnekIdle_4_1";
		}
	}

	void Bounce::Reset(Coordinator& coord, bool deleteEntities)
	{
		if (m_createdDisplays && deleteEntities)
			DeleteDisplay(coord);

		m_createdDisplays = false;

		m_guideDots.clear();
		m_powerBar = INVALID_ENTITY;

		m_createdDisplays = false;
		m_active = false;
		m_currState = BOUNCE_END;
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

		//std::cout << "Bounce! LAUNCH!" << std::endl;

		m_currState = BOUNCE_RELEASED;

		HideDisplay(coord);
		
		if (coord.HasComponent<RigidBody>(m_owner))
		{
			//Launch force based on how long its charged
			float launchPower = GetLaunchPower();
			//float launchPower = m_maxLaunchForce; //For testing

			coord.ModifyComponent<RigidBody>(m_owner).vel.x = launchPower * (float)cos(m_launchAngle * PI/180);
			coord.ModifyComponent<RigidBody>(m_owner).vel.y = launchPower * (float)sin(m_launchAngle * PI/180);
			//std::cout << "Bounce Launch! X Vel = " << coord.ModifyComponent<RigidBody>(m_owner).vel.x << ", " 
			//	<< "Y Vel = " << coord.ModifyComponent<RigidBody>(m_owner).vel.y << std::endl;

		}

		// play Snek launch sound
		coord.GetSystem<AudioSystem>()->PlayRandomVariant("SnekLaunch", 2, 0.6f);


		if (coord.HasComponent<Renderable>(m_owner))
		{
			coord.ModifyComponent<Renderable>(m_owner).materialName = "SnekFall_4_1";
		}

		//std::cout << "Launch! Player Pos: " << std::endl;
	}

	void Bounce::ChangeAngle(Coordinator& coord, bool isAngleIncrease)
	{
		if (m_currState != BOUNCE_CHARGING)
			return;

		if (isAngleIncrease)
			m_launchAngle += m_angleChangeSpeed * coord.GetSystem<ApplicationSystem>()->GetDT();
		else
			m_launchAngle -= m_angleChangeSpeed * coord.GetSystem<ApplicationSystem>()->GetDT();

		//if (coord.HasComponent<Animator>(m_owner))
		//{
		//	if (m_launchAngle < 90)
		//		coord.ModifyComponent<Animator>(m_owner).playbackSpeed = 1.0f;
		//	else if(m_launchAngle > 90)
		//		coord.ModifyComponent<Animator>(m_owner).playbackSpeed = 2.0f;
		//}



		m_launchAngle = std::clamp(m_launchAngle, 0.0f, m_maxLaunchAngle);
	}

	void Bounce::ChangePower(Coordinator& coord, bool isPowerIncrease)
	{
		if (m_currState != BOUNCE_CHARGING)
			return;

		if (isPowerIncrease)
			m_currLaunchPower += m_launchChangeSpeed * coord.GetSystem<ApplicationSystem>()->GetDT();
		else
			m_currLaunchPower -= m_launchChangeSpeed * coord.GetSystem<ApplicationSystem>()->GetDT();

		m_currLaunchPower = std::clamp(m_currLaunchPower, m_minLaunchForce, m_maxLaunchForce);
	}

	void Bounce::UpdateCharging(Coordinator& coord)
	{
		// Play charge sound once when charging begins
		if (!m_bounceFlags.m_chargeSoundPlayed)
		{
			//coord.GetSystem<AudioSystem>()->PlaySFXWithVolume("SnekChargeUp", 0.6f);
			m_bounceFlags.m_chargeSoundPlayed = true;
		}

		if (m_bounceFlags.m_powerIncreasing && m_bounceFlags.m_powerDecreasing)
		{ 
			//Empty by design
		}
		else if (m_bounceFlags.m_powerIncreasing)
			this->ChangePower(coord, true);
		else if (m_bounceFlags.m_powerDecreasing)
			this->ChangePower(coord, false);

		if (m_bounceFlags.m_angleIncreasing && m_bounceFlags.m_angleDecreasing)
		{ 
			//Empty by design
		}
		else if (m_bounceFlags.m_angleIncreasing)
			this->ChangeAngle(coord, true);
		else if (m_bounceFlags.m_angleDecreasing)
			this->ChangeAngle(coord, false);

		//Remnant of when charging was based on time
		/*if (m_powerIncreasing)
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
		}*/

		UpdateDisplays(coord);

		//std::cout << "Bounce! Charge Time = " << m_chargeTime << "/" << m_maxChargeTime << std::endl;
		//std::cout << "Bounce! Angle = " << m_launchAngle << std::endl;
		//std::cout << "Bounce! Angle = " << m_launchAngle << std::endl;
	}

	float Bounce::GetLaunchPowerRatio()
	{
		return 1 / (m_maxLaunchForce - m_minLaunchForce) * (m_currLaunchPower - m_minLaunchForce);
	}

	float Bounce::GetLaunchPower()
	{
		return m_currLaunchPower;
		//return ((m_maxLaunchForce - m_minLaunchForce) * GetLaunchPowerRatio()) + m_minLaunchForce;
	}

	void Bounce::CreateDisplay(Coordinator& coord)
	{
		m_createdDisplays = true;

		Collider col = coord.GetComponent<Collider>(m_owner);

		for (int x = 0; x < m_guideDotnum; ++x)
		{
			Entity dot = coord.GetSystem<EntityFactory>()->Create(m_guideDotPrefab);
			Transform& dotTrans = coord.ModifyComponent<Transform>(dot);
			dotTrans.scale = Vec2(m_guideDotSize, m_guideDotSize);
			dotTrans.layer = LayerType::FOREGROUND;

			if (coord.HasComponent<Collider>(dot))
			{
				coord.ModifyComponent<Collider>(dot).width = col.width;
				coord.ModifyComponent<Collider>(dot).height = col.height;
				coord.ModifyComponent<Collider>(dot).radius = col.radius;
			}

			if (!coord.HasComponent<Color>(dot))
				coord.AddComponent(dot, Color());

			coord.ModifyComponent<Color>(dot).a = 0.5f;

			//coord.ModifyComponent<Renderable>(dot).materialName = "ColorMat";


			dotTrans.isActive = false;
			m_guideDots.push_back(dot);
		}

		if (m_guideDots.size() > 0)
			if (coord.HasComponent<Color>(m_guideDots[0]))
				m_guideDotAlpha = coord.GetComponent<Color>(m_guideDots[0]).a;

		//Create Power Bar
		m_powerBar = coord.GetSystem<EntityFactory>()->Create(m_powerBarPrefab);
		Transform& powerTrans = coord.ModifyComponent<Transform>(m_powerBar);
		powerTrans.scale = m_powerBarScale;
		powerTrans.pos = coord.GetComponent<Transform>(m_owner).pos + m_powerBarOffsetPos;
		powerTrans.isActive = false;
		powerTrans.layer = LayerType::FOREGROUND;

		//Create angle indicator
		m_angleIndicator = coord.GetSystem<EntityFactory>()->Create(m_angleIndicatorPrefab);
		Transform& angleTrans = coord.ModifyComponent<Transform>(m_angleIndicator);
		angleTrans.scale = m_angleIndicatorScale;
		angleTrans.pos = coord.GetComponent<Transform>(m_owner).pos;
		angleTrans.pos.z = powerTrans.pos.z - 0.1f;

		if (!coord.HasComponent<Color>(m_angleIndicator))
			coord.AddComponent(m_angleIndicator, Color());

		coord.ModifyComponent<Color>(m_angleIndicator).a = 0.5f;
	}

	void Bounce::UpdateDisplays(Coordinator& coord)
	{
		if (!coord.HasComponent<Transform>(m_owner) ||
			!coord.HasComponent<RigidBody>(m_owner))
			return;

		Transform& ownerTrans = coord.ModifyComponent<Transform>(m_owner);
		Transform& angleTrans = coord.ModifyComponent<Transform>(m_angleIndicator);

		//Convert to radians
		float rad = (float)((m_launchAngle) * PI / 180.f);

		//Forward direction
		Vec2 forward;
		forward.x = cos(rad);
		forward.y = sin(rad);

		//Normalize
		if (forward.magnitude() > 0.f)
			forward.normalize();

		//Pos
		//Offset in FRONT of player
		float offsetDist = m_angleIndicatorOffset;

		angleTrans.pos.x = ownerTrans.pos.x + forward.x * offsetDist;
		angleTrans.pos.y = ownerTrans.pos.y + forward.y * offsetDist;
		angleTrans.pos.z = ownerTrans.pos.x - 0.1f;

		//Rotation
		//Face same direction as launch
		angleTrans.rot = atan2(forward.y, forward.x) * (float)(180.f / PI);

		// If your sprite points UP by default, add +90
		//angleTrans.rot += 90.f;

		// --- ACTIVATE ---
		angleTrans.isActive = true;

		//Update power bar
		//Change scale to x if horziontal bar
		float newScale = ((m_powerBarScale.y - m_minPowerBarScale) * (1 - GetLaunchPowerRatio())) + m_minPowerBarScale;
		coord.ModifyComponent<Transform>(m_powerBar).scale.x = newScale;
		coord.ModifyComponent<Transform>(m_powerBar).isActive = true;
		coord.ModifyComponent<Transform>(m_powerBar).pos.y = (coord.GetComponent<Transform>(m_owner).pos + m_powerBarOffsetPos).y ;
		coord.ModifyComponent<Transform>(m_powerBar).pos.x = (coord.GetComponent<Transform>(m_owner).pos + m_powerBarOffsetPos).x + (newScale / 2);

		Transform const& playerTrans = coord.GetComponent<Transform>(m_owner);
		RigidBody const& rb = coord.GetComponent<RigidBody>(m_owner);

		Vec2 pos = { playerTrans.pos.x, playerTrans.pos.y };

		float launchPower = GetLaunchPower();

		Vec2 vel;
		vel.x = launchPower * (float)cos(m_launchAngle * PI / 180.f);
		vel.y = launchPower * (float)sin(m_launchAngle * PI / 180.f);

		float dt = 0.016f; //Use fixed dt for no jitter, if used normal when the dt blips the dots will wiggle alot
		float gravity = coord.GetSystem<PhysicsSystem>()->GetGravity();
		float drag = coord.GetSystem<PhysicsSystem>()->GetDrag();

		Collider const& playerCol = coord.GetComponent<Collider>(m_owner);
		

		Transform simTransform = playerTrans;

		bool bounced = false;



		for (size_t i = 0; i < m_guideDots.size(); i++)
		{
			if (coord.HasComponent<Color>(m_guideDots[i]))
				coord.ModifyComponent<Color>(m_guideDots[i]).a = 0.5f;

			Vec2 accel{ 0.f, 0.f };
			accel.y += gravity * rb.gravitymult;

			vel += accel * dt;
			vel.x *= (1.f - drag * dt);

			Vec2 nextPos = pos + vel * dt;

			simTransform.pos.x = nextPos.x;
			simTransform.pos.y = nextPos.y;

			for (Entity e : coord.GetLivingEntities())
			{
				if (e == m_owner || !coord.HasComponent<Collider>(e))
					continue;

				Collider const& otherCol = coord.GetComponent<Collider>(e);
				if (otherCol.type == ColliderType::Trigger)
					continue;

				Transform const& otherTrans = coord.GetComponent<Transform>(e);

				Vec2 normal;
				float depth;

				if (coord.GetSystem<PhysicsSystem>()->CheckCollision(
					simTransform, otherTrans, playerCol, otherCol, normal, depth))
				{
					bounced = true;
					break;
				}
			}

			pos += vel * dt;

			// show dot
			Transform& dotTrans = coord.ModifyComponent<Transform>(m_guideDots[i]);
			dotTrans.pos.x = pos.x;
			dotTrans.pos.y = pos.y;
			dotTrans.pos.z = 0.5f;
			dotTrans.isActive = true;

			// limit dots after bounce
			if (bounced)
			{
				for (size_t j = i; j < m_guideDots.size(); j++)
				{
					coord.ModifyComponent<Transform>(m_guideDots[j]).isActive = false;
				}
				break;
			}
		}
		
	}

	void Bounce::HideDisplay(Coordinator& coord)
	{
		if (!m_guideDots.empty())
		{
			for (Entity dot : m_guideDots)
			{
				if (dot != INVALID_ENTITY && coord.EntityIsAlive(dot))
					coord.ModifyComponent<Transform>(dot).isActive = false;
			}
		}

		if (m_powerBar != INVALID_ENTITY && coord.EntityIsAlive(m_powerBar))
			coord.ModifyComponent<Transform>(m_powerBar).isActive = false;

		if (m_angleIndicator != INVALID_ENTITY && coord.EntityIsAlive(m_angleIndicator))
			coord.ModifyComponent<Transform>(m_angleIndicator).isActive = false;
	}

	void Bounce::DeleteDisplay(Coordinator& coord)
	{
		if (!m_guideDots.empty())
		{
			for (Entity dot : m_guideDots)
			{
				if (coord.EntityIsAlive(dot))
					coord.DestroyEntity(dot);
			}

			m_guideDots.clear();
		}

		if (m_powerBar != INVALID_ENTITY && coord.EntityIsAlive(m_powerBar))
			coord.DestroyEntity(m_powerBar);

		m_powerBar = INVALID_ENTITY;

		if (m_angleIndicator != INVALID_ENTITY && coord.EntityIsAlive(m_angleIndicator))
			coord.DestroyEntity(m_angleIndicator);
		m_angleIndicator = INVALID_ENTITY;

	}

	void Bounce::Collided(Coordinator& coord, Entity other)
	{
		if (!m_active || other == INVALID_ENTITY)
			return;

		if (coord.HasComponent<Collider>(other)									&&
			coord.GetComponent<Collider>(other).type != ColliderType::Trigger	&&
			coord.HasComponent<Transform>(other)								&&
			coord.HasComponent<RigidBody>(other))
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
			// Snek bounce sound
			coord.GetSystem<AudioSystem>()->PlayRandomVariant("SnekBounce", 2, 0.6f);

			//Transform& tra = coord.ModifyComponent<Transform>(m_owner);
			//Transform& traWall = coord.ModifyComponent<Transform>(other);
			//RigidBody& bod = coord.ModifyComponent<RigidBody>(m_owner);
			//std::cout << "BOING! Entity pos: " << tra.pos << ", Wall/Floor Pos: " << traWall.pos 
			//	<< ", Entity Vel: " << bod.vel << ", Entity Force Mag: " << bod.force.magnitude << ", Entity Force Dir: " << bod.force.direction << std::endl;
		}
	}
}
/******************************************************************************
 * File:        Bounce.h
 * Project:     NightSnack
 * Author(s):	Clement Ang Yang Ying (Primary, 100%)
 *
 * Description:
 * This file declares the class Bounce and its functions
 * When triggered it begins targeting power and when released it send the
 * entity flying and allows them to bounce off walls
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
#pragma once
#include <MathLibrary/DistanceJoint2D.h>
#include "../Systems/Physics/Physics.h"

namespace NightSnack {
	class Bounce
	{
	private:

		enum BounceState
		{
			BOUNCE_CHARGING,
			BOUNCE_RELEASED,
			BOUNCE_END
		};

		bool m_active = false;

		BounceState m_currState = BOUNCE_END;

		Entity m_owner = INVALID_ENTITY;
		std::vector<Entity> m_guideDots;
		Entity m_powerBar = INVALID_ENTITY;

		std::string m_guideDotPrefab = "BounceGuideDot";
		const int m_guideDotnum = 50;
		const float m_distBetweenDots = 0.25;
		const float m_guideDotSize = 10;
		float m_guideDotAlpha = 1; //Updates from prefab when making dots

		std::string m_powerBarPrefab = "BouncePowerBar";
		const Vec2 m_powerBarScale = Vec2{ 25, 75 };
		const Vec3 m_powerBarOffsetPos = Vec3{ 25, 25, 12 };
		const float m_minPowerBarScale = 5;


		//Charge
		float m_chargeTime = 0.0f;
		const float m_maxChargeTime = 2.5f;
		
		//When charging is the power increasing or decreasing
		bool m_powerIncreasing = true;

		//In degrees
		float m_launchAngle = 45.0f;
		//In degrees
		const float m_maxLaunchAngle = 90.0f;
		//Also in degrees
		const float m_angleChangeSpeed = 75.0f;
		bool m_facingRight = true;

		const float m_maxLaunchForce = 2000.0f;
		const float m_minLaunchForce = 500.0f;
		//const float m_minVel = 20.0f;

		//How long until start checking for grounded during launch state
		float m_bounceDelayTime = 0;
		const float m_maxGroundedDelayTime = 0.15f;

		float m_ownerInitalRestitution = 0;

		bool m_createdDisplays = false;

	public:
		Bounce(){}

		void Initalize(Entity owner, Coordinator& coord, EventDispatcher& disp) { 
			m_owner = owner; 
			disp.Subscribe<CollisionEvent>([this, &coord](const CollisionEvent& event)
				{
					//If not active or either entity is 
					if (!m_active || !((Entity)event.entityA == this->m_owner || (Entity)event.entityB == m_owner))
						return;

					Entity other = INVALID_ENTITY;
					if ((Entity)(event.entityA) != m_owner)
						other = event.entityA;
					else
						other = event.entityB;

					Collided(coord, other);
				});

			disp.Subscribe<InputEvent>([this, &coord](const InputEvent& event)
				{
					if (!m_active)
						return;

					//Flip launch direction to left
					switch (event.inputAction) {
						case InputEvent::InputAction::PlayerMoveLeft:
							
							m_facingRight = false;
							break;
					//Flip launch direction to right
						case InputEvent::InputAction::PlayerMoveRight:
							m_facingRight = true;
							break;
					case InputEvent::InputAction::PlayerUp:
						if (event.pressed == InputEvent::InputActionState::Pressed ||
							event.pressed == InputEvent::InputActionState::Repeat) {
							this->ChangeAngle(coord, true);
						}
						break;
					case InputEvent::InputAction::PlayerDown:
						if (event.pressed == InputEvent::InputActionState::Pressed ||
							event.pressed == InputEvent::InputActionState::Repeat) {
							this->ChangeAngle(coord, false);
							//this->ExtendRetractTongue(coord, true);
						}
						break;
					case InputEvent::InputAction::PlayerJumpAbility:
						if (event.pressed == InputEvent::InputActionState::Released)
						{
							this->Launch(coord);
						}
						break;
					}
					


				});
		}
		
		bool Start(Coordinator& coord);
		//Update function for bounce
		//Returns false if bounce stopped
		bool Update(Coordinator& coord);
		void End(Coordinator& coord);

		void Reset(Coordinator& coord);

		void UpdateOwner(Entity e);

		void SetFacingDirection(bool isFacingRight);

		void Launch(Coordinator& coord);
		void ChangeAngle(Coordinator& coord, bool isAngleIncrease);


		bool IsActive() const {
			return m_active;
		}
		
	private:
		/************************************************************************/
		/*!
		\brief
		Calcluates the ratio of power stored to max power
		*/
		/************************************************************************/
		float GetLaunchPowerRatio();

		/************************************************************************/
		/*!
		\brief
		Calcluates the power of launch using power ratio and min power
		*/
		/************************************************************************/
		float GetLaunchPower();

		/************************************************************************/
		/*!
		\brief
		Creates the display guide dots and power bar entities

		/param coord
		Refrence to coordinator
		*/
		/************************************************************************/
		void CreateDisplay(Coordinator& coord);

		//Updates power bar and target line
		void UpdateDisplays(Coordinator& coord);

		/************************************************************************/
		/*!
		\brief
		Hides the power bar and guide dot entities by setting them to inactive

		/param coord
		Refrence to coordinator
		*/
		/************************************************************************/
		void HideDisplay(Coordinator& coord);

		/************************************************************************/
		/*!
		\brief
		Deletes the power bar and guide dot entities
		
		/param coord
		Refrence to coordinator
		*/
		/************************************************************************/
		void DeleteDisplay(Coordinator& coord);


		/************************************************************************/
		/*!
		\brief
		Performas logic to bounce the entity off the collided entity
		Also filters collided entity of type trigger out

		/param coord
		Refrence to coordinator

		/param collidedEntity
		Entity it collided with, if the collider is of type trigger it is ignored
		*/
		/************************************************************************/
		void Collided(Coordinator& coord, Entity collidedEntity);

	};
}
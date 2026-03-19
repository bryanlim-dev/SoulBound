/******************************************************************************
 * File:        Bounce.h
 * Project:     NightSnack
 * Author(s):	Clement Ang Yang Ying (Primary, 100%)
 *
 * Description:
 * This file declares the class Bounce and its functions
 * When triggered it begins charging power + shows an indicator and when released it send the
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
		const Vec2 m_powerBarScale = Vec2{ 75, 25 };
		const Vec3 m_powerBarOffsetPos = Vec3{ -25, -75, 12 };
		const float m_minPowerBarScale = 50;


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
		const float m_angleChangeSpeed = 150.0f;
		bool m_facingRight = true;

		const float m_maxLaunchForce = 2000.0f;
		const float m_minLaunchForce = 500.0f;
		const float m_launchChangeSpeed = 1500.0f;
		float m_currLaunchPower = m_maxLaunchForce / 2;
		//const float m_minVel = 20.0f;

		//How long until start checking for grounded during launch state
		float m_bounceDelayTime = 0;
		const float m_maxGroundedDelayTime = 0.15f;

		float m_ownerInitalRestitution = 0;

		bool m_createdDisplays = false;

		const float m_maxVel0Time = 0.5f; //Failsafe incase player is stuck, if Y vel == 0 for too long ocnsidered grounded
		float m_currVel0Time = 0; //Failsafe incase player is stuck, if Y vel == 0 for too long ocnsidered grounded

	public:
		Bounce(){}

		/************************************************************************/
		/*!
		\brief
		Called when creating the ability class
		Initalizes the owner and event calls/functions/subscriptions

		/param owner
		Entity ID of the owner

		/param coord
		Refernece to the coordinator
		
		/param disp
		Reference to the EventDispatcher to subscribe to
		*/
		/************************************************************************/
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
							if (m_facingRight)
								this->ChangePower(coord, false);
							else
								this->ChangePower(coord, true);
							break;
					//Flip launch direction to right
						case InputEvent::InputAction::PlayerMoveRight:
							if (m_facingRight)
								this->ChangePower(coord, true);
							else
								this->ChangePower(coord, false);
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

		/************************************************************************/
		/*!
		\brief
		Called before bounce ability starts
		Sets trackers to their default values

		/param coord
		Refernece to the coordinator
		*/
		/************************************************************************/
		bool Start(Coordinator& coord);

		/************************************************************************/
		/*!
		\brief
		Update function for bounce
		Returns false if bounce stopped

		/param coord
		Refernece to the coordinator
		*/
		/************************************************************************/
		bool Update(Coordinator& coord);
		/************************************************************************/
		/*!
		\brief
		Function that handles the end of the entire bounce sequence (i.e entity landed)
		Hides the indicator display

		/param coord
		Refernece to the coordinator
		*/
		/************************************************************************/
		void End(Coordinator& coord);

		/************************************************************************/
		/*!
		\brief
		Handles reseting of the ability to default state (i.e delete guiide entities)

		/param coord
		Refernece to the coordinator

		/param deleteEntities
		False if do not destory attached entities
		Defaulted to true
		(e.g for scene transition all objects are destoryed anyway)
		*/
		/************************************************************************/
		void Reset(Coordinator& coord, bool deleteEntities = true);

		/************************************************************************/
		/*!
		\brief
		Updates the entity id

		/param e
		ID of the owner
		*/
		/************************************************************************/
		void UpdateOwner(Entity e);

		/************************************************************************/
		/*!
		\brief
		Sets where the launch direction is

		/param isFacingRight
		True, Launch to the right
		False, Launch to the left
		*/
		/************************************************************************/
		void SetFacingDirection(bool isFacingRight);

		/************************************************************************/
		/*!
		\brief
		Launches the entity owner with the current angle and power

		/param coord
		Refrence to coordinator
		*/
		/************************************************************************/
		void Launch(Coordinator& coord);

		/************************************************************************/
		/*!
		\brief
		Changes the angle of the launch up to the limit

		/param coord
		Refrence to coordinator

		/param isAngleIncrease
		Is the angle change increasing or decreasing
		True for increasing, false for decreasing
		*/
		/************************************************************************/
		void ChangeAngle(Coordinator& coord, bool isAngleIncrease);

		/************************************************************************/
		/*!
		\brief
		Changes the power of the launch up to the limit

		/param coord
		Refrence to coordinator

		/param isPowerIncrease
		Is the pwoer change increasing or decreasing
		True for increasing, false for decreasing
		*/
		/************************************************************************/
		void ChangePower(Coordinator& coord, bool isPowerIncrease);


		bool IsActive() const {
			return m_active;
		}
		
	private:
		/************************************************************************/
		/*!
		\brief
		Calcluates the ratio of power stored to max power
		Ratio is from 0.0f - 1.0f

		\return
		Ratio of current launch power to max power from 0.0 - 1.0
		*/
		/************************************************************************/
		float GetLaunchPowerRatio();

		/************************************************************************/
		/*!
		\brief
		Returns the launch power

		\return
		Current Launch Power
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

		/************************************************************************/
		/*!
		\brief
		Updates the display guide dots and power bar entities

		/param coord
		Refrence to coordinator
		*/
		/************************************************************************/
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
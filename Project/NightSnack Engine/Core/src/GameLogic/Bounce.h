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

		BounceState m_currState = BOUNCE_END;

		Entity m_owner = INVALID_ENTITY;
		Entity m_targetLine = INVALID_ENTITY;
		Entity m_chargeBar = INVALID_ENTITY;

		bool m_active = false;

		float m_chargeTime = 0.0f;
		const float m_maxChargeTime = 2.5f;
		
		//When charging is the power increasing or decreasing
		bool m_powerIncreasing = true;

		//In degrees
		float m_launchAngle = 45.0f;
		//In degrees
		const float m_maxLaunchAngle = 90.0f;
		const float m_angleChangeSpeed = 25.0f;
		bool m_facingRight = true;

		const float m_maxLaunchForce = 2500.0f;
		const float m_minVel = 20.0f;

		//How long until start checking for grounded during launch state
		float m_bounceDelayTime = 0;
		const float m_maxGroundedDelayTime = 0.15f;

		float m_ownerInitalRestitution = 0;

	public:
		Bounce(){}

		void Initalize(Entity owner, Coordinator& coord, EventDispatcher& disp) { 
			m_owner = owner; 
			disp.Subscribe<CollisionEvent>([this, &coord](const CollisionEvent& event)
				{
					//If not active or either entity is 
					if (!m_active || !(event.entityA == this->m_owner || event.entityB == m_owner))
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
							if (event.pressed == InputEvent::InputActionState::Pressed ||
								event.pressed == InputEvent::InputActionState::Repeat) {
								//this->SwingActivated(coord, false);
								m_facingRight = false;
							}
							break;
					//Flip launch direction to right
						case InputEvent::InputAction::PlayerMoveRight:
							if (event.pressed == InputEvent::InputActionState::Pressed ||
								event.pressed == InputEvent::InputActionState::Repeat) {
								//this->SwingActivated(coord, true);
								m_facingRight = true;
							}
							break;
					case InputEvent::InputAction::PlayerJump:
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
					case InputEvent::InputAction::PlayerSpecialAbility:
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

		void UpdateOwner(Entity e);

		void SetFacingDirection(bool isFacingRight);

		void Launch(Coordinator& coord);
		void ChangeAngle(Coordinator& coord, bool isAngleIncrease);


		bool IsActive() const {
			return m_active;
		}
		
	private:
		//Updates power bar and target line
		void UpdateDisplays(Coordinator& coord);

		void DeleteDisplay(Coordinator& coord);

		void Collided(Coordinator& coord, Entity collidedEntity);

	};
}
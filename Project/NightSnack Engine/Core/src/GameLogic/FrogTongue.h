/******************************************************************************
 * File:        FrogTongue.h
 * Project:     NightSnack
 * Author(s):	Clement Ang Yang Ying (Primary, 100%)
 *
 * Description:
 * This file declares the the class FrogTongue and its functions
 * When triggered it finds grappbleable points and connects the owner to that point
 * via DistanceJoint2D and draws a line/tongue between them
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
#pragma once
#include <MathLibrary/DistanceJoint2D.h>
#include <Systems/ParticleSystem.h>

namespace NightSnack {
	class FrogTongue
	{
	private:
		Entity m_owner = INVALID_ENTITY;
		Entity m_anchor = INVALID_ENTITY;
		Entity m_tongue = INVALID_ENTITY;
		Entity m_indicator = INVALID_ENTITY;

		EmitterID m_indicatorEmitter{};
		bool m_emitterActive = false;

		DistanceJoint2D m_distanceJoint;

		const float m_tongueLengthMax = 550.0f;
		const float m_tongueWidth = 5.f;
		const float m_swingForce = 750.0f;

		bool m_active = false;

		float m_extendRate = 500.0f;

	public:
		FrogTongue(){}

		void Initalize(Entity owner, Coordinator& coord, EventDispatcher& disp) { 
			m_owner = owner; 
			disp.Subscribe<InputEvent>([this, &coord](const InputEvent& event)
				{
					if (!m_active)
						return;

					switch (event.inputAction) {
					case InputEvent::InputAction::PlayerMoveLeft:
						if (event.pressed == InputEvent::InputActionState::Pressed ||
							event.pressed == InputEvent::InputActionState::Repeat) {
							this->SwingActivated(coord, false);
						}
						break;
					case InputEvent::InputAction::PlayerMoveRight:
						if (event.pressed == InputEvent::InputActionState::Pressed ||
							event.pressed == InputEvent::InputActionState::Repeat) {
							this->SwingActivated(coord, true);
						}
						break;
					case InputEvent::InputAction::PlayerUp:
						if (event.pressed == InputEvent::InputActionState::Pressed ||
							event.pressed == InputEvent::InputActionState::Repeat) {
							this->ExtendRetractTongue(coord, false);
						}
						break;
					case InputEvent::InputAction::PlayerDown:
						if (event.pressed == InputEvent::InputActionState::Pressed ||
							event.pressed == InputEvent::InputActionState::Repeat) {
							this->ExtendRetractTongue(coord, true);
						}
						break;
					case InputEvent::InputAction::PlayerJumpAbility:
						if (event.pressed == InputEvent::InputActionState::Released)
						{

							this->End(coord);
						}
						break;
					}
					


				});
		}
		
		//Finds thed cloest attachtable surface/entity
		//Returns false if too far to attach or cant find
		bool Start(Coordinator& coord);
		//Update function for tongue
		//Returns false if ability stopped
		bool Update(Coordinator& coord);
		void End(Coordinator& coord);

		//Only supports player
		void SwingActivated(Coordinator& coord, bool isRight);

		//Extends the tongue
		void ExtendRetractTongue(Coordinator& coord, bool isExtend);

		// Updates the owner position to match the tongue anchor poin
		void UpdateOwner(Entity e);

		// Updates the indicator position and visibility
		void UpdateIndicator(Coordinator& coord);

	private:

		void UpdateTonguePos(Coordinator& coord);
		float DistanceToAnchor(Vec3 ownerPos, Vec3 anchorPos);

		// Spawn and Destroy methods for indicator
		void SpawnIndicator(Coordinator& coord);
		void DestroyIndicator(Coordinator& coord);

	};
}
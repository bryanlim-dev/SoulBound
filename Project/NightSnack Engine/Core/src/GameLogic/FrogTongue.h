/******************************************************************************
 * File:        FrogTongue.h
 * Project:     NightSnack
 * Author(s):	Clement Ang Yang Ying (Primary, 80%)
 *				Wei Jie Lim	(Secondary, 20%) - Added indicator functions for grappable points
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
		

		/************************************************************************/
		/*!
		\brief
		Called before grapple ability starts
		Finds thed cloest attachtable surface/entity
		Creates the tongue entity object if not created
		Returns false if too far to attach or cant find

		/param coord
		Refernece to the coordinator

		/return
		True - Grapple actiavted
		False - Grapple not activated (usually due to no attach points)
		*/
		/************************************************************************/
		bool Start(Coordinator& coord);
		//Update function for tongue
		//Returns false if ability stopped

		/************************************************************************/
		/*!
		\brief
		Called during update when grappling ability is active

		/param coord
		Refernece to the coordinator

		/return
		True - Grapple is still active
		False - Grapple deactivated
		*/
		/************************************************************************/
		bool Update(Coordinator& coord);
		/************************************************************************/
		/*!
		\brief
		Function that handles the end of the grapple ability
		Hides the tongue entity

		/param coord
		Refernece to the coordinator
		*/
		/************************************************************************/
		void End(Coordinator& coord);

		/************************************************************************/
		/*!
		\brief
		Handles reseting of the ability to default state (i.e delete tongue entity)

		/param coord
		Refernece to the coordinator

		/param deleteEntities
		False if do not destory attached entities
		Defaulted to true
		(e.g for scene transition all objects are destoryed anyway)
		*/
		/************************************************************************/
		void Reset(Coordinator& coord, bool deleteEntities = true);

		//Only supports player
		/************************************************************************/
		/*!
		\brief
		Applies a swing for to the entity
		Currently only works for the main player entity

		/param coord
		Refernece to the coordinator

		/param isRight
		True - Swinging to the right
		False - Swinging to the left
		*/
		/************************************************************************/
		void SwingActivated(Coordinator& coord, bool isRight);

		/************************************************************************/
		/*!
		\brief
		Extends or retracts the tongue/distance between the entity and the anchor

		/param coord
		Refernece to the coordinator

		/param isExtend
		True - Extending distance
		False - Retracting distance
		*/
		/************************************************************************/
		void ExtendRetractTongue(Coordinator& coord, bool isExtend);

		/************************************************************************/
		/*!
		\brief
		Updates the owner position to match the tongue anchor poin

		/param coord
		Refernece to the coordinator
		*/
		/************************************************************************/
		void UpdateOwner(Entity e);

		/************************************************************************/
		/*!
		\brief
		Updates the indicator particles position and visibility

		/param coord
		Refernece to the coordinator
		*/
		/************************************************************************/
		void UpdateIndicator(Coordinator& coord);

		void Reset();

	private:

		/************************************************************************/
		/*!
		\brief
		Updates the tongue entity that is between the anchor and entity

		/param coord
		Refernece to the coordinator
		*/
		/************************************************************************/
		void UpdateTonguePos(Coordinator& coord);

		/************************************************************************/
		/*!
		\brief
		Calculates and returns the distance between the 2 vectors, returns distance oonly in positive numbers
		Usually for the owner and anchor

		/param ownerPos
		Vec3 of the owner pos

		/param anchorPos
		Vec3 of the anchor pos

		/return
		Magnitude distance of the vectors, but only in positive numbers
		*/
		/************************************************************************/
		float DistanceToAnchor(Vec3 ownerPos, Vec3 anchorPos);


		/************************************************************************/
		/*!
		\brief
		Hides the tongue entity by setting them to inactive

		/param coord
		Refrence to coordinator
		*/
		/************************************************************************/
		void HideDisplay(Coordinator& coord);

		/************************************************************************/
		/*!
		\brief
		Deletes the tongue entity

		/param coord
		Refrence to coordinator
		*/
		/************************************************************************/
		void DeleteDisplay(Coordinator& coord);

		/************************************************************************/
		/*!
		\brief
		Destroy methods for indicator

		/param coord
		Refernece to the coordinator
		*/
		/************************************************************************/
		void DestroyIndicator(Coordinator& coord);

	};
}
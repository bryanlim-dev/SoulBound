/******************************************************************************
 * File:        CollectibleSystem.h
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created Collectible System
 *
 * Description:
 * This file contains the declarations of the Collectible System.
 * Collectible System functionalities include:
 * - Despawn collectible upon collision with player, and respawn again after a certain period of time if needed
 * - Spawning and despawning of collectibles' particles whenever the collectible is active or inactive
 * 
 * Copyright © 2026 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include "Core/ECS/Coordinator.h"
#include "Systems/Application/IEvent.h"
#include "PlayerController.h"
#include "Physics/Physics.h"

namespace NightSnack {

	class CollectibleSystem : public ISystem {

	public:

		

		/************************************************************************/
		/*!
		\brief
		Constructor for the CollectibleSystem

		\param[in] coord
		Reference to the ECS Coordinator

		\param[in] dispatcher
		Reference to the event dispatcher used for Input Events
		*/
		/************************************************************************/
		CollectibleSystem(Coordinator& coord, EventDispatcher& dispatcher) : m_coord{ coord }, m_dispatcher{ &dispatcher } {
			m_systemSignature.set(GetComponentTypeID<Collectible>());
		}

		/************************************************************************/
		/*!
		\brief
		Main update called by the core.
		*/
		/************************************************************************/
		void Update() override;

		/************************************************************************/
		/*!
		\brief
		Function call to start to spawn all particle emitters once, when the level is reloaded
		*/
		/************************************************************************/
		void SpawnCollectibleParticles();

		/************************************************************************/
		/*!
		\brief
		Get the system's signature.
		Required by ISystem inheritance

		\return
		Signature (set of required components)
		*/
		/************************************************************************/
		Signature GetSignature() const override { return m_systemSignature; }

		/************************************************************************/
		/*!
		\brief
		Gets the name of this system, for debugging purposes
		Required by ISystem inheritance

		\return
		Name of the system
		*/
		/************************************************************************/
		std::string GetName() const { return "CollectibleSystem"; }

	private:

		/************************************************************************/
		/*!
		\brief
		Create all particle emitters that are meant for each collectible object 
		*/
		/************************************************************************/
		void CreateParticleEmitters();

		Signature m_systemSignature{};
		// Reference to Coordinator;
		Coordinator& m_coord;
		// System event dispatcher
		EventDispatcher* m_dispatcher = nullptr;

		Entity m_player = INVALID_ENTITY;

		bool m_createdParticleEmitters{ false };
	};

} // NightSnack
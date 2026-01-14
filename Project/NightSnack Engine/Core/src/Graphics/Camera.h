/******************************************************************************
 * File:        Camera.h
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 100%) - Camera System
 *
 * Description:
 * This file declares the CameraSystem class, which manages the camera entity.
 * The system ensures the camera follows the player as it is moved around the scene.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include "Core/ECS/ISystem.h"
#include "Core/ECS/Coordinator.h"
#include "Core/ECS/Component.h"
#include "Systems/Application/IEvent.h"

namespace NightSnack {

	/**
	 * @brief CameraSystem class which maanges the camera entity
	 */
	class CameraSystem : public ISystem {
	public:

		/**
		* @brief construct the camera system with reference to the coordinator and event dispatcher
		* @param coordinator reference to the ECS coordinator
		* @param dispatcher reference to the event dispatcher
		*/
		CameraSystem(Coordinator& coordinator, EventDispatcher& dispatcher);
		/**
		* @brief destructor
		*/
		~CameraSystem() = default;

		/**
		* @brief update to ensure the camera follows the player
		* @param steps simulation steps
		*/
		void Update() override;

		Transform FollowPlayer();
		void CenterCameraWhenIdle();

		/**
		 * @brief retrieve the system signature
		 * @return system signature
		 */
		Signature GetSignature() const override;

		/**
		 * @brief retrieve the system name
		 * @return system name
		 */
		std::string GetName() const override;

	private:
		Coordinator& m_coord;
		EventDispatcher* m_dispatcher;

		Entity m_mainCamera;
		Entity m_player;

		/**
		 * @brief find or create the camera entity
		 * @param coord reference to the ECS coordinator
		 * @return the entity ID of the camera
		 */
		Entity CameraComponent(Coordinator& coord);

		/**
		 * @brief find the player entity
		 * @param coord reference to the ECS coordinator
		 * @return the entity ID of the player
		 */
		Entity FindPlayer(Coordinator& coord);

		void FollowEntity(Entity e, Transform& t);
		void LimitWithinBounds(Transform& t);
	};
}
/******************************************************************************
 * File:        Camera.h
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 100%) - Camera System
 *
 * Description:
 * This file declares the CameraSystem class, which manages the camera entity.
 * The system ensures the camera follows the player as it is moved around the scene.
 *
 * Copyright � 2025 DigiPen Institute of Technology Singapore.
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

		enum class CAMERASTATE {
			MENU, 
			FOLLOW_PLAYER,
			LERP_CHECKPOINT,
			PAN_ROOM_COUNTDOWN, 
			PAN_ROOM, 
			PAN_ROOM_TO_KEY,
			PAN_ROOM_ZOOM_OUT,
			PAN_ROOM_ZOOM_IN,
			PAN_ROOM_TO_PLAYER
		};

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
		void UpdateCamBounds(json const&);
		//isPannningEnabled - Sets if start pan or not
		void StartCamPan(bool isPannningEnabled, bool isMainMenu = 0);
		void StartCamPan();

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

		/**
		 * @brief retrieve the main camera entity
		 * @return Main Camera Entity ID
		 */
		Entity GetMainCamera() const;

		bool GetCameraIsPanning() const;
		void ResetCameraPanProperties();

	private:
		Coordinator& m_coord;
		EventDispatcher* m_dispatcher;

		Entity m_mainCamera = INVALID_ENTITY;
		Entity m_player = INVALID_ENTITY;
		Entity m_centerOfLevel = INVALID_ENTITY;

		CAMERASTATE m_cameraState = CAMERASTATE::FOLLOW_PLAYER;

		Vec2 m_roomOriginOffset{ 0.f, 0.f };
		Vec2 m_currentRoom;
		Vec2 m_targetCamPos;
		Vec2 m_originalCamPos;
		//bool m_isLerping = false;
		bool m_panEnabled = false;
		bool m_panComplete = false;
		bool m_zoomComplete = false;
		const float m_camTriggerLerpSpeed = 8.0f; // Used for moving to a new area
		const float m_camPanLerpSpeed = 0.2f;	  // Used for camera panning
		const float m_camPanCountdown = 1.0f;	  // Used for countdown before camera panning
		const float m_zoomOutTarget = 0.25f;		  // End zoom out value for camera
		const float m_zoomInTarget = 1.0f;		  // End zoom in value for camera
		const float m_zoomSpeed = 0.5f;			  // Used for when camera is zooming in and out
		const float m_panDuration = 4.f;
		const float m_zoomDuration = 3.f;
		const float m_zoomHoldDuration = 2.f;	  // Used for a short duration when the camera is zoomed out to the max
		const float m_keyHoldDuration = 1.f;      // Used for a short duration when the camera has panned to the key
		//float m_lerpSpeed = 8.0f;
		float m_currCamPanTimer = 0.0f;
		float m_originalZoom = 1.0f;
		float m_targetZoom = 1.0f;
		float m_holdTimer = 0.0f;
		float m_zoomTimer = 0.0f;
		float m_panTimer = 0.0f;
		size_t m_panningTargetIndex = 0;

		std::vector<Vec2> m_panningTargets{};	  // Used to store all camera panning targets for the current scene

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

		Entity FindCenterOfLevel();

		void FollowEntity(Entity e, Transform& t);
		void LimitWithinBounds(Transform& t);
		void LerpCameraPosition(Transform& camT);
		bool ConstantPanCameraPosition(Transform& camT);
		bool LerpZoom();
		bool StoreCameraPanningTargets();
	};
}
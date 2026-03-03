/******************************************************************************
 * File:        Camera.cpp
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 100%) - Camera System
 *
 * Description:
 * This file defines the CameraSystem class, which manages the camera entity.
 * The system ensures the camera follows the player as it is moved around the scene.
 *
 * Copyright � 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
#include "Camera.h"
#include "Systems/Graphics.h"
#include "Systems/PlayerController.h"

#include <iostream>

namespace NightSnack {

    static float maxDistance = 40.f;

    CameraSystem::CameraSystem(Coordinator& coordinator, EventDispatcher& dispatcher)
        : m_coord(coordinator),
        m_dispatcher(&dispatcher),
        m_mainCamera(INVALID_ENTITY),
        m_player(INVALID_ENTITY) {
    }

    void CameraSystem::Update()
    {
        // Update target camera position
        Transform& t = m_coord.ModifyComponent<Transform>(m_mainCamera);
        Camera& c = m_coord.ModifyComponent<Camera>(m_mainCamera);

        //if (m_isLerping) {

        //    //m_coord.Pause();
        //    // Room Camera with lerping
        //    //LerpCameraPosition(t);

        //    return;
        //}

        //if (m_coord.IsPaused()) 
        //{
        //    m_targetCamPos = t.pos;
        //    // Set the room origin offset to the current camera position
        //    m_roomOriginOffset.x = t.pos.x;
        //    m_roomOriginOffset.y = t.pos.y;
        //    return;
        //}

        // find camera if we don't have one
        if (!m_coord.EntityIsAlive(m_mainCamera)) {
            m_mainCamera = m_coord.GetSystem<GraphicsSystem>()->GetCamera();
        }
        //// find player if we don't have one
        //if (!m_coord.EntityIsAlive(m_player)) {
        //    m_player = FindPlayer(m_coord);
        //}
        m_player = m_coord.GetSystem<PlayerControllerSystem>()->GetPlayer();
        
        if (m_centerOfLevel == INVALID_ENTITY) {
            m_centerOfLevel = FindCenterOfLevel();
        }

        switch (m_cameraState) {
        case CAMERASTATE::FOLLOW_PLAYER:
        {
            FollowEntity(m_player, t);
            if (c.lock)
                LimitWithinBounds(t);
            break;
        }

        case CAMERASTATE::PAN_ROOM_COUNTDOWN:
        {
            // Have a short countdown first before panning the camera
            if (m_currCamPanTimer < m_camPanCountdown) {
                m_currCamPanTimer += FixedDT;
                return;
            }

            // Store the camera panning targets here first
            StoreCameraPanningTargets();

            // Countdown has finished, start to pan the camera across the room now
            m_originalCamPos = t.pos; // Store the original position for constant panning
            m_panTimer = 0.0f;
            m_cameraState = CAMERASTATE::PAN_ROOM;
            m_currCamPanTimer = 0.f;

            return;
        }

        case CAMERASTATE::LERP_CHECKPOINT:
            LerpCameraPosition(t);
            return;

        case CAMERASTATE::PAN_ROOM:
        {
            static size_t panningTargetIndex = 0;
            if (panningTargetIndex < m_panningTargets.size()) {
                m_targetCamPos = m_panningTargets[panningTargetIndex];
                //m_originalCamPos = t.pos;

                if (!m_panComplete) {
                    m_panComplete = ConstantPanCameraPosition(t);

                } else {
                    // Current panning target is completed, move to the next target
                    m_panComplete = false;
                    m_originalCamPos = t.pos;
                    ++panningTargetIndex;
                }

                // Camera has panned all of the targets already including the player
                // Reset some variables first 
                if (!m_panComplete && panningTargetIndex >= m_panningTargets.size()) {
                    panningTargetIndex = 0;
                    m_panComplete = false;
                    m_panningTargets.clear();
                    m_cameraState = CAMERASTATE::FOLLOW_PLAYER;
                }
            } 
            return;
        }

        case CAMERASTATE::PAN_ROOM_TO_KEY:
        {
            if (!m_panComplete) m_panComplete = ConstantPanCameraPosition(t);
            else {
                m_holdTimer += FixedDT;

                if (m_holdTimer >= m_keyHoldDuration) {
                    // Reached key, set up zoom out toward center of level
                    m_targetCamPos = m_coord.GetComponent<Transform>(m_centerOfLevel).pos;
                    m_originalCamPos = t.pos;
                    auto& cam = m_coord.GetComponent<Camera>(m_mainCamera);
                    m_originalZoom = cam.zoom;
                    m_targetZoom = m_zoomOutTarget;
                    m_holdTimer = 0.0f;
                    m_zoomTimer = 0.0f;
                    m_panTimer = 0.0f;
                    m_panComplete = false;
                    m_zoomComplete = false;
                    m_cameraState = CAMERASTATE::PAN_ROOM_ZOOM_OUT;
                }
            }
            return;
        }
        case CAMERASTATE::PAN_ROOM_TO_PLAYER:
        {
            bool done = ConstantPanCameraPosition(t);
            if (done) m_cameraState = CAMERASTATE::FOLLOW_PLAYER;
            return;
        }

        case CAMERASTATE::PAN_ROOM_ZOOM_OUT:
        {
            if (!m_panComplete) m_panComplete = ConstantPanCameraPosition(t);
            if (!m_zoomComplete) m_zoomComplete = LerpZoom();

            if (m_panComplete && m_zoomComplete) {
                // Both done, hold for a moment
                m_holdTimer += FixedDT;
                if (m_holdTimer >= m_zoomHoldDuration) {
                    // Set up zoom in and pan back to player
                    m_holdTimer = 0.0f;

                    auto& cam = m_coord.GetComponent<Camera>(m_mainCamera);
                    m_originalZoom = cam.zoom;
                    m_targetZoom = m_zoomInTarget;

                    Vec2 playerPos = m_coord.GetComponent<Transform>(m_player).pos;
                    m_targetCamPos = {
                        std::clamp(playerPos.x, cam.minX, cam.maxX),
                        std::clamp(playerPos.y, cam.minY, cam.maxY)
                    };

                    //m_targetCamPos = m_coord.GetComponent<Transform>(m_player).pos;
                    m_originalCamPos = t.pos;
                    m_zoomTimer = 0.0f;
                    m_panTimer = 0.0f;
                    m_panComplete = false;
                    m_zoomComplete = false;
                    m_cameraState = CAMERASTATE::PAN_ROOM_ZOOM_IN;
                }
            }
            return;
        }

        case CAMERASTATE::PAN_ROOM_ZOOM_IN:
        {
            if (!m_panComplete) m_panComplete = ConstantPanCameraPosition(t);
            if (!m_zoomComplete) m_zoomComplete = LerpZoom();

            if (m_panComplete && m_zoomComplete) {
                auto& cam = m_coord.GetComponent<Camera>(m_mainCamera);
                Vec2 playerPos = m_coord.GetComponent<Transform>(m_player).pos;
                t.pos.x = std::clamp(playerPos.x, cam.minX, cam.maxX);
                t.pos.y = std::clamp(playerPos.y, cam.minY, cam.maxY);

                m_panComplete = false;
                m_zoomComplete = false;

                m_cameraState = CAMERASTATE::FOLLOW_PLAYER;
            }
            return;
        }
        }

        /*FollowEntity(m_player, t);
        if (c.lock)
            LimitWithinBounds(t);*/
        
        // Update actual camera pos
        //m_coord.ModifyComponent<Transform>(m_mainCamera) = t;
        // make camera follow player
   //     if (m_mainCamera != INVALID_ENTITY && m_player != INVALID_ENTITY)
   //     {
            //// check if both entities have Transform component
   //         if (m_coord.HasComponent<Transform>(m_mainCamera) &&
   //             m_coord.HasComponent<Transform>(m_player))
   //         {
   //             // get player position
   //             const auto& playerTransform = m_coord.GetComponent<Transform>(m_player);
   //             // set camera position to player position
   //             auto& cameraTransform = m_coord.ModifyComponent<Transform>(m_mainCamera);
   //             cameraTransform.pos.x = playerTransform.pos.x;
   //             cameraTransform.pos.y = playerTransform.pos.y;
   //         }
   //     }
   //     else 
   //     {
   //         // Set camera position to player position
   //         auto& cameraTransform = m_coord.ModifyComponent<Transform>(m_mainCamera);
   //         cameraTransform.pos = Vec3{0.f, 0.f, 2.f};
   //     }
    }

    void CameraSystem::FollowEntity(Entity e, Transform& ct) {
        // get entity position
        const auto& pt = m_coord.GetComponent<Transform>(e);
        auto& cam = m_coord.GetComponent<Camera>(m_mainCamera);
        // increment camera position
        ct.pos.x = pt.pos.x + cam.offset.x;
        ct.pos.y = pt.pos.y + cam.offset.y;
    }

    Transform CameraSystem::FollowPlayer() {

        // get player position
        const auto& pt = m_coord.GetComponent<Transform>(m_player);
        Vec2 pv = m_coord.GetComponent<RigidBody>(m_player).vel;

        // get camera position
        Transform ct = m_coord.GetComponent<Transform>(m_mainCamera);

        if (abs(ct.pos.x - pt.pos.x) > 60.f) {
            ct.pos.x += pv.x * FixedDT;
        }
        if (abs(ct.pos.y - pt.pos.y) > 60.f) {
            ct.pos.y += pv.y * FixedDT;
        }
        return ct;
    }

    void CameraSystem::LimitWithinBounds(Transform& ct) {

        auto& cam = m_coord.GetComponent<Camera>(m_mainCamera);

        if (ct.pos.x < cam.minX) ct.pos.x = cam.minX;
        if (ct.pos.x > cam.maxX) ct.pos.x = cam.maxX;

        if (ct.pos.y < cam.minY) ct.pos.y = cam.minY;
        if (ct.pos.y > cam.maxY) ct.pos.y = cam.maxY;
    }

    void CameraSystem::LerpCameraPosition(Transform& camT) {

        //if (!m_coord.EntityIsAlive(m_mainCamera) || !m_coord.EntityIsAlive(m_player))
        //    return;
        //const auto& playerT = m_coord.GetComponent<Transform>(m_player);
        //auto& cam = m_coord.GetComponent<Camera>(m_mainCamera);
        //auto& camT = m_coord.ModifyComponent<Transform>(m_mainCamera);
        //
        //int playerRoomX = static_cast<int>(std::round((playerT.pos.x - m_roomOriginOffset.x) / cam.width));
        //int playerRoomY = static_cast<int>(std::round((playerT.pos.y - m_roomOriginOffset.y) / cam.height));
        //int currentRoomX = static_cast<int>(m_currentRoom.x);
        //int currentRoomY = static_cast<int>(m_currentRoom.y);
        //// Start lerping if player entered new room
        //if (!m_isLerping && (playerRoomX != currentRoomX || playerRoomY != currentRoomY))
        //{
        //    m_currentRoom.x = static_cast<float>(playerRoomX);
        //    m_currentRoom.y = static_cast<float>(playerRoomY);
        //    m_targetCamPos =
        //    {
        //        m_currentRoom.x * cam.width + m_roomOriginOffset.x,
        //        m_currentRoom.y * cam.height + m_roomOriginOffset.y,
        //        camT.pos.z
        //    };
        //    m_isLerping = true;
        //}

        // Lerp camera to target position
        camT.pos.x += (m_targetCamPos.x - camT.pos.x) * std::min(m_camTriggerLerpSpeed * FixedDT, 1.0f);
        camT.pos.y += (m_targetCamPos.y - camT.pos.y) * std::min(m_camTriggerLerpSpeed * FixedDT, 1.0f);
        
        // Snap if close enough
        if (std::abs(camT.pos.x - m_targetCamPos.x) < 1.0f &&
            std::abs(camT.pos.y - m_targetCamPos.y) < 1.0f)
        {
            camT.pos.x = m_targetCamPos.x;
            camT.pos.y = m_targetCamPos.y;
            //m_coord.Unpause();
            //m_isLerping = false;
            
            m_cameraState = CAMERASTATE::FOLLOW_PLAYER;
        }
    }

    bool CameraSystem::ConstantPanCameraPosition(Transform& camT) {
        m_panTimer += FixedDT;
        float t = std::min(m_panTimer / m_panDuration, 1.0f);

        // Smoothstep: eases in and out
        float smoothT = t * t * (3.0f - 2.0f * t);

        camT.pos.x = m_originalCamPos.x + (m_targetCamPos.x - m_originalCamPos.x) * smoothT;
        camT.pos.y = m_originalCamPos.y + (m_targetCamPos.y - m_originalCamPos.y) * smoothT;

        if (t >= 1.0f) {
            camT.pos.x = m_targetCamPos.x;
            camT.pos.y = m_targetCamPos.y;
            m_panTimer = 0.0f;
            return true;
        }
        return false;
    }

    bool CameraSystem::LerpZoom() {
        auto& cam = m_coord.ModifyComponent<Camera>(m_mainCamera);

        m_zoomTimer += FixedDT;
        float t = std::min(m_zoomTimer / m_zoomDuration, 1.0f);

        // Smoothstep: eases in AND out, no abrupt snap at either end
        float smoothT = t * t * (3.0f - 2.0f * t);

        cam.zoom = m_originalZoom + (m_targetZoom - m_originalZoom) * smoothT;

        if (t >= 1.0f) {
            cam.zoom = m_targetZoom;
            m_zoomTimer = 0.0f;
            return true;
        }
        return false;
    }

    void CameraSystem::UpdateCamBounds(json const& c) {

        // Update Bounds
        auto& cam = m_coord.ModifyComponent<Camera>(m_mainCamera);
        cam.maxX = c["maxX"]; cam.minX = c["minX"];
        cam.maxY = c["maxY"]; cam.minY = c["minY"];
        cam.zoom = c["zoom"];

        // Set target position and activate Lerp if necessary
        auto& pos = m_coord.GetComponent<Transform>(m_mainCamera).pos;
        if (pos.x < cam.minX || pos.x > cam.maxX
            || pos.y < cam.minY || pos.y > cam.maxY) {

            m_targetCamPos = { 
                std::clamp(pos.x, cam.minX, cam.maxX), 
                std::clamp(pos.y, cam.minY, cam.maxY) };
            
            // Reset the speed here if it was changed
            //m_lerpSpeed = m_camTriggerLerpSpeed;
            //m_isLerping = true;
            m_cameraState = CAMERASTATE::LERP_CHECKPOINT;
        }
    }

    void CameraSystem::StartCamPan(bool isPannningEnabled)
    {
        m_panEnabled = isPannningEnabled;
        if (m_panEnabled)
            StartCamPan();
        else
            m_cameraState = CAMERASTATE::FOLLOW_PLAYER;
    }

    void CameraSystem::StartCamPan() {
        if (!m_panEnabled)
        {
            m_cameraState = CAMERASTATE::FOLLOW_PLAYER;
            return;
        }

        m_currCamPanTimer = 0.0f;
        m_cameraState = CAMERASTATE::PAN_ROOM_COUNTDOWN;

        //const auto& entities = m_coord.GetLivingEntities();

        //for (Entity e : entities)
        //{
        //    if (m_coord.HasComponent<Collectible>(e) && m_coord.GetComponent<Collectible>(e).type == COLLECTIBLE_TYPE::KEY) {
        //        m_targetCamPos = m_coord.GetComponent<Transform>(e).pos;
        //        m_currCamPanTimer = 0.0f; // Reset the countdown timer
        //        //m_lerpSpeed = m_camPanLerpSpeed; // Reset the speed here if it was changed
        //        m_cameraState = CAMERASTATE::PAN_ROOM_COUNTDOWN;
        //        break;
        //    }
        //}
    }

    void CameraSystem::CenterCameraWhenIdle() {
        static float time = 0.f;
        static Vec3 camPrev = m_coord.ModifyComponent<Transform>(m_mainCamera).pos;
        static bool idle = false;

        const auto& pt = m_coord.GetComponent<Transform>(m_player);
        Vec3& camCurr = m_coord.ModifyComponent<Transform>(m_mainCamera).pos;

        // Increment timer;
        time += FixedDT;

        // If camera is following player, don't count idle time;
        if (camPrev.x != camCurr.x && camPrev.y != camCurr.y) {
            time = 0.f;
        }

        if (time >= 2.f) {
            idle = true;
        }

        if (idle) {
            camCurr += (pt.pos - camCurr) * FixedDT;
        }

        if (abs(pt.pos.x - camCurr.x) <= 10.f && abs(pt.pos.y - camCurr.y) <= 10.f) {
            idle = false;
        }
    }

    Entity CameraSystem::CameraComponent(Coordinator& coord)
    {
        // Search for existing camera
        for (Entity entity : m_entities)
        {
            if (coord.HasComponent<Camera>(entity))
            {
                auto& camera = coord.GetComponent<Camera>(entity);
                if (camera.isMainCam)
                {
                    //std::cout << "[CameraSystem] Found main camera: " << entity << "\n";
                    return entity;
                }
            }
        }

        Entity cameraEntity = coord.CreateEntity();

        // add Transform
        Transform transform;
        transform.pos = Vec3{ 0.0f, 0.0f, 0.0f };
        transform.rot = 0.0f;
        transform.scale = Vec2{ 1.0f, 1.0f };
        coord.AddComponent(cameraEntity, transform);

        // add Camera
        Camera camera;
        camera.isMainCam = true;
        camera.zoom = 1.0f;
        camera.width = 1600.0f;
        camera.height = 900.0f;
        coord.AddComponent(cameraEntity, camera);

        //std::cout << "[CameraSystem] Created camera: " << cameraEntity << "\n";
        return cameraEntity;
    }

    Entity CameraSystem::FindPlayer(Coordinator& coord)
    {
        // find entity with PlayerController component
        const auto& entities = coord.GetLivingEntities();

        for (Entity entity : entities)
        {
            if (coord.HasComponent<PlayerController>(entity))
            {
                //std::cout << "[CameraSystem] Found player: " << entity << "\n";
                return entity;
            }
        }

        return INVALID_ENTITY;
    }

    Entity CameraSystem::FindCenterOfLevel() {
        const auto& entities = m_coord.GetLivingEntities();

        for (Entity entity : entities)
        {
            if (m_coord.GetComponent<Transform>(entity).name == "CenterOfLevelForCam") {
                return entity;
            }
        }

        return INVALID_ENTITY;
    }

    void CameraSystem::StoreCameraPanningTargets() {
        m_panningTargets.clear();
        const auto& entities = m_coord.GetLivingEntities();

        // Find the win zone first
        Entity winZone = INVALID_ENTITY;
        for (Entity e : entities) {
            if (m_coord.GetComponent<Transform>(e).name == "WinZone") {
                winZone = e;
                m_panningTargets.push_back(m_coord.GetComponent<Transform>(e).pos);
                break;
            }
        }

        if (winZone == INVALID_ENTITY) {
            //std::cout << "Cannot find Win Zone Entity!!!\n";
            m_panningTargets.clear();
            return;
        }
        
        // Find all 3 key collectibles next 
        std::vector<Entity> keys{};
        for (Entity e : entities) {
            if (m_coord.HasComponent<Collectible>(e) && m_coord.GetComponent<Collectible>(e).type == COLLECTIBLE_TYPE::KEY) {
                keys.push_back(e);
            }
        }

        if (keys.empty()) {
            //std::cout << "Cannot find any keys in the level!!!\n";
            m_panningTargets.clear();
            return;
        }

        // Sort the keys starting from shortest distance from the end of level first 
        const auto& winZonePos = m_coord.GetComponent<Transform>(winZone).pos;
        std::sort(keys.begin(), keys.end(), [this, winZonePos](Entity a, Entity b) {
                float distA = (m_coord.GetComponent<Transform>(a).pos - winZonePos).magnitude();
                float distB = (m_coord.GetComponent<Transform>(b).pos - winZonePos).magnitude();
                return distA < distB;
        });

        for (Entity key : keys) {
            m_panningTargets.push_back(m_coord.GetComponent<Transform>(key).pos);
        }

        // Find the camera bounds of the first area where the player is 
        auto& cam = m_coord.GetComponent<Camera>(m_mainCamera);
        Vec2 startCamPos = Vec2((cam.minX + cam.maxX) * 0.5f, (cam.minY + cam.maxY) * 0.5f);
        m_panningTargets.push_back(startCamPos);
    }

    // signature
    Signature CameraSystem::GetSignature() const
    {
        Signature sig;
        sig.set(GetComponentTypeID<Camera>(), true);
        sig.set(GetComponentTypeID<Transform>(), true);
        return sig;
    }

    // get system name
    std::string CameraSystem::GetName() const
    {
        return "CameraSystem";
    }
    Entity CameraSystem::GetMainCamera() const
    {
        return m_mainCamera;
    }

    bool CameraSystem::GetCameraIsPanning() const
    {
        return (m_cameraState == CAMERASTATE::PAN_ROOM_COUNTDOWN || 
            m_cameraState == CAMERASTATE::PAN_ROOM || 
            m_cameraState == CAMERASTATE::PAN_ROOM_TO_KEY ||
            m_cameraState == CAMERASTATE::PAN_ROOM_ZOOM_OUT ||
            m_cameraState == CAMERASTATE::PAN_ROOM_ZOOM_IN ||
            m_cameraState == CAMERASTATE::PAN_ROOM_TO_PLAYER);
    }
}
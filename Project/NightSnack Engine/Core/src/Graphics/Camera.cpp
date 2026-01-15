/******************************************************************************
 * File:        Camera.cpp
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 100%) - Camera System
 *
 * Description:
 * This file defines the CameraSystem class, which manages the camera entity.
 * The system ensures the camera follows the player as it is moved around the scene.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
#include "Camera.h"
#include "Systems/Graphics.h"
#include <iostream>

namespace NightSnack {

    static float maxDistance = 40.f;

    CameraSystem::CameraSystem(Coordinator& coordinator, EventDispatcher& dispatcher)
        : m_coord(coordinator),
        m_dispatcher(&dispatcher),
        m_mainCamera(INVALID_ENTITY),
        m_player(INVALID_ENTITY) {}

    void CameraSystem::Update()
    {
        if (m_coord.IsPaused()) { return; }

        // find camera if we don't have one
        if (!m_coord.EntityIsAlive(m_mainCamera)) {
            m_mainCamera = m_coord.GetSystem<GraphicsSystem>()->GetCamera();
        }
        // find player if we don't have one
        if (!m_coord.EntityIsAlive(m_player)) {
            m_player = FindPlayer(m_coord);
        }

        Transform t = m_coord.GetComponent<Transform>(m_mainCamera);
        FollowEntity(m_player, t);
        LimitWithinBounds(t);
        
        // Update actual camera pos
        m_coord.ModifyComponent<Transform>(m_mainCamera) = t;

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
}
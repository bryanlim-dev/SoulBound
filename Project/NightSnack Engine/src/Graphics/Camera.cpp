/******************************************************************************
 * File:        Camera.cpp
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 100%)
 *
 * Description:

 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "Camera.h"
#include <iostream>

namespace NightSnack {

    CameraSystem::CameraSystem(Coordinator& coordinator, EventDispatcher& dispatcher)
        : m_coord(&coordinator),
        m_dispatcher(&dispatcher),
        m_mainCamera(INVALID_ENTITY),
        m_player(INVALID_ENTITY) {}

    void CameraSystem::Update(int steps)
    {
        (void)steps;

        // use local reference to the coordinator
        Coordinator& coord = *m_coord;

        // Find camera if we don't have one
        if (m_mainCamera == INVALID_ENTITY)
        {
            m_mainCamera = CameraComponent(coord);
        }

        // Find player if we don't have one
        if (m_player == INVALID_ENTITY)
        {
            m_player = FindPlayer(coord);
        }

        // If we have both camera and player, make camera follow player
        if (m_mainCamera != INVALID_ENTITY && m_player != INVALID_ENTITY)
        {
            // Check if both entities still have transforms
            if (coord.HasComponent<Transform>(m_mainCamera) &&
                coord.HasComponent<Transform>(m_player))
            {
                // Get player position
                const auto& playerTransform = coord.GetComponent<Transform>(m_player);

                // Set camera position to player position
                auto& cameraTransform = coord.ModifyComponent<Transform>(m_mainCamera);
                cameraTransform.pos.x = playerTransform.pos.x;
                cameraTransform.pos.y = playerTransform.pos.y;
            }
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
                    std::cout << "[CameraSystem] Found main camera: " << entity << "\n";
                    return entity;
                }
            }
        }

        Entity cameraEntity = coord.CreateEntity();

        // Add Transform
        Transform transform;
        transform.pos = Vec3{ 0.0f, 0.0f, 0.0f };
        transform.rot = 0.0f;
        transform.scale = Vec2{ 1.0f, 1.0f };
        coord.AddComponent(cameraEntity, transform);

        // Add Camera
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
        // Find entity with PlayerController component
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

    Signature CameraSystem::GetSignature() const
    {
        Signature sig;
        sig.set(GetComponentTypeID<Camera>(), true);
        sig.set(GetComponentTypeID<Transform>(), true);
        return sig;
    }

    std::string CameraSystem::GetName() const
    {
        return "CameraSystem";
    }

}
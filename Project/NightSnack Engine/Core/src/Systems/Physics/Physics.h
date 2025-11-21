/******************************************************************************
 * File:        Physics.h
 * Project:     NightSnack
 * Author(s):   Chewn Thing Kwan (Primary - 80%) - Created the class.
 *				LIM Wei Jie (Secondary, 20%) - Event dispatcher subscription 
 *              to Input Events to toggle debug mode and step through physics updates
 *
 * Description: 
 * This file declares the PhysicsSystem class which handles the RigidBody and
 * Collider components of Entities.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include "Core/ECS/Coordinator.h"
#include "Systems/Application/IEvent.h"
#include "Systems/PlayerController.h"

#include <bitset>
#include <unordered_set>
#include <array>
#include <iostream>

namespace NightSnack {

    // Minimum velocity threshold to consider an object at rest;
    constexpr float velocityCutoff = 0.2f;

    class PhysicsSystem : public ISystem {

    public:
        /************************************************************************/
        /*!
        \brief
        Constructor for PhysicsSystem that sets the system signature and sets up
        input events for step by steps physics debugging mode as well as
        ApplyForce() function that is used in default scene for testing:
            Dynamic -> Dynamic collision
            Dynamic -> Static collsion

        \param coord
        Reference to Coordinator.
        \param dispatcher
        Reference to Event Dispatcher.
        */
        /************************************************************************/
        PhysicsSystem(Coordinator& coord, EventDispatcher& disp) 
            : m_coord{ coord }, m_dispatcher{ &disp } {

            // Physics components of interest;
            m_systemSignature.set(GetComponentTypeID<Transform>());
            m_systemSignature.set(GetComponentTypeID<RigidBody>());
            m_systemSignature.set(GetComponentTypeID<Collider>());

            // Event dispatcher initialisations
            m_dispatcher->Subscribe<InputEvent>([this, &coord](const InputEvent& event) {
                if (event.pressed == InputEvent::InputActionState::Pressed) {
                    switch (event.inputAction) {
                    case InputEvent::InputAction::PhysicsDebugMode:
                        debugMode = !debugMode;
                        break;
                    case InputEvent::InputAction::PhysicsDebugStep:
                        stepRequest = true;
                        break;
                    }
                }
            });
            // Add some default layers;
            AddLayer(LayerType::BACKGROUND);
            AddLayer(LayerType::GAMELAYER);

            // Initialize collision grid;
            m_gridCols = 30; m_gridRows = 15;
            m_gridWidth = 2500.f; m_gridHeight = 1250.f;
            m_cellSize.x = m_gridWidth / (float) m_gridCols;
            m_cellSize.y = m_gridHeight / (float) m_gridRows;
            m_gridCenter = {};
        }

        /************************************************************************/
        /*!
        \brief
        Computes the Velocity components of entities based on their Force, Mass
        and Acceleration components, and also applys gravity and drag/friction.

        \param dt
        delta time.
        \param coord
        Reference to Coordinator.
        */
        /************************************************************************/
        void Update() override;

        // Required by ISystem base class;
        Signature GetSignature() const override { return m_systemSignature; }
        std::string GetName() const { return "PhysicsSystem"; }

        bool IsDebugMode() const { return debugMode; }
        bool IsStepRequested() const { return stepRequest; }

        void ManageForces(RigidBody& f);
        void TransferForce(Transform const& t1, Transform const& t2, RigidBody& from, RigidBody& to);
        void ApplyForce(RigidBody& r, Force const&);
        void ResetForce(Force& f);

        void ComputeVelocity(RigidBody& rig, Collider const& col) const;

        std::pair<int, int> GetCell(Vec2 const& pos) const;
        void CollisionGridInsert(Entity, Collider const&, Transform const&);
        void ResolveCollisions();
        void ResolveOverlap(Transform& t1, Transform& t2, Collider& c1, Collider& c2, RigidBody& r1, RigidBody& r2);
        bool CheckCollision(Transform const& t1, Transform const& t2, Collider const& c1, Collider const& c2);

        float GetGravity() const { return gravity; }
        float GetDrag() const { return drag; }
        float GetFriction() const { return friction; }

        void SetGravity(float g) { gravity = g; }
        void SetDrag(float d) { drag = d; }
        void SetFriction(float f) { friction = f; }

    private:
        Signature m_systemSignature{};
        EventDispatcher* m_dispatcher{ nullptr };
        Coordinator& m_coord;

        // Collision stuff;
        std::vector<std::bitset<MAX_ENTITIES>> m_grid1;
        std::vector<std::bitset<MAX_ENTITIES>> m_grid2;
        std::unordered_set<std::uint64_t> m_testedEntityPairs;

        Vec2 m_gridCenter;
        Vec2 m_cellSize;
        float m_gridWidth, m_gridHeight;
        int m_gridCols, m_gridRows;

        // Physics parameters;
        float gravity = -980.f;
        float drag = 0.5f;
        float friction = 0.8f;

        // DebugMode for Physics;
        bool debugMode = false;
        bool stepRequest = false;
    };

}   // NightSnack

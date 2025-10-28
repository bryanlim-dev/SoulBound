/******************************************************************************
 * File:        Physics.h
 * Project:     NightSnack
 * Author(s):   Chewn Thing Kwan (Primary - 62%) - Created the class.
 *				LIM Wei Jie (Secondary, 38%) - Event dispatcher subscription 
 *              to Input Events to toggle debug mode and step through physics updates
 *
 * Description: 
 *
 * Copyright � 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include "Core/ECS/Coordinator.h"
#include "Systems/Application/IEvent.h"

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
        PhysicsSystem(Coordinator& coord, EventDispatcher& disp) : m_coord{ coord }, m_dispatcher{ &disp } {
            // Physics components of interest;
            m_systemSignature.set(GetComponentTypeID<Transform>());
            m_systemSignature.set(GetComponentTypeID<RigidBody>());
            m_systemSignature.set(GetComponentTypeID<Collider>());

            // Event dispatcher initialisations
            m_dispatcher->Subscribe<InputEvent>([this, &coord](const InputEvent& event) {
                if (event.pressed == InputEvent::InputActionState::Pressed) {
                    switch (event.inputAction) {
                    case InputEvent::InputAction::DynamicBallTest:
                        ApplyForce(coord, 2, 80.f, 0.f);
                        break;
                    case InputEvent::InputAction::PhysicsDebugMode:
                        debugMode = !debugMode;
                        break;
                    case InputEvent::InputAction::PhysicsDebugStep:
                        stepRequest = true;
                        break;
                    }
                }

                });
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
        void Update(int steps) override;

        // Required by ISystem base class;
        Signature GetSignature() const override { return m_systemSignature; }
        std::string GetName() const { return "PhysicsSystem"; }

        bool IsDebugMode() const { return debugMode; }
        bool IsStepRequested() const { return stepRequest; }

        void ApplyForce(Coordinator& coord, Entity e, float fx, float fy);

    private:
        Signature m_systemSignature{};
        EventDispatcher* m_dispatcher{ nullptr };
        Coordinator& m_coord;

        // Physics parameters;
        float gravity = -980.f;
        float drag = 0.2f;
        float friction = 0.8f;

        // DebugMode for Physics;
        bool debugMode = false;
        bool stepRequest = false;
    };

}   // NightSnack

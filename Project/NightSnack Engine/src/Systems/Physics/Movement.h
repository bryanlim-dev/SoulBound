/******************************************************************************
 * File:        Movement.h
 * Project:     NightSnack
 * Author(s):   Chewn Thing Kwan (Primary - 60%) - Created the class.
 *				Lim Wei Jie (Secondary, 40%) - Event dispatcher subscription
 *              to Input Events to toggle debug mode and step through physics
 *              updates. Skips Movement updates when in debug mode unless step
 *              is requested.
 *
 * Description:
 * Defines MovementSystem class that will update the Position component of
 * entities based on their Velocity components.
 *
 * Copyright � 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#ifndef MOVEMENT_HPP
#define MOVEMENT_HPP

#include "Core/ECS/Coordinator.h"
#include "Physics.h"

namespace NightSnack {

    static float dt = 0.016f;

    class MovementSystem : public ISystem {

    public:
        /************************************************************************/
        /*!
        \brief
        Constructor for MovementSystem that sets the system signature and sets up
        input events for step by steps physics debugging mode.

        \param coord
        Reference to Coordinator.
        \param dispatcher
        Reference to Event Dispatcher.
        */
        /************************************************************************/
        MovementSystem(Coordinator& coord, EventDispatcher& dispatcher) : m_coord{ coord } {
            // Input all components you want access to...
            m_systemSignature.set(GetComponentTypeID<Transform>());
            m_systemSignature.set(GetComponentTypeID<RigidBody>());

            // Event dispatcher initialisations
            m_dispatcher = &dispatcher;
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
            // Add more Components here if necessary...
        }

        /************************************************************************/
        /*!
        \brief
        Update function to be called in core loop, updates position of all entities
        that have Position and Velocity components.

        \param dt
        delta time.
        \param coord
        Reference to Coordinator.
        */
        /************************************************************************/
        void Update(int steps) override {
            (void)steps;

            // Pause movement updates if in Physics debug mode
            if (debugMode && !stepRequest) {
                return;
            }

            // Iterate through all Entities this system can access;
            for (Layer& l : m_layers) {
                if (!l.IsActive()) { continue; }

                for (Entity e : l.entities()) {
                    if (e == INVALID_ENTITY) { continue; }
                    // uhhh for some reason camera colliding so... this is here xdd
                    //if (m_coord.HasComponent<Camera>(e)) { continue; }

                    // First obtain references to components of interest;
                    auto& pos = m_coord.ModifyComponent<Transform>(e).pos;
                    auto& velocity = m_coord.ModifyComponent<RigidBody>(e).vel;

                    // Get more components here if necessary...

                    // Do stuff that you want to do with the components;
                    pos.x += velocity.x * dt;
                    pos.y += velocity.y * dt;

                    if (pos.y > 450.f) { pos.y = 450.f; }
                    if (pos.y < -450.f) { pos.y = -450.f; }
                    if (pos.x > 800.f) { pos.x = 800.f; }
                    if (pos.x < -800.f) { pos.x = -800.f; }

                    // Do more stuff here if necessary...

                    if (debugMode) stepRequest = false; // reset step request
                }
            }
        }

        // Required by ISystem base class;
        Signature GetSignature() const override { return m_systemSignature; }
        std::string GetName() const override { return "Movement"; }

    private:
        Signature m_systemSignature{};
        EventDispatcher* m_dispatcher = nullptr;
        Coordinator& m_coord;

        // DebugMode for Physics;
        bool debugMode = false;
        bool stepRequest = false;
    };

    // Declare and define more functions that will help you do stuff...

    // Example:
    // void SomeHelperFunction() {}
    // ^ This function will then be called in either Update() or another function
    //   that is called in Update();

}   // NightSnack

#endif // MOVEMENT_HPP
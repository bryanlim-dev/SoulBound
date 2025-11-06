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
            AddLayer(LayerType::BACKGROUND);
            AddLayer(LayerType::GAMELAYER);
            AddLayer(LayerType::FOREGROUND);
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

            // Pause movement updates if in Physics debug mode
            if (debugMode && !stepRequest) {
                return;
            }

            // Accumulated Time FixedDT;
            for (int s{}; s < steps; ++s) {

                // Iterate through all Entities this system can access;
                for (Layer& l : m_layers) {
                    if (!l.IsActive()) { continue; }

                    for (Entity e : l.Entities()) {
                        if (e == INVALID_ENTITY) { break; }

                        // First obtain references to components of interest;
                        auto& pos = m_coord.ModifyComponent<Transform>(e).pos;
                        auto& velocity = m_coord.ModifyComponent<RigidBody>(e).vel;

                        // Get more components here if necessary...

                        // Do stuff that you want to do with the components;
                        pos.x += velocity.x * FixedDT;
                        pos.y += velocity.y * FixedDT;

                        // Limit movement to borders of screen;
                        if (pos.y > m_coord.GetWindowHeight() / 2.f) { pos.y = m_coord.GetWindowHeight() / 2.f; }
                        if (pos.y < -m_coord.GetWindowHeight() / 2.f) { pos.y = -m_coord.GetWindowHeight() / 2.f; }
                        if (pos.x > m_coord.GetWindowWidth() / 2.f) { pos.x = m_coord.GetWindowWidth() / 2.f; }
                        if (pos.x < -m_coord.GetWindowWidth() / 2.f) { pos.x = -m_coord.GetWindowWidth() / 2.f; }

                        // Do more stuff here if necessary...

                        if (debugMode) stepRequest = false; // reset step request
                    }
                }
            }
        }

        // Required by ISystem base class;
        Signature GetSignature() const override { return m_systemSignature; }
        std::string GetName() const override { return "MovementSystem"; }

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
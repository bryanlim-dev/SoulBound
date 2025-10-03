/******************************************************************************
 * File:        Physics.h
 * Project:     NightSnack
 * Author(s):
 *
 * Description:
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include "ECS/Entity.h"
#include "ECS/ISystem.h"
#include "ECS/Coordinator.h"
#include "ECS/Component.h"
#include "Systems/IEvent.h"

// Minimum velocity threshold to consider an object at rest;
constexpr float velocityCutoff = 0.2f;

class PhysicsSystem : public ISystem {

public:
    // Default constructor to set up SystemSignature;
    PhysicsSystem(Coordinator& coord, EventDispatcher& disp) : m_dispatcher{ &disp } {
		// Physics components of interest;
        m_systemSignature.set(GetComponentTypeID<Position>());
        m_systemSignature.set(GetComponentTypeID<Velocity>());
        m_systemSignature.set(GetComponentTypeID<Acceleration>());
        m_systemSignature.set(GetComponentTypeID<Mass>());
        m_systemSignature.set(GetComponentTypeID<Force>());
        m_systemSignature.set(GetComponentTypeID<Collider>());

        // Event dispatcher initialisations
        m_dispatcher->Subscribe<InputEvent>([this, &coord](const InputEvent& event) {
			if (event.pressed == InputEvent::InputActionState::Pressed) {
			switch (event.inputAction) {
			case InputEvent::InputAction::DynamicBallTest:
                ApplyForce(coord, 2, 80.f, 0.f);
                break;
            //case InputEvent::InputAction::GravityBallTest:
            //    if (coord.GetComponent<Collider>(6).isGrounded) {
            //    coord.ModifyComponent<Collider>(6).isGrounded = false;
            //    ApplyForce(coord, 6, 0.f, 60.f);
            //    }
            //    break;
            //    break;
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

    void Update(float dt, Coordinator& coord) override;
	Signature GetSignature() const override { return m_systemSignature; }
    std::string GetName() const { return "PhysicsSystem"; }

	bool IsDebugMode() const { return debugMode; }
	bool IsStepRequested() const { return stepRequest; }

    void ApplyForce(Coordinator& coord, Entity e, float fx, float fy);

private:
    Signature m_systemSignature{};
    EventDispatcher* m_dispatcher{ nullptr };

	// Physics parameters;
    float gravity = -980.f;
    float drag = 0.2f;
    float friction = 0.8f;

	// DebugMode for Physics;
	bool debugMode = false;
	bool stepRequest = false;
};

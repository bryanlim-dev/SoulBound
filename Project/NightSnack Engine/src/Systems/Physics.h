#pragma once

#include "ECS/Entity.h"
#include "ECS/ISystem.h"
#include "ECS/Coordinator.h"
#include "ECS/Component.h"

// Minimum velocity threshold to consider an object at rest;
constexpr float velocityCutoff = 0.2f;

class PhysicsSystem : public ISystem {

public:
    // Default constructor to set up SystemSignature;
    PhysicsSystem() {
		// Physics components of interest;
        m_systemSignature.set(GetComponentTypeID<Position>());
        m_systemSignature.set(GetComponentTypeID<Velocity>());
        m_systemSignature.set(GetComponentTypeID<Acceleration>());
        m_systemSignature.set(GetComponentTypeID<Mass>());
        m_systemSignature.set(GetComponentTypeID<Force>());
        m_systemSignature.set(GetComponentTypeID<Collider>());
    
<<<<<<< Updated upstream
=======
        // Event dispatcher initialisations
        m_dispatcher = &dispatcher;
        m_dispatcher->Subscribe<InputEvent>([this, &coord](const InputEvent& event) {
			if (event.pressed == InputEvent::InputActionState::Pressed) {
			switch (event.inputAction) {
			case InputEvent::InputAction::DynamicBallTest:
                ApplyForce(coord, 2, 80.f, 0.f);
                break;
            case InputEvent::InputAction::GravityBallTest:
                if (coord.GetComponent<Collider>(6).isGrounded) {
                coord.ModifyComponent<Collider>(6).isGrounded = false;
                ApplyForce(coord, 6, 0.f, 60.f);
                }
                break;
			}
		}

		});
>>>>>>> Stashed changes
    }

    void Update(float dt, Coordinator& coord) override;
	Signature GetSignature() const override { return m_systemSignature; }
    std::string GetName() const { return "PhysicsSystem"; }

    void ApplyForce(Coordinator& coord, Entity e, float fx, float fy);

private:
    Signature m_systemSignature{};

	// Physics parameters;
    float gravity = -980.f;
    float drag = 0.2f;
    float friction = 0.8f;

};

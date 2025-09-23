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

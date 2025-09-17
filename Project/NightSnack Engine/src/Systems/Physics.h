#pragma once

#include "ECS/Entity.h"
#include "ECS/ISystem.h"
#include "ECS/Coordinator.h"
#include "ECS/Component.h"

// Minimum velocity threshold to consider an object at rest;
constexpr double velocityCutoff = 0.1;

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

  void Update(double dt, Coordinator& coord) override;
	Signature GetSignature() const override { return m_systemSignature; }

  void ApplyForce(Coordinator& coord, Entity e, double fx, double fy);

private:
  Signature m_systemSignature{};

	// Physics parameters;
  double gravity = -980; 
	double drag = 0.2;
	double friction = 0.8;

};

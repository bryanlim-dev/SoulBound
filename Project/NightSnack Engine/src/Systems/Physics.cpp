#include "Systems/Physics.h"

// Main update function;
void PhysicsSystem::Update(double dt, Coordinator& coord) {

	// Iterate through all Entities this system can access;
	for (auto const& e : m_entities) {

		// First obtain references to components of interest;
		auto& force = coord.GetComponent<Force>(e);
		auto& vel = coord.GetComponent<Velocity>(e);
		auto& accel = coord.GetComponent<Acceleration>(e);
		auto& mass = coord.GetComponent<Mass>(e);

		// Calculate acceleration from force and mass;
		accel.ax = force.fx / mass.val;
		accel.ay = force.fy / mass.val;

		// Apply gravity;
		accel.ay += gravity;

		// Update velocity based on acceleration;
		vel.dx += accel.ax * dt;
		vel.dy += accel.ay * dt;

		// Apply horizontal drag, a force opposite to the direction of motion
		// when mid-air;
		vel.dx *= 1.0 - drag * dt;

		// Apply friction if on the ground;
		if (coord.GetComponent<Collider>(e).isGrounded) {

			vel.dx *= 1.0 - friction * dt;

			// Small threshold to stop sliding;
			if (std::abs(vel.dx) < velocityCutoff) vel.dx = 0.0;
		}

		// Clear forces for next frame;
		force.fx = 0.0;
		force.fy = 0.0;

	}
}

// Apply a force to an entity by modifying its Force component;
void PhysicsSystem::ApplyForce(Coordinator& coord, Entity e, double fx, double fy) {
	auto& force = coord.GetComponent<Force>(e);
	force.fx += fx;
	force.fy += fy;
}
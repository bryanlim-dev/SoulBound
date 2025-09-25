#include "Systems/Physics.h"
#include "Systems/Input.h"

// Main update function;
void PhysicsSystem::Update(float dt, Coordinator& coord) {

	if (coord.GetSystem<InputSystem>()->IsKeyPressed(InputSystem::SPACE)) {
		coord.GetSystem<PhysicsSystem>()->ApplyForce(coord, Entity{ 0 }, 750.f, 0.f);
	}

	// Iterate through all Entities this system can access;
	for (auto const& e : m_entities) {

		// First obtain references to components of interest;
		auto& force = coord.GetComponent<Force>(e);
		auto& vel = coord.GetComponent<Velocity>(e);
		auto& accel = coord.GetComponent<Acceleration>(e);
		auto& mass = coord.GetComponent<Mass>(e);

		// Calculate acceleration from force and mass;
		accel.ax = force.fx * 1000.f / mass.val;
		accel.ay = force.fy * 1000.f / mass.val;

		// Apply gravity;
		if (coord.GetComponent<Collider>(e).isGrounded == false) {
			accel.ay += gravity;
		}

		// Update velocity based on acceleration;
		vel.dx += accel.ax * dt;
		vel.dy += accel.ay * dt;

		// Apply horizontal drag, a force opposite to the direction of motion
		// when mid-air;
		vel.dx *= 1.f - drag * dt;

		// Apply friction if on the ground;
		if (coord.GetComponent<Collider>(e).isGrounded) {

			vel.dx *= 1.f - friction * dt;

			// Small threshold to stop sliding;
			if (std::abs(vel.dx) < velocityCutoff) vel.dx = 0.0;
		}

		// Clear forces for next frame;
		force.fx = 0.f;
		force.fy = 0.f;

	}
}

// Apply a force to an entity by modifying its Force component;
void PhysicsSystem::ApplyForce(Coordinator& coord, Entity e, float fx, float fy) {
	auto& force = coord.GetComponent<Force>(e);
	force.fx += fx;
	force.fy += fy;
}
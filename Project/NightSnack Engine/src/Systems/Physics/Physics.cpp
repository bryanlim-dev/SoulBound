/******************************************************************************
 * File:        Physics.cpp
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan (Primary, 88%) - Defined PhysicsSystem member functions
 *				Lim Wei Jie (Secondary, 12%) - Skips Physics updates when in
 *				debug mode unless a step is requested via InputEvent
 *
 * Description:
 * Defines the member functions of PhysicsSystem class.
 *
 * Copyright � 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "Systems/Physics/Physics.h"
#include "Systems/Application/Input.h"

namespace NightSnack {

	static float dt = 0.016f;

	// Main update function;
	void PhysicsSystem::Update(int steps) {
		// Skip Physics if in debug mode
		if (debugMode && !stepRequest) {
			return;
		}
		for (int s{}; s < steps; ++s) {
			// Iterate through all Entities this system can access;
			for (auto const& e : m_entities) {

				RigidBody& rig = m_coord.ModifyComponent<RigidBody>(e);

				

				// compute velocities;
				//ApplyForces(rig);
				//Gravity(rig);
				//DragFriction(rig);
				//UpdateForce(force);

				// resolve collisions;

				// update transforms;




				// First obtain references to components of interest;
				auto& vel = m_coord.ModifyComponent<RigidBody>(e).vel;
				auto& accel = m_coord.ModifyComponent<RigidBody>(e).accel;
				auto& mass = m_coord.ModifyComponent<RigidBody>(e).mass;
				bool g = m_coord.GetComponent<RigidBody>(e).useGravity;

				// Calculate acceleration from force and mass;
				accel.x = force.fx * 1000.f / mass;
				accel.y = force.fy * 1000.f / mass;

				// Apply gravity;
				if (g && m_coord.GetComponent<Collider>(e).isGrounded == false) {
					accel.y += gravity;
				}

				// Update velocity based on acceleration;
				vel.x += accel.x * dt;
				vel.y += accel.y * dt;

				// Apply horizontal drag, a force opposite to the direction of motion
				// when mid-air;
				vel.x *= 1.f - drag * dt;

				// Apply friction if on the ground;
				if (m_coord.GetComponent<Collider>(e).isGrounded) {

					vel.x *= 1.f - friction * dt;

					// Small threshold to stop sliding;
					if (std::abs(vel.x) < velocityCutoff) vel.x = 0.0;
				}

				// Clear forces for next frame;
				force.fx = 0.f;
				force.fy = 0.f;
			}
		}
		if (debugMode) stepRequest = false; // reset step request
	}

	// Apply a force to an entity by modifying its Force component;
	void PhysicsSystem::ApplyForce(Coordinator& coord, Entity e, float fx, float fy) {
		auto& force = coord.ModifyComponent<Force>(e);
		force.fx += fx;
		force.fy += fy;
	}

}	// NightSnack

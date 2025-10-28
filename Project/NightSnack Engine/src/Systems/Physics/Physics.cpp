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

#include <algorithm>
#include <unordered_map>

#include "Physics.h"
#include "Collision.h"
#include "Systems/PlayerController.h"

namespace NightSnack {

	std::pair<int, int> PhysicsSystem::GetCell(Vec2 const& pos) const {
		Vec2 gridPos = pos - m_gridCenter;
		return { std::floor(gridPos.x / m_cellSize) + GRID_W / 2, std::floor(gridPos.y / m_cellSize) / GRID_H };
	}

	void PhysicsSystem::CollisionGridInsert(Entity e, Collider const& col, Transform const& t) {

		Vec2 min, max;

		if (col.shape == ShapeType::AABB) {
			min = Vec2{ t.pos.x - (col.width / 2.f), t.pos.y - (col.height / 2.f) };
			max = Vec2{ t.pos.x + (col.width / 2.f), t.pos.y + (col.height / 2.f) };
		}
		else if (col.shape == ShapeType::Circle) {
			min = Vec2{ t.pos.x - (col.radius / 2.f), t.pos.y - (col.radius / 2.f) };
			max = Vec2{ t.pos.x + (col.radius / 2.f), t.pos.y + (col.radius / 2.f) };
		}
		// find min and max cell;
		std::pair<int, int> minCell = GetCell(min);
		std::pair<int, int> maxCell = GetCell(max);

		// Clamp to grid bounds
		for (int x = minCell.first; x <= maxCell.first; ++x) {
			for (int y = minCell.second; y <= maxCell.second; ++y) {
				if (x >= 0 && x < GRID_W && y >= 0 && y < GRID_H) {
					m_grid[x][y].set(e);
				}
			}
		}
	}

	void PhysicsSystem::ComputeVelocity(RigidBody& rig, Collider const& col) const {

		// Compute Accel;
		if (rig.force.isActive) {
			rig.accel = (rig.force.direction * rig.force.magnitude) * rig.invMass;
		}
		if (rig.useGravity && !col.isGrounded) {
			rig.accel.y += gravity;
		}
		// Compute Velocity;
		rig.vel += rig.accel * FixedDT;
		rig.vel.x *= 1.f - drag * FixedDT;

		// Apply friction if on the ground;
		if (col.isGrounded) {
			rig.vel.x *= 1.f - friction * FixedDT;

			if (std::abs(rig.vel.x) < velocityCutoff)
				rig.vel.x = 0.0;
		}
	}

	// Main update function;
	void PhysicsSystem::Update(int steps) {
		// Skip Physics if in debug mode
		if (debugMode && !stepRequest) {
			return;
		}
		// Update Center of Collision grid;
		Entity p = m_coord.GetSystem<PlayerControllerSystem>()->GetPlayer();
		m_gridCenter = Vec2{ m_coord.GetComponent<Transform>(p).pos.x, m_coord.GetComponent<Transform>(p).pos.y };

		for (int s{}; s < steps; ++s) {

			for (Layer& l : m_layers) {
				// Skip inactive and empty layers;
				if (l.Empty() || !l.IsActive()) { continue; }

				for (Entity e : l.Entities()) {
					// Reached the end of Valid Entities in Array;
					if (e == INVALID_ENTITY) { break; }

					Transform& trans = m_coord.ModifyComponent<Transform>(e);
					RigidBody& rig = m_coord.ModifyComponent<RigidBody>(e);
					Collider& col = m_coord.ModifyComponent<Collider>(e);

					ComputeVelocity(rig, col);
					// BroadPhase;
					CollisionGridInsert(e, col, trans);
				}
				CheckCollisions(l.Entities());
			}



			for (Entity e2 : l.Entities()) {
				if (e == INVALID_ENTITY) { break; }
				if (e == e2) { continue; }

				if (CheckCollision(e, e2)) {
					ResolveCollisions(e, e2);
				}
			}












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
				auto& force = m_coord.ModifyComponent<RigidBody>(e).force;
				auto& vel = m_coord.ModifyComponent<RigidBody>(e).vel;
				auto& accel = m_coord.ModifyComponent<RigidBody>(e).accel;
				auto& mass = m_coord.ModifyComponent<RigidBody>(e).mass;
				bool g = m_coord.GetComponent<RigidBody>(e).useGravity;

				// Calculate acceleration from force and mass;
				accel = force.direction * 1000.0 / mass;

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

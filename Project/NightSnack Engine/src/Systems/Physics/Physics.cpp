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
#include <bit>

#include "Physics.h"
#include "Collision.h"
#include "Systems/PlayerController.h"

namespace NightSnack {

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

				// Resize collision grid for layer in current frame;
				m_collisionGrid.resize(m_gridCols, std::vector<std::bitset<MAX_ENTITIES>>(m_gridRows));
				m_cellSize = Vec2{ m_gridWidth / (float)m_gridCols, m_gridHeight / (float)m_gridRows };
				// Clear all bits in collision grid;
				std::for_each(std::begin(m_collisionGrid), std::end(m_collisionGrid),
					[](std::bitset<MAX_ENTITIES>& bits) { bits.reset(); });

				for (Entity e : l.Entities()) {
					// Reached the end of Valid Entities in Array;
					if (e == INVALID_ENTITY) { break; }

					Transform& trans = m_coord.ModifyComponent<Transform>(e);
					RigidBody& rig = m_coord.ModifyComponent<RigidBody>(e);
					Collider& col = m_coord.ModifyComponent<Collider>(e);

					// Update force's lifetime;
					ManageForces(rig.force);
					// Update velocities in RigidBody;
					ComputeVelocity(rig, col);
					// BroadPhase collision partitioning;
					CollisionGridInsert(e, col, trans);
				}
				ResolveCollisions();
			}



			//for (Entity e2 : l.Entities()) {
			//	if (e == INVALID_ENTITY) { break; }
			//	if (e == e2) { continue; }

			//	if (CheckCollision(e, e2)) {
			//		ResolveCollisions(e, e2);
			//	}
			//}

		}
		if (debugMode) stepRequest = false; // reset step request
	}

	// Helper functions to iterate through Collision Bitsets;
	static size_t FindFirst(const std::bitset<MAX_ENTITIES>& bits) {
		for (size_t i = 0; i < bits.size(); ++i)
			if (bits.test(i)) return i;
		return bits.size();
	}
	static size_t FindNext(const std::bitset<MAX_ENTITIES>& bits, size_t pos) {
		for (size_t i = pos + 1; i < bits.size(); ++i)
			if (bits.test(i)) return i;
		return bits.size();
	}

	void PhysicsSystem::ResolveCollisions() {

		for (int x = 0; x < m_gridCols; ++x) {
			for (int y = 0; y < m_gridRows; ++y) {
				std::bitset<MAX_ENTITIES>& cell = m_collisionGrid[x][y];

				for (size_t e1 = FindFirst(cell); e1 < MAX_ENTITIES; e1 = FindNext(cell, e1)) {
					for (size_t e2 = FindNext(cell, e1); e2 < MAX_ENTITIES; e2 = FindNext(cell, e2)) {
						
						Collider& col1 = m_coord.ModifyComponent<Collider>(e1);
						Collider& col2 = m_coord.ModifyComponent<Collider>(e2);

						// Skip if both entities are static bodies;
						if (col1.isStatic && col2.isStatic) { continue; }

						Transform& trans1 = m_coord.ModifyComponent<Transform>(e1);
						Transform& trans2 = m_coord.ModifyComponent<Transform>(e2);
						
						if (!CheckCollision(trans1, trans2, col1, col2)) { continue; }

						// perform resolutions;
						
					}
				}
			}
		}
	}

	bool PhysicsSystem::CheckCollision(Transform const& t1, Transform const& t2, Collider const& c1, Collider const& c2) {
		
		if (c1.shape == ShapeType::AABB && c2.shape == ShapeType::AABB) {
			return CheckAABBvsAABB(t1.pos, c1, t2.pos, c2);
		}
		else if (c1.shape == ShapeType::Circle && c2.shape == ShapeType::AABB) {
			return CheckCirclevsAABB(t1.pos, c1, t2.pos, c2);
		}
		else if (c1.shape == ShapeType::AABB && c2.shape == ShapeType::Circle) {
			return CheckCirclevsAABB(t2.pos, c2, t1.pos, c1);
		}
		else if (c1.shape == ShapeType::Circle && c2.shape == ShapeType::Circle) {
			return CheckCirclevsCircle(t1.pos, c1, t2.pos, c2);
		}
		// Currently testing only Circles and AABBs;
		return false;
	}

	void PhysicsSystem::ManageForces(Force& f) {
		if (!f.isActive) { return; }
		f.lifetime -= FixedDT;
		
		if (f.lifetime <= 0.f) {
			f.lifetime = 0.f;
			f.isActive = false;
			f.magnitude = 0.f;
			f.torque = 0.f;
		}
	}

	std::pair<int, int> PhysicsSystem::GetCell(Vec2 const& pos) const {
		Vec2 gridPos = pos - m_gridCenter;
		int cellX = std::floor(gridPos.x / m_cellSize.x);
		int cellY = std::floor(gridPos.y / m_cellSize.y);

		// Map -> 0 <= cellX <= m_gridWidth;
		cellX = static_cast<int>(cellX + m_gridWidth / 2.f);
		cellY = static_cast<int>(cellY + m_gridHeight / 2.f);

		return { cellX, cellY };
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

		for (int x = minCell.first; x <= maxCell.first; ++x) {
			for (int y = minCell.second; y <= maxCell.second; ++y) {
				if (x >= 0 && x < m_gridCols && y >= 0 && y < m_gridRows) {

					m_collisionGrid[x][y].set(e);
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

			if (std::abs(rig.vel.x) < velocityCutoff) {
				rig.vel.x = 0.0;
			}
		}
	}

}	// NightSnack

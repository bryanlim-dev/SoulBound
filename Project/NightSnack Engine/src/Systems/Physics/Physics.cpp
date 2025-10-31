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
				for (auto& col : m_collisionGrid) {
					for (auto& bits : col) {
						bits.reset();
					}
				}

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
		}
		if (debugMode) stepRequest = false; // reset step request
	}

	// Helper functions to iterate through Collision Bitsets;
	static Entity FindFirst(const std::bitset<MAX_ENTITIES>& bits) {
		for (Entity i = 0; i < static_cast<Entity>(bits.size()); ++i)
			if (bits.test(i)) return i;
		return static_cast<Entity>(bits.size());
	}
	static Entity FindNext(const std::bitset<MAX_ENTITIES>& bits, Entity pos) {
		for (Entity i = pos + 1; i < static_cast<Entity>(bits.size()); ++i)
			if (bits.test(i)) return i;
		return static_cast<Entity>(bits.size());
	}

	void PhysicsSystem::ResolveCollisions() {

		for (int x = 0; x < m_gridCols; ++x) {
			for (int y = 0; y < m_gridRows; ++y) {

				std::bitset<MAX_ENTITIES>& cell = m_collisionGrid[x][y];

				for (Entity e1 = FindFirst(cell); e1 < MAX_ENTITIES; e1 = FindNext(cell, e1)) {
					for (Entity e2 = FindNext(cell, e1); e2 < MAX_ENTITIES; e2 = FindNext(cell, e2)) {

						Collider& c1 = m_coord.ModifyComponent<Collider>(e1);
						Collider& c2 = m_coord.ModifyComponent<Collider>(e2);

						// Skip if first entity is a static body;
						if (c1.isStatic) { continue; }

						Transform& t1 = m_coord.ModifyComponent<Transform>(e1);
						Transform& t2 = m_coord.ModifyComponent<Transform>(e2);

						if (!CheckCollision(t1, t2, c1, c2)) { continue; }

						// perform resolutions;
						RigidBody& r1 = m_coord.ModifyComponent<RigidBody>(e1);
						RigidBody& r2 = m_coord.ModifyComponent<RigidBody>(e2);

						// Find point of intersection;
						Vec2 half1 = (c1.shape == ShapeType::AABB) ? Vec2{ c1.width * 0.5f, c1.height * 0.5f }
																	: Vec2{ c1.radius * 0.5f, c1.radius * 0.5f };
						Vec2 half2 = (c2.shape == ShapeType::AABB) ? Vec2{ c2.width * 0.5f, c2.height * 0.5f }
																	: Vec2{ c2.radius * 0.5f, c2.radius * 0.5f };

						Vec2 move = { t2.pos.x - t1.pos.x, t2.pos.y - t1.pos.y };
						Vec2 overlap = { half1.x + half2.x - std::fabs(move.x),
										half1.y + half1.y - std::fabs(move.y) };

						if (overlap.x < overlap.y) {
							float sign = (move.x < 0.f ? -1.f : 1.f);

							if (c2.isStatic) { t1.pos.x -= overlap.x * sign; }
							else {
								t1.pos.x -= overlap.x * 0.5f * sign;
								t2.pos.x += overlap.x * 0.5f * sign;
							}
							r1.vel.x = 0;
							r2.vel.x = 0;
						}
						else {
							float sign = (move.y < 0 ? -1.f : 1.f);

							if (c2.isStatic) { t1.pos.y -= overlap.x * sign; }
							else {
								t1.pos.y -= overlap.y * 0.5f * sign;
								t2.pos.y += overlap.y * 0.5f * sign;
							}
							r1.vel.y = 0;
							r2.vel.y = 0;
						}
						if (r1.vel.y == 0.f) { c1.isGrounded = true; }
						if (r2.vel.y == 0.f) { c2.isGrounded = true; }
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

		// Map -> 0 <= cellX <= m_gridWidth;
		int cellX = static_cast<int>(std::floor(gridPos.x / m_cellSize.x) + m_gridWidth / 2.f);
		int cellY = static_cast<int>(std::floor(gridPos.y / m_cellSize.y) + m_gridHeight / 2.f);

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

		static float time = 0.f;

		// Apply friction if colliding with object;
		if (col.colliding) {
			time += FixedDT;

			rig.vel.x *= 1.f - friction * FixedDT;
			rig.vel.y *= 1.f - friction * FixedDT;

			if (std::abs(rig.vel.x) < velocityCutoff) {
				rig.vel.x = 0.0;
			}
			if (std::abs(rig.vel.y) < velocityCutoff) {
				rig.vel.x = 0.0;
			}
		}
		else {
			time = 0.f;
		}
	}

}	// NightSnack

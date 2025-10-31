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

		for (int s{}; s < steps; ++s) {

			for (Layer& l : m_layers) {
				// Skip inactive and empty layers;
				if (l.Empty() || !l.IsActive()) { continue; }

				m_testedEntityPairs.clear();

				// Resize collision grid for layer in current frame;
				m_grid1.resize(m_gridCols);
				m_grid2.resize(m_gridRows);
				m_cellSize = Vec2{ m_gridWidth / (float)m_gridCols, m_gridHeight / (float)m_gridRows };
				for (auto& b : m_grid1) b.reset();
				for (auto& b : m_grid2) b.reset();

				for (Entity e : l.Entities()) {
					// Reached the end of Valid Entities in Array;
					if (e == INVALID_ENTITY) { break; }

					Transform& trans = m_coord.ModifyComponent<Transform>(e);
					RigidBody& rig = m_coord.ModifyComponent<RigidBody>(e);
					Collider& col = m_coord.ModifyComponent<Collider>(e);

					// Update force's lifetime;
					ManageForces(rig);
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
		return MAX_ENTITIES;
	}
	static Entity FindNext(const std::bitset<MAX_ENTITIES>& bits, Entity pos) {
		for (Entity i = pos + 1; i < static_cast<Entity>(bits.size()); ++i)
			if (bits.test(i)) return i;
		return MAX_ENTITIES;
	}
	static std::uint64_t PackEntityPair(Entity a, Entity b) {
		if (a > b) std::swap(a, b);
		return (static_cast<uint64_t>(a) << 32) | static_cast<uint64_t>(b);
	}

	void PhysicsSystem::ResolveCollisions() {
		for (size_t x{}; x < m_grid1.size(); ++x) {
			if (!m_grid1[x].any()) { continue; }

			for (size_t y{}; y < m_grid2.size(); ++y) {
				if (!m_grid2[y].any()) { continue; }

				std::bitset<MAX_ENTITIES> res = m_grid1[x] & m_grid2[y];
				if (!res.any()) { continue; }

				for (Entity e1 = FindFirst(res); e1 < MAX_ENTITIES; e1 = FindNext(res, e1)) {
					for (Entity e2 = FindNext(res, e1); e2 < MAX_ENTITIES; e2 = FindNext(res, e2)) {

						uint64_t key = PackEntityPair(e1, e2);
						if (!m_testedEntityPairs.insert(key).second) { continue; }
						if (e1 == e2) { continue; }

						Collider& c1 = m_coord.ModifyComponent<Collider>(e1);
						Collider& c2 = m_coord.ModifyComponent<Collider>(e2);

						// Skip if first entity is a static body;
						if (c1.isStatic) { continue; }

						// Check for collision in e1's next position;
						RigidBody& r1 = m_coord.ModifyComponent<RigidBody>(e1);
						RigidBody& r2 = m_coord.ModifyComponent<RigidBody>(e2);

						Transform& t1 = m_coord.ModifyComponent<Transform>(e1);
						Transform next1;
						next1.pos = Vec3{ t1.pos.x + r1.vel.x * FixedDT, t1.pos.y + r1.vel.y * FixedDT, t1.pos.z };
						Transform& t2 = m_coord.ModifyComponent<Transform>(e2);

						if (!CheckCollision(next1, t2, c1, c2)) { continue; }

						// Resolve collisions;
						Vec2 half1 = (c1.shape == ShapeType::AABB) ? Vec2{ c1.width * 0.5f, c1.height * 0.5f }
						: Vec2{ c1.radius, c1.radius };
						Vec2 half2 = (c2.shape == ShapeType::AABB) ? Vec2{ c2.width * 0.5f, c2.height * 0.5f }
						: Vec2{ c2.radius, c2.radius };

						Vec2 move = { t2.pos.x - t1.pos.x, t2.pos.y - t1.pos.y };
						Vec2 overlap = { half1.x + half2.x - std::fabs(move.x),
										half1.y + half2.y - std::fabs(move.y) };

						if (overlap.x < overlap.y) {
							float sign = (move.x < 0.f ? -1.f : 1.f);

							t1.pos.x -= (overlap.x + Z_EPSILON) * sign;
							r1.vel.x = 0;
							r2.vel.x = 0;
						}
						else {
							float sign = (move.y < 0 ? -1.f : 1.f);

							t1.pos.y -= (overlap.y + Z_EPSILON) * sign;
							r1.vel.y = 0;
							r2.vel.y = 0;

							if (r1.vel.y <= 0.f && t1.pos.y > t2.pos.y) {
								c1.isGrounded = true;
							}
						}
						if (!c2.isStatic) {
							Force f; Vec2 dir;
							if (move.x != 0.f || move.y != 0.f)
								dir = move.normalized();
							f.direction.x = dir.x;
							f.direction.y = dir.y;
							f.lifetime = FixedDT * 2.f;	// impulse force;
							f.magnitude = r1.accel.magnitude() / r1.invMass * 0.5f;
							f.isActive = true;
							ApplyForce(r2, f);
						}
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

	void PhysicsSystem::ManageForces(RigidBody& rig) {

		if (rig.forces.empty()) { return; }

		for (std::vector<Force>::iterator it = std::begin(rig.forces); it != std::end(rig.forces); ) {
			it->lifetime -= FixedDT;
			if (it->lifetime <= 0.f) {
				it = rig.forces.erase(it);  // erase() returns the next valid iterator
			}
			else {
				++it;
			}
		}
	}

	std::pair<int, int> PhysicsSystem::GetCell(Vec2 const& pos) const {
		Vec2 gridPos = pos - m_gridCenter;

		// Map -> 0 <= cellX <= m_gridWidth;
		int cellX = static_cast<int>(std::floor(gridPos.x / m_cellSize.x) + m_gridCols / 2.f);
		int cellY = static_cast<int>(std::floor(gridPos.y / m_cellSize.y) + m_gridRows / 2.f);

		return { cellX, cellY };
	}

	void PhysicsSystem::CollisionGridInsert(Entity e, Collider const& col, Transform const& t) {

		Vec2 min, max;

		if (col.shape == ShapeType::AABB) {
			min = Vec2{ t.pos.x - (col.width / 2.f), t.pos.y - (col.height / 2.f) };
			max = Vec2{ t.pos.x + (col.width / 2.f), t.pos.y + (col.height / 2.f) };
		}
		else if (col.shape == ShapeType::Circle) {
			min = Vec2{ t.pos.x - (col.radius), t.pos.y - col.radius };
			max = Vec2{ t.pos.x + (col.radius), t.pos.y + col.radius };
		}
		// find min and max cell;
		std::pair<int, int> minCell = GetCell(min);
		std::pair<int, int> maxCell = GetCell(max);

		for (int x = minCell.first; x <= maxCell.first; ++x) {
			if (x >= 0 && x < m_gridCols) {
				if (e >= MAX_ENTITIES) { continue; }
				m_grid1[x].set(static_cast<size_t>(e));
			}
		}
		for (int y = minCell.second; y <= maxCell.second; ++y) {
			if (y >= 0 && y < m_gridRows) {
				if (e >= MAX_ENTITIES) { continue; }
				m_grid2[y].set(static_cast<size_t>(e));
			}
		}
	}

	void PhysicsSystem::ApplyForce(RigidBody& r, Force const& f) {
		r.forces.push_back(f);
	}

	void PhysicsSystem::ComputeVelocity(RigidBody& rig, Collider const& col) const {

		// Reset Accel each frame;
		rig.accel = Vec2{ 0.f, 0.f };

		// Compute Accel;
		for (Force& f : rig.forces) {
			if (f.isActive) {
				rig.accel += (f.direction * f.magnitude) * rig.invMass;
			}
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

			if (std::fabs(rig.vel.x) < velocityCutoff) {
				rig.vel.x = 0.0;
			}
			if (std::fabs(rig.vel.y) < velocityCutoff) {
				rig.vel.y = 0.0;
			}
		}
		else {
			time = 0.f;
		}
	}

}	// NightSnack

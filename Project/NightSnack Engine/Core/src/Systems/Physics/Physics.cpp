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

					col.isGrounded = false;
					col.colliding = false;

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

		Entity player = m_coord.GetSystem<PlayerControllerSystem>()->GetPlayer();

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

						if (c1.isStatic) { continue; }

						RigidBody& r1 = m_coord.ModifyComponent<RigidBody>(e1);
						RigidBody& r2 = m_coord.ModifyComponent<RigidBody>(e2);

						Transform& t1 = m_coord.ModifyComponent<Transform>(e1);
						Transform& t2 = m_coord.ModifyComponent<Transform>(e2);

						// Check for collision in e1's next position;
						Transform next = t1;
						next.pos.x += r1.vel.x * FixedDT;
						next.pos.y += r1.vel.y * FixedDT;

						if (!CheckCollision(next, t2, c1, c2)) { continue; }
						c1.colliding = true;

						// Not colliding yet;
						next.pos = t1.pos;
						for (int i{}; i < 4; ++i) {
							next.pos.x += r1.vel.x * FixedDT * 0.25f;
							next.pos.y += r1.vel.y * FixedDT * 0.25f;
							if (CheckCollision(next, t2, c1, c2)) {
								next.pos.x -= r1.vel.x * FixedDT * 0.25f;
								next.pos.y -= r1.vel.y * FixedDT * 0.25f;
								break;
							}
						}
						t1.pos = next.pos;

						// Entities are already overlapping;
						if (CheckCollision(t1, t2, c1, c2)) {
							// Resolve overlaps;
							ResolveOverlap(t1, t2, c1, c2, r1, r2);
						}
						// Apply force to collided target;
						if (!c2.isStatic && e2 != player) {
							TransferForce(t1, t2, r1, r2);
						}
					}
				}
			}
		}
	}

	void PhysicsSystem::ResolveOverlap(Transform& t1, Transform& t2, Collider& c1, Collider& c2, RigidBody& r1, RigidBody& r2) {
		(void)r2;

		// Resolve collisions;
		Vec2 half1 = (c1.shape == ShapeType::AABB) ? Vec2{ c1.width * 0.5f, c1.height * 0.5f } : Vec2{ c1.radius, c1.radius };
		Vec2 half2 = (c2.shape == ShapeType::AABB) ? Vec2{ c2.width * 0.5f, c2.height * 0.5f } : Vec2{ c2.radius, c2.radius };

		Vec2 move = { t2.pos.x - t1.pos.x, t2.pos.y - t1.pos.y };
		Vec2 overlap = { half1.x + half2.x - std::fabs(move.x), half1.y + half2.y - std::fabs(move.y) };

		// Shift outside of each other; 
		Vec2 sign; 
		sign.x = move.x < 0.f ? -1.f : 1.f;
		sign.y = move.y < 0.f ? -1.f : 1.f;

		if (overlap.x < overlap.y) {
			t1.pos.x -= (overlap.x) * sign.x;
			r1.vel.x = 0.f;
		}
		else {
			t1.pos.y -= (overlap.y) * sign.y;
			r1.vel.y = 0.f;

			if (t1.pos.y > t2.pos.y) {
				c1.isGrounded = true;
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

		if (!rig.force.isActive) { return; }

		rig.force.lifetime -= FixedDT;

		if (rig.force.lifetime <= 0.f) {
			ResetForce(rig.force);
		}
	}
	void PhysicsSystem::TransferForce(Transform const& t1, Transform const& t2, RigidBody& r1, RigidBody& r2) {
		
		if (!r1.force.isActive) { return; }

		// Vector from e1 to e2;
		Vec3 dir = t2.pos - t1.pos;
		if (dir.magnitude() != 0.f) { dir.normalize(); }

		// Apply force to collide target ----------------------------
		r2.force.direction.x = dir.x;
		r2.force.direction.y = dir.y;
		r2.force.magnitude = (r2.force.magnitude * (1.f - r1.restitution) + r1.force.magnitude * r2.restitution);

		// Update this entity's force -------------------------------
		r1.force.direction.x = -dir.x;
		r1.force.direction.y = -dir.y;
		r1.force.magnitude = (r1.force.magnitude * (1.f - r2.restitution) + r2.force.magnitude * r1.restitution);

		// Update Velocities ----------------------------------------
		float mag = r1.vel.magnitude();
		r1.vel = r1.force.direction * mag * 0.8f;
		
		mag = r2.vel.magnitude();
		r2.vel = r2.force.direction * mag * 0.8f;
	}

	void PhysicsSystem::ApplyForce(RigidBody& r, Force const& f) {
		
		if (!f.isActive) { return; }

		// Calculate new force;
		Vec2 newforce = r.force.direction * r.force.magnitude;
		Vec2 apply = f.direction * f.magnitude;
		newforce += apply;
		float newlife = std::max(r.force.lifetime, f.lifetime);

		// Update newforce;
		if (newforce.magnitude() != 0.f) {
			r.force.direction = newforce.normalized();
			r.force.magnitude = newforce.magnitude() * r.restitution;
			r.force.lifetime = newlife;
			r.force.isActive = true;
		}
		else {
			r.force = Force{};
		}
	}
	void PhysicsSystem::ResetForce(Force& f) {
		f.direction = Vec2{};
		f.lifetime = 0.f;
		f.magnitude = 0.f;

		f.isActive = false;
	}

	void PhysicsSystem::ComputeVelocity(RigidBody& rig, Collider const& col) const {

		// Reset Accel each frame;
		rig.accel = Vec2{ 0.f, 0.f };
		if (col.isStatic) {
			rig.vel = Vec2{};
		}

		// Compute Accel;

		if (rig.force.isActive) {
			rig.accel += (rig.force.direction * rig.force.magnitude) * rig.invMass;
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

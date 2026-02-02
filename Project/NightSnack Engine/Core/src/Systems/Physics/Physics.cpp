/******************************************************************************
 * File:        Physics.cpp
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan (Primary)
 *
 * Description:
 * Defines the member functions of PhysicsSystem class.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include <algorithm>
#include <unordered_map>
#include <bit>

#include "Physics.h"
#include "Systems/Graphics.h"
#include "Collision.h"

namespace {
	using namespace NightSnack;

	bool CircleVsAABB(Transform const& tCircle, Transform const& tAABB,
		Collider const& cCircle, Collider const& cAABB,
		Vec2& normal, float& depth) {
		Vec2 halfSize = { cAABB.width * 0.5f, cAABB.height * 0.5f };

		// Find closest point on AABB to circle center
		Vec2 closest;
		closest.x = std::clamp(tCircle.pos.x, tAABB.pos.x - halfSize.x,
			tAABB.pos.x + halfSize.x);
		closest.y = std::clamp(tCircle.pos.y, tAABB.pos.y - halfSize.y,
			tAABB.pos.y + halfSize.y);

		Vec2 delta = tCircle.pos - closest;
		float distSq = delta.x * delta.x + delta.y * delta.y;

		if (distSq >= cCircle.radius * cCircle.radius) {
			return false; // No collision
		}

		float dist = sqrt(distSq);

		if (dist > 0.0001f) {
			depth = cCircle.radius - dist;
			normal = delta / dist;
		}
		else {
			// Circle center inside AABB - find closest edge
			Vec2 toEdge = tCircle.pos - tAABB.pos;
			float dx = halfSize.x - abs(toEdge.x);
			float dy = halfSize.y - abs(toEdge.y);

			if (dx < dy) {
				normal = Vec2{ toEdge.x > 0 ? 1.0f : -1.0f, 0.0f };
				depth = cCircle.radius + dx;
			}
			else {
				normal = Vec2{ 0.0f, toEdge.y > 0 ? 1.0f : -1.0f };
				depth = cCircle.radius + dy;
			}
		}
		return true;
	}
}

namespace NightSnack {

	// Main update function;
	void PhysicsSystem::Update() {
		if (m_coord.IsPaused()) return;

		// Skip Physics if in debug mode
		if (debugMode && !stepRequest) {
			return;
		}

		for (int s{}; s < m_coord.steps; ++s) {

			UpdateGridPosition();

			for (Layer& l : m_layers) {
				// Skip inactive and empty layers;
				if (l.Empty() || !l.IsActive()) { continue; }

				m_testedEntityPairs.clear();

				// Resize collision grid for layer in current frame;
				m_grid.resize(m_gridRows);
				for (std::vector<std::bitset<MAX_ENTITIES>>& row : m_grid) {
					row.resize(m_gridCols);
					for (std::bitset<MAX_ENTITIES>& bs : row) {
						bs.reset();
					}
				}
				
				//m_grid1.resize(m_gridCols);
				//m_grid2.resize(m_gridRows);
				//m_cellSize = Vec2{ m_gridWidth / (float)m_gridCols, m_gridHeight / (float)m_gridRows };
				//for (auto& b : m_grid1) b.reset();
				//for (auto& b : m_grid2) b.reset();

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

					// Reset colliders for this frame;
					col.isGrounded = false;
					col.colliding = false;
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
	void PhysicsSystem::UpdateGridPosition() {
		Entity cam = m_coord.GetSystem<GraphicsSystem>()->GetCamera();
		Transform const& t = m_coord.GetComponent<Transform>(cam);

		Vec2 dir = t.pos - m_gridCenter;

		float xStep = std::floor(dir.x / m_cellSize.x);
		float yStep = std::floor(dir.y / m_cellSize.y);

		m_gridCenter.x += xStep * m_cellSize.x;
		m_gridCenter.y += yStep * m_cellSize.y;

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

		for (int y = minCell.second; y <= maxCell.second; ++y) {
			for (int x = minCell.first; x <= maxCell.first; ++x) {
				if (y >= 0 && y < m_gridRows && x >= 0 && x < m_gridCols) {
					m_grid[y][x].set(static_cast<size_t>(e));
				}
			}
		}

		//for (int x = minCell.first; x <= maxCell.first; ++x) {
		//	if (x >= 0 && x < m_gridCols) {
		//		if (e >= MAX_ENTITIES) { continue; }
		//		m_grid1[x].set(static_cast<size_t>(e));
		//	}
		//}
		//for (int y = minCell.second; y <= maxCell.second; ++y) {
		//	if (y >= 0 && y < m_gridRows) {
		//		if (e >= MAX_ENTITIES) { continue; }
		//		m_grid2[y].set(static_cast<size_t>(e));
		//	}
		//}
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

		for (size_t x{}; x < m_gridCols; ++x) {
			for (size_t y{}; y < m_gridRows; ++y) {

				if (!m_grid[y][x].any()) { continue; }
				std::bitset<MAX_ENTITIES>& res = m_grid[y][x];

				for (Entity e1 = FindFirst(res); e1 < MAX_ENTITIES; e1 = FindNext(res, e1)) {
					for (Entity e2 = FindNext(res, e1); e2 < MAX_ENTITIES; e2 = FindNext(res, e2)) {

						uint64_t key = PackEntityPair(e1, e2);
						if (!m_testedEntityPairs.insert(key).second) { continue; }
						if (e1 == e2) { continue; }

						Collider& c1 = m_coord.ModifyComponent<Collider>(e1);
						Collider& c2 = m_coord.ModifyComponent<Collider>(e2);

						RigidBody& r1 = m_coord.ModifyComponent<RigidBody>(e1);
						RigidBody& r2 = m_coord.ModifyComponent<RigidBody>(e2);

						Transform& t1 = m_coord.ModifyComponent<Transform>(e1);
						Transform& t2 = m_coord.ModifyComponent<Transform>(e2);

						// Calculate collision normal and penetration depth first
						Vec2 normal; float depth;
						if (!CheckCollision(t1, t2, c1, c2, normal, depth)) { continue; }
						c1.colliding = c2.colliding = true;

						//if (!CheckCollision(t1, t2, c1, c2) && !CheckTunneling(t1, t2, r1) && !CheckTunneling(t2, t1, r2)) { continue; }

						if (c1.type != ColliderType::Trigger && c2.type != ColliderType::Trigger) {
							ResolveCollision(t1, t2, c1, c2, r1, r2, normal, depth);

							if (abs(normal.y) > abs(normal.x)) {
								if (t1.pos.y > t2.pos.y && !c1.isStatic) {
									r1.vel.y = 0.f;
									c1.isGrounded = true;
								}
								if (t2.pos.y > t1.pos.y && !c2.isStatic) {
									r2.vel.y = 0.f;
									c2.isGrounded = true;
								}
							}

						//	// Resolve overlaps;
						//	ResolveOverlap(t1, t2, c1, c2, r1, r2);

						//	// Not yet colliding, Check tunneling from e1;
						//	if (CheckTunneling(t1, t2, r1) && !c1.isStatic) {
						//		c1.colliding = c2.colliding = true;
						//		Transform next{};
						//		next.pos = t1.pos;

						//		for (int i{}; i < 4; ++i) {
						//			next.pos.x += r1.vel.x * FixedDT * 0.25f;
						//			next.pos.y += r1.vel.y * FixedDT * 0.25f;
						//			if (CheckCollision(next, t2, c1, c2)) {
						//				next.pos.x -= r1.vel.x * FixedDT * 0.25f;
						//				next.pos.y -= r1.vel.y * FixedDT * 0.25f;
						//				break;
						//			}
						//		}
						//		t1.pos = next.pos;
						//	}
						//	// Check tunneling from e2;
						//	else if (CheckTunneling(t2, t1, r2) && !c2.isStatic) {
						//		c1.colliding = c2.colliding = true;
						//		Transform next{};
						//		next.pos = t2.pos;

						//		for (int i{}; i < 4; ++i) {
						//			next.pos.x += r2.vel.x * FixedDT * 0.25f;
						//			next.pos.y += r2.vel.y * FixedDT * 0.25f;
						//			if (CheckCollision(t1, next, c1, c2)) {
						//				next.pos.x -= r2.vel.x * FixedDT * 0.25f;
						//				next.pos.y -= r2.vel.y * FixedDT * 0.25f;
						//				break;
						//			}
						//		}
						//		t2.pos = next.pos;
						//	}
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

		if (!c1.isStatic) {
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
		else if (!c2.isStatic) {
			Vec2 overlap = { half1.x + half2.x - std::fabs(move.x), half1.y + half2.y - std::fabs(move.y) };

			// Shift outside of each other; 
			Vec2 sign;
			sign.x = move.x < 0.f ? 1.f : -1.f;
			sign.y = move.y < 0.f ? 1.f : -1.f;

			if (overlap.x < overlap.y) {
				t2.pos.x -= (overlap.x) * sign.x;
				r2.vel.x = 0.f;
			}
			else {
				t2.pos.y -= (overlap.y) * sign.y;
				r2.vel.y = 0.f;

				if (t2.pos.y > t1.pos.y) {
					c2.isGrounded = true;
				}
			}
		}
		else {
			Vec2 overlap = { half1.x + half2.x - std::fabs(move.x), half1.y + half2.y - std::fabs(move.y) };

			// Shift outside of each other; 
			Vec2 sign;
			sign.x = move.x < 0.f ? 1.f : -1.f;
			sign.y = move.y < 0.f ? 1.f : -1.f;

			if (overlap.x < overlap.y) {
				t1.pos.x += (overlap.x) * sign.x * 0.5f;
				t2.pos.x -= (overlap.x) * sign.x * 0.5f;
				r1.vel.x = 0.f;
				r2.vel.x = 0.f;
			}
			else {
				t1.pos.y += (overlap.y) * sign.y * 0.5f;
				t2.pos.y -= (overlap.y) * sign.y * 0.5f;
				r1.vel.y = 0.f;
				r2.vel.y = 0.f;

				if (t1.pos.y > t2.pos.y) {
					c1.isGrounded = true;
				}
				if (t2.pos.y > t1.pos.y) {
					c2.isGrounded = true;
				}
			}
		}
	}

	bool PhysicsSystem::CheckCollision(Transform const& t1, Transform const& t2, Collider const& c1, Collider const& c2,
		Vec2& normal, float& depth) {

		if (c1.shape == ShapeType::Circle && c2.shape == ShapeType::Circle) {
			Vec2 delta = t2.pos - t1.pos;
			float distSq = delta.x * delta.x + delta.y * delta.y;
			float radiusSum = c1.radius + c2.radius;

			if (distSq >= radiusSum * radiusSum) {
				return false; // No collision
			}
			float dist = sqrt(distSq);
			depth = radiusSum - dist;

			if (dist > 0.0001f) {
				normal = delta / dist; // Normalize
			}
			else {
				normal = Vec2{ 1.0f, 0.0f }; // Arbitrary direction if overlapping exactly
			}
			return true;
		}
		else if (c1.shape == ShapeType::AABB && c2.shape == ShapeType::AABB) {
			Vec2 delta = t2.pos - t1.pos;

			// Half-extents
			Vec2 h1 = { c1.width * 0.5f, c1.height * 0.5f };
			Vec2 h2 = { c2.width * 0.5f, c2.height * 0.5f };
			// Overlap on each axis
			float overlapX = (h1.x + h2.x) - abs(delta.x);
			float overlapY = (h1.y + h2.y) - abs(delta.y);

			if (overlapX <= 0 || overlapY <= 0) {
				return false; // No collision
			}
			// Find axis of least penetration (collision normal)
			if (overlapX < overlapY) {
				depth = overlapX;
				normal = Vec2{ delta.x > 0 ? 1.0f : -1.0f, 0.0f };
			}
			else {
				depth = overlapY;
				normal = Vec2{ 0.0f, delta.y > 0 ? 1.0f : -1.0f };
			}
			return true;
		}
		else {
			// AABB vs Circle
			if (c1.shape == ShapeType::Circle) {
				return CircleVsAABB(t1, t2, c1, c2, normal, depth);
			}
			else {
				return CircleVsAABB(t2, t1, c2, c1, normal, depth);
			}
		}
	}

	void PhysicsSystem::ResolveCollision(Transform& t1, Transform& t2, Collider& c1, Collider& c2, RigidBody& r1, RigidBody& r2, Vec2 const& normal, float depth) {
		// --- Position Correction (separate objects) ---
		const float percent = 0.8f; // Penetration percentage to correct (0.2 - 0.8)
		const float slop = 0.01f; // Penetration allowance
		Vec2 correction = normal * std::max(depth - slop, 0.0f) * percent;

		if (c2.isStatic) {
			t1.pos.x = t1.pos.x - correction.x;
			t1.pos.y = t1.pos.y - correction.y;
		}
		else if (c1.isStatic) {
			t2.pos.x = t2.pos.x + correction.x;
			t2.pos.y = t2.pos.y + correction.y;
		}
		else {
			float mass1 = 1.f / r1.invMass;
			float mass2 = 1.f / r2.invMass;
			float totalMass = mass1 + mass2;
			t1.pos.x = t1.pos.x - correction.x * (mass2 / totalMass);
			t2.pos.x = t2.pos.x + correction.x * (mass1 / totalMass);

			if (t1.pos.y > t2.pos.y && r1.vel.y < 0) {
				t1.pos.y = t1.pos.y - correction.y * (mass2 / totalMass);
			}
			if (t2.pos.y > t1.pos.y && r2.vel.y < 0) {
				t2.pos.y = t2.pos.y + correction.y * (mass1 / totalMass);
			}
		}

		//// --- Velocity Resolution (impulse) ---
		//Vec2 relativeVel = r1.vel - r2.vel;
		//float velAlongNormal = relativeVel.dot(normal);

		//// Don't resolve if velocities are separating
		//if (velAlongNormal > 0) {
		//	return;
		//}

		//// Calculate restitution (bounciness)
		//float e = std::min(r1.restitution, r2.restitution);

		//// Calculate impulse scalar
		//float j = -(1.0f + e) * velAlongNormal;

		//if (!c1.isStatic && !c2.isStatic) {
		//	j /= (1.0f * r1.invMass + 1.0f * r2.invMass);
		//}
		//else if (c2.isStatic) {
		//	j /= (1.0f * r1.invMass);
		//}
		//else { // c1.isStatic
		//	j /= (1.0f * r2.invMass);
		//}

		//// Apply impulse
		//Vec2 impulse = normal * j;

		//if (!c1.isStatic) {
		//	r1.vel -= impulse * r1.invMass;
		//}
		//if (!c2.isStatic) {
		//	r2.vel += impulse * r2.invMass;
		//}
	}

	bool PhysicsSystem::CheckTunneling(Transform const& t1, Transform const& t2, RigidBody const& r1) {

		// Obtain vector from current e1 to e2;
		Vec2 dir{ t2.pos.x - t1.pos.x, t2.pos.y - t1.pos.y };
		if (dir.magnitude() != 0.f) { dir.normalize(); }
		
		// Obtain vector from next e1 to e2;
		Vec2 nextpos = { t1.pos.x + (r1.vel.x * FixedDT), t1.pos.y + (r1.vel.y * FixedDT) };
		Vec2 nextdir { t2.pos.x - nextpos.x, t2.pos.y - nextpos.y };
		if (nextdir.magnitude() != 0.f) { nextdir.normalize(); }

		// Normalized direction of travel of e1;
		Vec2 normal;
		if (r1.vel.magnitude() != 0.f)
			normal = r1.vel.normalized();
		else
			return false;	// No velocity, not possible to tunnel

		if (dir.dot(normal) * nextdir.dot(normal) <= 0.f) {
			return true;
		}
		return false;
	}

	//bool PhysicsSystem::CheckCollision(Transform const& t1, Transform const& t2, Collider const& c1, Collider const& c2) {
	//	
	//	if (c1.shape == ShapeType::AABB && c2.shape == ShapeType::AABB) {
	//		return CheckAABBvsAABB(t1.pos, c1, t2.pos, c2);
	//	}
	//	else if (c1.shape == ShapeType::Circle && c2.shape == ShapeType::AABB) {
	//		return CheckCirclevsAABB(t1.pos, c1, t2.pos, c2);
	//	}
	//	else if (c1.shape == ShapeType::AABB && c2.shape == ShapeType::Circle) {
	//		return CheckCirclevsAABB(t2.pos, c2, t1.pos, c1);
	//	}
	//	else if (c1.shape == ShapeType::Circle && c2.shape == ShapeType::Circle) {
	//		return CheckCirclevsCircle(t1.pos, c1, t2.pos, c2);
	//	}
	//	// Currently testing only Circles and AABBs;
	//	return false;
	//}

	void PhysicsSystem::ManageForces(RigidBody& rig) {

		if (!rig.force.isActive) { return; }

		rig.force.lifetime -= FixedDT;

		if (rig.force.lifetime <= 0.f) {
			ResetForce(rig.force);
		}
	}
	void PhysicsSystem::TransferForce(Transform const& t1, Transform const& t2, RigidBody& r1, RigidBody& r2, Collider& c1, Collider& c2) {
		
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
		float mag = 0.f;
		if (!c1.isStatic) {
			mag = r1.vel.magnitude();
			r1.vel = r1.force.direction * mag * 0.8f;
		}
		if (!c2.isStatic) {
			mag = r2.vel.magnitude();
			r2.vel = r2.force.direction * mag * 0.8f;
		}
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
			return;
		}

		// Compute Accel;
		if (rig.force.isActive) {
			rig.accel += (rig.force.direction * rig.force.magnitude) * rig.invMass;
		}

		if (rig.useGravity && !col.isGrounded) {
			rig.accel.y += gravity * rig.gravitymult;
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

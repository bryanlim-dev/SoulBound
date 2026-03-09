/******************************************************************************
 * File:        Collision.cpp
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 100%) - Collision system with respective entity response
 *
 * Description:
 * This file defines the collision system. It handles collision detection
 * and response with entities in the game. It supports dynamic and static
 * collision checks such as circle and circle, AABB and AABB, circle and AABB etc
 * Handles collision responses such as velocity flips and dampening.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "Collision.h"
#include <cmath>
#include <iostream>

namespace NightSnack {

    
	//void PhysicsSystem::ResolveCollisions() {

	//	for (int x = 0; x < m_gridCols; ++x) {
	//		for (int y = 0; y < m_gridRows; ++y) {

	//			std::bitset<MAX_ENTITIES>& cell = m_collisionGrid[x][y];

	//			for (Entity e1 = FindFirst(cell); e1 < MAX_ENTITIES; e1 = FindNext(cell, e1)) {
	//				for (Entity e2 = FindNext(cell, e1); e2 < MAX_ENTITIES; e2 = FindNext(cell, e2)) {

	//					Collider& c1 = m_coord.ModifyComponent<Collider>(e1);
	//					Collider& c2 = m_coord.ModifyComponent<Collider>(e2);

	//					// Skip if first entity is a static body;
	//					if (c1.isStatic) { continue; }

	//					Transform& t1 = m_coord.ModifyComponent<Transform>(e1);
	//					Transform& t2 = m_coord.ModifyComponent<Transform>(e2);

	//					if (!CheckCollision(t1, t2, c1, c2)) { continue; }

	//					// perform resolutions;
	//					RigidBody& r1 = m_coord.ModifyComponent<RigidBody>(e1);
	//					RigidBody& r2 = m_coord.ModifyComponent<RigidBody>(e2);

	//					// Find point of intersection;
	//					Vec2 half1 = (c1.shape == ShapeType::AABB) ? Vec2{ c1.width * 0.5f, c1.height * 0.5f }
	//																: Vec2{ c1.radius * 0.5f, c1.radius * 0.5f };
	//					Vec2 half2 = (c2.shape == ShapeType::AABB) ? Vec2{ c2.width * 0.5f, c2.height * 0.5f }
	//																: Vec2{ c2.radius * 0.5f, c2.radius * 0.5f };

	//					Vec2 move = { t2.pos.x - t1.pos.x, t2.pos.y - t1.pos.y };
	//					Vec2 overlap = { half1.x + half2.x - std::fabs(move.x),
	//									half1.y + half1.y - std::fabs(move.y) };

	//					if (overlap.x < overlap.y) {
	//						float sign = (move.x < 0.f ? -1.f : 1.f);

	//						if (c2.isStatic) { t1.pos.x -= overlap.x * sign; }
	//						else {
	//							t1.pos.x -= overlap.x * 0.5f * sign;
	//							t2.pos.x += overlap.x * 0.5f * sign;
	//						}
	//						r1.vel.x = 0;
	//						r2.vel.x = 0;
	//					}
	//					else {
	//						float sign = (move.y < 0 ? -1.f : 1.f);

	//						if (c2.isStatic) { t1.pos.y -= overlap.x * sign; }
	//						else {
	//							t1.pos.y -= overlap.y * 0.5f * sign;
	//							t2.pos.y += overlap.y * 0.5f * sign;
	//						}
	//						r1.vel.y = 0;
	//						r2.vel.y = 0;
	//					}
	//					if (r1.vel.y == 0.f) { c1.isGrounded = true; }
	//					if (r2.vel.y == 0.f) { c2.isGrounded = true; }
	//				}
	//			}
	//		}
	//	}
	//}

    Vec3 FindIntersectionPoint(Collider const& c1, Collider const& c2, Transform const& t1, Transform const& t2) {
        Vec3 interPT{ t1.pos };
        Vec3 dir = t2.pos - t1.pos;
        dir.normalize();

        if (c2.shape == ShapeType::AABB) {
            if (dir.x < 0) {

            }
        }
        
        if (c2.shape == ShapeType::Circle) {
            interPT = t2.pos - (dir * (c2.radius + Z_EPSILON));
        }
        if (c1.shape == ShapeType::Circle) {
            interPT = t1.pos + (dir * (c1.radius - Z_EPSILON));
        }
        else if (c2.shape == ShapeType::AABB) {
            if (c1.shape == ShapeType::AABB) {
                interPT.x = std::clamp(t2.pos.x, (t1.pos.x - c1.width / 2.f), (t1.pos.x + c1.width / 2.f)) - Z_EPSILON;
                interPT.y = std::clamp(t2.pos.y, (t1.pos.y - c1.height / 2.f), (t1.pos.y + c1.height / 2.f)) - Z_EPSILON;
                interPT.z = t1.pos.z;
            }
        }
        return interPT;
    }

    // Checks if velocity needs to be flipped;
    bool ShouldFlipVelocity(float pos1, float pos2, float vel1, float dt)
    {
        float newpos = pos1 + vel1 * dt;
        float distsq = (pos2 - pos1) * (pos2 - pos1);
        float newdistsq = (pos2 - newpos) * (pos2 - newpos);

        // If the current velocity will bring the two objects closer;
        if (newdistsq < distsq)
        {
            // Should flip the velocity;
            return true;
        }
        else {
            return false;
        }
    }

    bool CheckEntityCollision(const Vec3& aPos, const Collider& aCol, const Vec3& bPos, const Collider& bCol)
    {

        int circleCount = 0;
        bool aColIsCircle = false;

        if (aCol.shape == ShapeType::Circle)
        {
            circleCount++;
            aColIsCircle = true;
        }
        if (bCol.shape == ShapeType::Circle)
            circleCount++;

        bool colliding = false;
        if (circleCount == 0) //Both AABB
            colliding = CheckAABBvsAABB(aPos, aCol, bPos, bCol);
        else if (circleCount == 1) //1 is circle 1 is AABB
        {
            if (aColIsCircle)
                colliding = CheckCirclevsAABB(aPos, aCol, bPos, bCol);
            else
                colliding = CheckCirclevsAABB(bPos, bCol, aPos, aCol);
        }
        else //Both is circle
            colliding = CheckCirclevsCircle(aPos, aCol, bPos, bCol);

        return colliding;
    }

    // Helper Functions

    // Axis-Aligned Bounding Box vs AABB
    bool CheckAABBvsAABB(const Vec3& aPos, const Collider& a,
        const Vec3& bPos, const Collider& b)
    {
        return (std::abs(aPos.x - bPos.x) * 2 <= (a.width + b.width)) &&
            (std::abs(aPos.y - bPos.y) * 2 <= (a.height + b.height));
    }

    // Circle vs Circle
    bool CheckCirclevsCircle(const Vec3& aPos, const Collider& a,
        const Vec3& bPos, const Collider& b)
    {
        float dx = aPos.x - bPos.x;
        float dy = aPos.y - bPos.y;
        float distSq = dx * dx + dy * dy;
        float radiusSum = a.radius + b.radius;

        return distSq <= radiusSum * radiusSum;
    }

    // Circle vs AABB
    bool CheckCirclevsAABB(const Vec3& circlePos, const Collider& circle,
        const Vec3& aabbPos, const Collider& aabb)
    {
        float halfW = aabb.width / 2.0f;
        float halfH = aabb.height / 2.0f;

        // AABB bounds
        float left = aabbPos.x - halfW;
        float right = aabbPos.x + halfW;
        float top = aabbPos.y + halfH;
        float bottom = aabbPos.y - halfH;

        // Find closest point on AABB to circle center
        float closestX = Clamp(circlePos.x, left, right);
        float closestY = Clamp(circlePos.y, bottom, top);

        // Distance from circle center to closest point
        float dx = circlePos.x - closestX;
        float dy = circlePos.y - closestY;
        float distanceSquared = dx * dx + dy * dy;
        float radiusSquared = circle.radius * circle.radius;

        // Add some debug output for AABB collisions
        bool collision = distanceSquared <= radiusSquared;

        return collision;
    }

    // Clamp helper
    float Clamp(float value, float min, float max)
    {
        return std::max(min, std::min(value, max));
    }

}   // NightSnack

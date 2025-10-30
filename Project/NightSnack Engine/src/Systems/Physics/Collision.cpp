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

    // Helper Functions

    // Axis-Aligned Bounding Box vs AABB
    bool CheckAABBvsAABB(const Vec3& aPos, const Collider& a,
        const Vec3& bPos, const Collider& b)
    {
        return (std::abs(aPos.x - bPos.x) * 2 < (a.width + b.width)) &&
            (std::abs(aPos.y - bPos.y) * 2 < (a.height + b.height));
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

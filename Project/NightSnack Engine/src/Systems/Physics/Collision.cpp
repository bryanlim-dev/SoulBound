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

    static float dt = 0.016f;

    // Initializing Collision System
    void CollisionSystem::Init(EventDispatcher& dispatcher)
    {
        m_dispatcher = &dispatcher;

        // Testing code for collision event subscibtion (subscribe only in init)
        m_dispatcher->Subscribe<CollisionEvent>([](const CollisionEvent& event)
            {
                (void)event; // temp unreferenced;

                //Play sound, spawn particles, update score, etc.
                //std::cout << "Collision event: Entity " << event.entityA << " collided with Entity " << event.entityB << std::endl;
            });
    }

    // Update loop (runs each frame)
    void CollisionSystem::Update(int steps)
    {

        // Step 1: Reset collision flags
        for (auto const& e : m_entities)
        {
            auto& col = m_coord.ModifyComponent<Collider>(e);
            col.colliding = false;
        }

        // Step 3: Check for collisions and resolve them
        for (auto itA = m_entities.begin(); itA != m_entities.end(); ++itA)
        {
            for (auto itB = m_entities.begin(); itB != m_entities.end(); ++itB)
            {
                if (itB == itA) continue;

                auto& posA = m_coord.GetComponent<Transform>(*itA).pos;
                auto& velA = m_coord.ModifyComponent<RigidBody>(*itA).vel;
                auto& colA = m_coord.ModifyComponent<Collider>(*itA);

                auto& posB = m_coord.GetComponent<Transform>(*itB).pos;
                auto& velB = m_coord.ModifyComponent<RigidBody>(*itB).vel;
                auto& colB = m_coord.ModifyComponent<Collider>(*itB);

                // Skip if both are static
                if (colA.isStatic) continue;

                bool collision = false;
                Vec3 nextPosA{ posA.x + velA.x * dt, posA.y + velA.y * dt };

                // Check collision
                if (colA.shape == ShapeType::AABB && colB.shape == ShapeType::AABB)
                {
                    collision = CheckAABBvsAABB(nextPosA, colA, posB, colB);
                }
                else if (colA.shape == ShapeType::Circle && colB.shape == ShapeType::Circle)
                {
                    collision = CheckCirclevsCircle(nextPosA, colA, posB, colB);
                }
                else if (colA.shape == ShapeType::Circle && colB.shape == ShapeType::AABB)
                {
                    collision = CheckCirclevsAABB(nextPosA, colA, posB, colB);
                }
                else if (colA.shape == ShapeType::AABB && colB.shape == ShapeType::Circle)
                {
                    collision = CheckCirclevsAABB(posB, colB, nextPosA, colA);
                }

                if (collision)
                {
                    colA.colliding = true;
                    colB.colliding = true;

                    //std::cout << "Collision detected between Entity " << *itA << " and Entity " << *itB << std::endl;

                    // Dispatch collision event between entity A & entity B
                    if (m_dispatcher)
                    {
                        m_dispatcher->QueueEvent(std::make_unique<CollisionEvent>(*itA, *itB));
                    }

                    // Separate objects first
                    // SeparateObjects(posA, colA, posB, colB, velA, velB);


                    bool flipVelx = ShouldFlipVelocity(posA.x, posB.x, velA.x, dt);
                    bool flipVely = ShouldFlipVelocity(posA.y, posB.y, velA.y, dt);

                    // Handle collision response
                    if (!colA.isStatic && !colB.isStatic)
                    {
                        // Dynamic vs Dynamic - both bounce with 30% energy loss

                        velA.x = flipVelx ? -velA.x * 0.7f : velA.x;
                        velA.y = flipVely ? -velA.y * 0.7f : velA.y;
                        velB.x = -velA.x * 0.7f;
                        velB.y = -velB.y * 0.7f;

                    }
                    else if (!colA.isStatic && colB.isStatic)
                    {
                        if (colB.type == ColliderType::Ground)
                        {
                            if (velA.y < 0.0f)  // Moving downward
                            {
                                // Progressive damping - reduce bounciness with each bounce
                                static std::unordered_map<Entity, int> bounceCount;
                                bounceCount[*itA]++;

                                float dynamicBounciness = bounciness * static_cast<float>(std::pow(0.8f, bounceCount[*itA]));
                                velA.y = -velA.y * dynamicBounciness;

                                if (std::abs(velA.y) < minBounceSpeed)
                                {
                                    velA.y = 0.0f;
                                    colA.isGrounded = true;
                                    bounceCount[*itA] = 0; // Reset counter
                                    //std::cout << "ball stopped after " << bounceCount[*itA] << " bounces" << std::endl;
                                }
                                else
                                {
                                    colA.isGrounded = false;
                                    //std::cout << "ball bounced #" << bounceCount[*itA] << " with velocity: " << velA.y << std::endl;
                                }
                            }
                        }
                        else {
                            // A is dynamic, B is static (wall/other) - bounce with damping
                            velA.x = flipVelx ? -velA.x * 0.7f : velA.x;
                            velA.y = flipVely ? -velA.y * 0.7f : velA.y;
                        }
                    }
                    //else if (colA.isStatic && !colB.isStatic)             // <- technically not necessary;
                    //{
                    //    // A is static, B is dynamic - B bounces with 30% energy loss
                    //    velB.dx = -velB.dx * 0.7;
                    //    velB.dy = -velB.dy * 0.7;
                    //}

                    // Check velocities after collision response
                    if (*itA == 0)
                    {
                        //std::cout << "P1 velocity after response: (" << velA.dx << ", " << velA.dy << ")" << std::endl;
                    }
                }
            }
        }
    }

    // Checks if velocity needs to be flipped;
    bool CollisionSystem::ShouldFlipVelocity(float pos1, float pos2, float vel1, float dt)
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
    bool CollisionSystem::CheckAABBvsAABB(const Vec3& aPos, const Collider& a,
        const Vec3& bPos, const Collider& b)
    {
        return (std::abs(aPos.x - bPos.x) * 2 < (a.width + b.width)) &&
            (std::abs(aPos.y - bPos.y) * 2 < (a.height + b.height));
    }

    // Circle vs Circle
    bool CollisionSystem::CheckCirclevsCircle(const Vec3& aPos, const Collider& a,
        const Vec3& bPos, const Collider& b)
    {
        float dx = aPos.x - bPos.x;
        float dy = aPos.y - bPos.y;
        float distSq = dx * dx + dy * dy;
        float radiusSum = a.radius + b.radius;

        return distSq <= radiusSum * radiusSum;
    }

    // Circle vs AABB
    bool CollisionSystem::CheckCirclevsAABB(const Vec3& circlePos, const Collider& circle,
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
    float CollisionSystem::Clamp(float value, float min, float max)
    {
        return std::max(min, std::min(value, max));
    }

}   // NightSnack

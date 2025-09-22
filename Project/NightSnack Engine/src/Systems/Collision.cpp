#include "Collision.h"
#include <cmath>
#include <iostream>

// Constructor
CollisionSystem::CollisionSystem()
{
    // Register the components this system needs:
    // Position, Velocity, and Collider
    m_systemSignature.set(GetComponentTypeID<Position>());
    m_systemSignature.set(GetComponentTypeID<Velocity>());
    m_systemSignature.set(GetComponentTypeID<Collider>());
}

// Update loop (runs each frame)
void CollisionSystem::Update(float dt, Coordinator& coord)
{
    // Step 1: Reset collision flags
    for (auto const& e : m_entities) 
    {
        auto& col = coord.GetComponent<Collider>(e);
        col.colliding = false;
    }

    // Step 2: Move all dynamic entities first
    for (auto const& e : m_entities) 
    {
        auto& pos = coord.GetComponent<Position>(e);
        auto& vel = coord.GetComponent<Velocity>(e);
        auto& col = coord.GetComponent<Collider>(e);

        // Skip static entities
        if (col.isStatic) continue;

        // Move the entity
        pos.x += vel.dx * dt;
        pos.y += vel.dy * dt;

        // Debug output for p1
        if (e == 0) {
            //std::cout << "P1 Position: (" << pos.x << ", " << pos.y << "), Velocity: (" << vel.dx << ", " << vel.dy << ")" << std::endl;
        }
    }

    // Step 3: Check for collisions and resolve them
    for (auto itA = m_entities.begin(); itA != m_entities.end(); ++itA)
    {
        for (auto itB = std::next(itA); itB != m_entities.end(); ++itB)
        {
            auto& posA = coord.GetComponent<Position>(*itA);
            auto& velA = coord.GetComponent<Velocity>(*itA);
            auto& colA = coord.GetComponent<Collider>(*itA);

            auto& posB = coord.GetComponent<Position>(*itB);
            auto& velB = coord.GetComponent<Velocity>(*itB);
            auto& colB = coord.GetComponent<Collider>(*itB);

            // Skip if both are static
            if (colA.isStatic) continue;

            bool collision = false;

            // Check collision
            if (colA.shape == ShapeType::AABB && colB.shape == ShapeType::AABB)
            {
                collision = CheckAABBvsAABB(posA, colA, posB, colB);
            }
            else if (colA.shape == ShapeType::Circle && colB.shape == ShapeType::Circle)
            {
                collision = CheckCirclevsCircle(posA, colA, posB, colB);
            }
            else if (colA.shape == ShapeType::Circle && colB.shape == ShapeType::AABB)
            {
                collision = CheckCirclevsAABB(posA, colA, posB, colB);
            }
            else if (colA.shape == ShapeType::AABB && colB.shape == ShapeType::Circle)
            {
                collision = CheckCirclevsAABB(posB, colB, posA, colA);
            }

            if (collision)
            {
                colA.colliding = true;
                colB.colliding = true;

                std::cout << "Collision detected between Entity " << *itA << " and Entity " << *itB << std::endl;

                // Separate objects first
                // SeparateObjects(posA, colA, posB, colB, velA, velB);

                // Debug: Check velocities before collision response
                if (*itA == 0) {
                    //std::cout << "P1 velocity before response: (" << velA.dx << ", " << velA.dy << ")" << std::endl;
                }

                bool flipVel = ShouldFlipVelocity(posA.x, posB.x, velA.dx, dt);

                // Handle collision response
                if (!colA.isStatic && !colB.isStatic)
                {
                    // Dynamic vs Dynamic - both bounce with 30% energy loss
                   
                    velA.dx = flipVel ? -velA.dx * 0.7f : velA.dx;
                    velA.dy = flipVel ? -velA.dy * 0.7f : velA.dy;
                    velB.dx = -velA.dx * 0.7f;
                    velB.dy = -velB.dy * 0.7f;

                    /*velA.dx = -velA.dx * 0.7;
                    velA.dy = -velA.dy * 0.7;
                    velB.dx = -velB.dx * 0.7;
                    velB.dy = -velB.dy * 0.7;*/
                }
                else if (!colA.isStatic && colB.isStatic)
                {
                    // A is dynamic, B is static - A bounces with 30% energy loss
                    velA.dx = flipVel ? -velA.dx * 0.7 : velA.dx;
                    velA.dy = flipVel ? -velA.dy * 0.7 : velA.dy;
                }
                //else if (colA.isStatic && !colB.isStatic)             // <- technically not necessary;
                //{
                //    // A is static, B is dynamic - B bounces with 30% energy loss
                //    velB.dx = -velB.dx * 0.7;
                //    velB.dy = -velB.dy * 0.7;
                //}

                // Check velocities after collision response
                if (*itA == 0) {
                    //std::cout << "P1 velocity after response: (" << velA.dx << ", " << velA.dy << ")" << std::endl;
                }
            }
        }
    }
}

// Checks if velocity needs to be flipped;
bool CollisionSystem::ShouldFlipVelocity(float pos1, float pos2, float vel1, float dt) {
    float newpos = pos1 + vel1 * dt;
    float distsq = (pos2 - pos1) * (pos2 - pos1);
    float newdistsq = (pos2 - newpos) * (pos2 - newpos);

    // If the current velocity will bring the two objects closer;
    if (newdistsq < distsq) {
        // Should flip the velocity;
        return true;
    }
    else {
        return false;
    }
}

// Helper Functions

// Axis-Aligned Bounding Box vs AABB
bool CollisionSystem::CheckAABBvsAABB(const Position& aPos, const Collider& a,
    const Position& bPos, const Collider& b)
{
    return (std::abs(aPos.x - bPos.x) * 2 < (a.width + b.width)) &&
        (std::abs(aPos.y - bPos.y) * 2 < (a.height + b.height));
}

// Circle vs Circle
bool CollisionSystem::CheckCirclevsCircle(const Position& aPos, const Collider& a,
    const Position& bPos, const Collider& b)
{
    float dx = aPos.x - bPos.x;
    float dy = aPos.y - bPos.y;
    float distSq = dx * dx + dy * dy;
    float radiusSum = a.radius + b.radius;

    return distSq <= radiusSum * radiusSum;
}

// Circle vs AABB
bool CollisionSystem::CheckCirclevsAABB(const Position& circlePos, const Collider& circle,
    const Position& aabbPos, const Collider& aabb)
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

    /*
    if (collision) {
        std::cout << "Circle-AABB collision: Circle at (" << circlePos.x << ", " << circlePos.y
            << ") radius " << circle.radius << ", AABB at (" << aabbPos.x << ", " << aabbPos.y
            << ") size " << aabb.width << "x" << aabb.height << std::endl;
        std::cout << "Closest point: (" << closestX << ", " << closestY << "), distance: "
            << std::sqrt(distanceSquared) << std::endl;
    }
    */

    return collision;
}

// Clamp helper
float CollisionSystem::Clamp(float value, float min, float max)
{
    return std::max(min, std::min(value, max));
}
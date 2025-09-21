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
            if (colA.isStatic && colB.isStatic) continue;

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
                SeparateObjects(posA, colA, posB, colB, velA, velB);

                // Debug: Check velocities before collision response
                if (*itA == 0) {
                    //std::cout << "P1 velocity before response: (" << velA.dx << ", " << velA.dy << ")" << std::endl;
                }

                // Handle collision response
                if (!colA.isStatic && !colB.isStatic)
                {
                    // Dynamic vs Dynamic - both bounce with 30% energy loss
                    velA.dx = -velA.dx * 0.7;
                    velA.dy = -velA.dy * 0.7;
                    velB.dx = -velB.dx * 0.7;
                    velB.dy = -velB.dy * 0.7;
                }
                else if (!colA.isStatic && colB.isStatic)
                {
                    // A is dynamic, B is static - A bounces with 30% energy loss
                    velA.dx = -velA.dx * 0.7;
                    velA.dy = -velA.dy * 0.7;
                }
                else if (colA.isStatic && !colB.isStatic)
                {
                    // A is static, B is dynamic - B bounces with 30% energy loss
                    velB.dx = -velB.dx * 0.7;
                    velB.dy = -velB.dy * 0.7;
                }

                // Check velocities after collision response
                if (*itA == 0) {
                    //std::cout << "P1 velocity after response: (" << velA.dx << ", " << velA.dy << ")" << std::endl;
                }
            }
        }
    }
}

// Helper function to check if an object is moving away from another object
bool CollisionSystem::IsMovingAway(const Position& pos1, const Position& pos2, const Velocity& vel1)
{
    // Calculate vector from pos1 to pos2
    float dx = pos2.x - pos1.x;
    float dy = pos2.y - pos1.y;

    // Calculate dot product of direction vector and velocity vector
    float dotProduct = dx * vel1.dx + dy * vel1.dy;

    // If dot product is negative, we're moving away from the other object
    return dotProduct < 0;
}

// Helper function to separate overlapping objects
void CollisionSystem::SeparateObjects(Position& posA, const Collider& colA,
    Position& posB, const Collider& colB,
    const Velocity& velA, const Velocity& velB)
{
    if (colA.shape == ShapeType::Circle && colB.shape == ShapeType::Circle)
    {
        SeparateCircles(posA, colA, posB, colB, velA, velB);
    }
    else if (colA.shape == ShapeType::Circle && colB.shape == ShapeType::AABB)
    {
        SeparateCircleAABB(posA, colA, posB, colB, true);
    }
    else if (colA.shape == ShapeType::AABB && colB.shape == ShapeType::Circle)
    {
        SeparateCircleAABB(posB, colB, posA, colA, false);
    }
}

// Separate two overlapping circles
void CollisionSystem::SeparateCircles(Position& posA, const Collider& colA,
    Position& posB, const Collider& colB,
    const Velocity& velA, const Velocity& velB)
{
    float dx = posA.x - posB.x;
    float dy = posA.y - posB.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    // Handle case where circles are exactly on top of each other
    if (distance < 0.001f) {
        dx = 1.0f;
        dy = 0.0f;
        distance = 1.0f;
    }

    float totalRadius = colA.radius + colB.radius;
    float overlap = totalRadius - distance;

    if (overlap > 0) {
        // Normalize direction vector
        dx /= distance;
        dy /= distance;

        // Calculate how much to move each object
        if (!colA.isStatic && !colB.isStatic) 
        {
            // Both dynamic - move both
            float moveA = overlap * 0.51f;
            float moveB = overlap * 0.51f;

            posA.x += dx * moveA;
            posA.y += dy * moveA;
            posB.x -= dx * moveB;
            posB.y -= dy * moveB;
        }
        else if (!colA.isStatic && colB.isStatic) 
        {
            // Only A is dynamic
            posA.x += dx * (overlap + 2.0f);
            posA.y += dy * (overlap + 2.0f);
        }
        else if (colA.isStatic && !colB.isStatic) 
        {
            // Only B is dynamic
            posB.x -= dx * (overlap + 2.0f);
            posB.y -= dy * (overlap + 2.0f);
        }
    }
}

// Separate circle from AABB
void CollisionSystem::SeparateCircleAABB(Position& circlePos, const Collider& circle,
    Position& aabbPos, const Collider& aabb, bool circleIsA)
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

    // Vector from closest point to circle center
    float dx = circlePos.x - closestX;
    float dy = circlePos.y - closestY;
    float distance = std::sqrt(dx * dx + dy * dy);

    //std::cout << "SeparateCircleAABB: Circle at (" << circlePos.x << ", " << circlePos.y << "), distance: " << distance << std::endl;

    if (distance < 0.001f) {
        // Circle center is inside AABB, push it out in the direction of least penetration
        float penetrationLeft = circlePos.x - left;
        float penetrationRight = right - circlePos.x;
        float penetrationTop = top - circlePos.y;
        float penetrationBottom = circlePos.y - bottom;

        float minPenetration = std::min({ penetrationLeft, penetrationRight, penetrationTop, penetrationBottom });

        //std::cout << "Circle inside AABB, min penetration: " << minPenetration << std::endl;

        if (minPenetration == penetrationLeft) 
        {
            circlePos.x = left - circle.radius - 5.0f;
            //std::cout << "Pushed left to x = " << circlePos.x << std::endl;
        }
        else if (minPenetration == penetrationRight) 
        {
            circlePos.x = right + circle.radius + 5.0f;
            //std::cout << "Pushed right to x = " << circlePos.x << std::endl;
        }
        else if (minPenetration == penetrationTop) 
        {
            circlePos.y = top + circle.radius + 5.0f;
            //std::cout << "Pushed up to y = " << circlePos.y << std::endl;
        }
        else {
            circlePos.y = bottom - circle.radius - 5.0f;
            //std::cout << "Pushed down to y = " << circlePos.y << std::endl;
        }
    }
    else {
        // Normal case - push circle away from closest point
        float overlap = circle.radius - distance;
        if (overlap > 0) 
        {
            dx /= distance; // Normalize
            dy /= distance;

            //std::cout << "Normal separation, overlap: " << overlap << std::endl;

            // Only move the circle (AABB is static in your case)
            circlePos.x += dx * (overlap + 5.0f); // Increased buffer
            circlePos.y += dy * (overlap + 5.0f);

            //std::cout << "Moved circle to (" << circlePos.x << ", " << circlePos.y << ")" << std::endl;
        }
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
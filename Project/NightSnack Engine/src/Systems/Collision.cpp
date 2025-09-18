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
void CollisionSystem::Update(double dt, Coordinator& coord)
{
    // Step 1: Move all dynamic (non-static) entities and
    // reset their collision state at the start of each frame
    for (auto const& e : m_entities) {
        auto& pos = coord.GetComponent<Position>(e);
        auto& vel = coord.GetComponent<Velocity>(e);
        auto& col = coord.GetComponent<Collider>(e);

        //// If the entity is dynamic, update its position using velocity * dt
        //if (!col.isStatic) 
        //{
        //    pos.x += vel.dx * dt;
        //    pos.y += vel.dy * dt;
        //}

        // Reset collision flag before checking this frame
        col.colliding = false;
    }

    // Step 2: Check collisions between all pairs of entities
    for (auto itA = m_entities.begin(); itA != m_entities.end(); ++itA) 
    {
        for (auto itB = std::next(itA); itB != m_entities.end(); ++itB) 
        {
            // Get references to both entities' components
            auto& posA = coord.GetComponent<Position>(*itA);
            auto& velA = coord.GetComponent<Velocity>(*itA);
            auto& colA = coord.GetComponent<Collider>(*itA);

            auto& posB = coord.GetComponent<Position>(*itB);
            auto& velB = coord.GetComponent<Velocity>(*itB);
            auto& colB = coord.GetComponent<Collider>(*itB);

            // Skip check if both objects are static
            if (colA.isStatic && colB.isStatic) continue;

            bool collision = false;

            // Dispatch collision check based on collider shapes
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

            // If a collision was detected
            if (collision) 
            {
                colA.colliding = true;
                colB.colliding = true;

                std::cout << "Collision detected between Entity "
                    << *itA << " and Entity " << *itB << "\n";
            }

            // Collision Response
            if (collision) 
            {
                if (!colA.isStatic && !colB.isStatic) 
                {
                    // Dynamic vs Dynamic -- both bounce (invert velocities)
                    velA.dx = -velA.dx / 2.0;
                    velA.dy = -velA.dy / 2.0;
                    velB.dx = -velA.dx / 2.0;
                    velB.dy = -velA.dy / 2.0;
                }
                else if (!colA.isStatic && colB.isStatic) 
                {
                    // A is dynamic, B is static -- only A bounces
                    velA.dx = -velA.dx;
                    velA.dy = -velA.dy;
                }
                else if (colA.isStatic && !colB.isStatic) 
                {
                    // A is static, B is dynamic -- only B bounces
                    velB.dx = -velB.dx;
                    velB.dy = -velB.dy;
                }
            }
        }
    }
}

// Helper Functions

// Axis-Aligned Bounding Box vs AABB
bool CollisionSystem::CheckAABBvsAABB(const Position& aPos, const Collider& a,
    const Position& bPos, const Collider& b) 
{
    // Check overlap along both X and Y axes
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

    // Collision if distance between centers <= sum of radii
    return distSq <= radiusSum * radiusSum;
}

// Circle vs AABB
bool CollisionSystem::CheckCirclevsAABB(const Position& circlePos, const Collider& circle,
    const Position& aabbPos, const Collider& aabb) 
{
    // Half extents of the AABB
    float halfW = aabb.width / 2.0f;
    float halfH = aabb.height / 2.0f;

    // Find closest point on AABB to circle center
    float closestX = Clamp(circlePos.x, aabbPos.x - halfW, aabbPos.x + halfW);
    float closestY = Clamp(circlePos.y, aabbPos.y - halfH, aabbPos.y + halfH);

    // Distance from circle center to closest point
    float dx = circlePos.x - closestX;
    float dy = circlePos.y - closestY;

    // Collision if distance <= circle radius
    return (dx * dx + dy * dy) <= (circle.radius * circle.radius);
}

// Clamp helper (restrict value between min and max)
float CollisionSystem::Clamp(float value, float min, float max) 
{
    return std::max(min, std::min(value, max));
}

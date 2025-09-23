#ifndef COLLISION_HPP
#define COLLISION_HPP
#include "ECS/Entity.h"
#include "ECS/ISystem.h"
#include "ECS/Coordinator.h"
#include "ECS/Component.h"
#include <iostream>

class CollisionSystem : public ISystem {
public:
    CollisionSystem();
    // to change to float after TK push changes for ISystem
    void Update(float dt, Coordinator& coord) override;
    std::string GetName() const { return "CollisionSystem"; }
    Signature GetSignature() const override { return m_systemSignature; }

    // Collision detection helpers
    bool CheckAABBvsAABB(const Position& aPos, const Collider& a,
        const Position& bPos, const Collider& b);
    bool CheckCirclevsCircle(const Position& aPos, const Collider& a,
        const Position& bPos, const Collider& b);
    bool CheckCirclevsAABB(const Position& circlePos, const Collider& circle,
        const Position& aabbPos, const Collider& aabb);

    // Utility helpers
    float Clamp(float value, float min, float max);
    bool IsMovingAway(const Position& pos1, const Position& pos2, const Velocity& vel1);

    // Separation helpers for resolving penetration
    void SeparateObjects(Position& posA, const Collider& colA,
        Position& posB, const Collider& colB,
        const Velocity& velA, const Velocity& velB);
    void SeparateCircles(Position& posA, const Collider& colA,
        Position& posB, const Collider& colB,
        const Velocity& velA, const Velocity& velB);
    void SeparateCircleAABB(Position& circlePos, const Collider& circle,
        Position& aabbPos, const Collider& aabb, bool circleIsA);

    bool ShouldFlipVelocity(float pos1, float pos2, float vel1, float dt);

private:
    Signature m_systemSignature{};
};

#endif // COLLISION_HPP
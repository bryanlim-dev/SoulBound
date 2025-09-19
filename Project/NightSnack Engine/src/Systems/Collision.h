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
    void Update(double dt, Coordinator& coord) override;
    std::string GetName() const { return "CollisionSystem"; }


    Signature GetSignature() const override { return m_systemSignature; }

    // Helpers
    bool CheckAABBvsAABB(const Position& aPos, const Collider& a,
        const Position& bPos, const Collider& b);

    bool CheckCirclevsCircle(const Position& aPos, const Collider& a,
        const Position& bPos, const Collider& b);

    bool CheckCirclevsAABB(const Position& circlePos, const Collider& circle,
        const Position& aabbPos, const Collider& aabb);

    float Clamp(float value, float min, float max);

private:
    Signature m_systemSignature{};

    
};

#endif // COLLISION_HPP

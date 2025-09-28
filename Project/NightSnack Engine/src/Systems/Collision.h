#ifndef COLLISION_HPP
#define COLLISION_HPP
#include "ECS/Entity.h"
#include "ECS/ISystem.h"
#include "ECS/Coordinator.h"
#include "ECS/Component.h"
#include "Systems/IEvent.h"
#include <iostream>

class CollisionSystem : public ISystem {
public:
    CollisionSystem();
    void Init(EventDispatcher& dispatcher);
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

    bool ShouldFlipVelocity(float pos1, float pos2, float vel1, float dt);

    float bounciness = 0.6f;      // bounciness (0 = no bounce, 1 = perfect bounce)
    float minBounceSpeed = 5.0f;  // cutoff to stop bouncing

private:
    Signature m_systemSignature{};

    // System event dispatcher
	EventDispatcher* m_dispatcher = nullptr;
};

#endif // COLLISION_HPP
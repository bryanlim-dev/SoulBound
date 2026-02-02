#include "DistanceJoint2D.h"

namespace NightSnack
{

    void DistanceJoint2D::SetBodies(Entity idA, Entity idB)
    {
        m_bodyA = idA;
        m_bodyB = idB;
    }

    void DistanceJoint2D::LockBodyB(bool isLocked)
    {
        m_lockAnchorB = isLocked;
    }

    void DistanceJoint2D::SetActive(bool active)
    {
        m_active = active;
    }

    void DistanceJoint2D::SetAnchors(const Vec2& a, const Vec2& b)
    {
        m_anchorA = a;
        m_anchorB = b;
    }

    void DistanceJoint2D::SetDistance(float d)
    {
        m_distance = d;
        m_minDistance = d;
        m_maxDistance = d;
        m_autoConfigure = false;
    }

    void DistanceJoint2D::SetLimits(float minD, float maxD)
    {
        m_minDistance = minD;
        m_maxDistance = maxD;
        m_autoConfigure = false;
    }

    void DistanceJoint2D::EnableSpring(bool enable)
    {
        m_enableSpring = enable;
    }

    void DistanceJoint2D::SetSpring(float frequencyHz, float dampingRatio)
    {
        m_frequencyHz = frequencyHz;
        m_dampingRatio = dampingRatio;
    }

    void DistanceJoint2D::SetMaxForce(float maxForce)
    {
        m_maxForce = maxForce;
    }

    void DistanceJoint2D::EnableCollision(bool enable)
    {
        m_enableCollision = enable;
    }

    void DistanceJoint2D::Initialize(
        Coordinator& coord,
        const Vec2& posA,
        const Vec2& posB)
    {
        if (!m_autoConfigure)
            return;

        Vec2 worldA = posA + m_anchorA;
        Vec2 worldB = posB + m_anchorB;

        float d = (worldB - worldA).magnitude();

        m_distance = d;
        m_minDistance = d;
        m_maxDistance = d;
    }

    void DistanceJoint2D::Solve(Coordinator& coord, const Vec2& posA, const Vec2& posB, float /*dt*/)
    {
        if (!m_active)
            return;

        if (!coord.HasComponent<RigidBody>(m_bodyA))
            return;

        RigidBody& bodyA = coord.ModifyComponent<RigidBody>(m_bodyA);

        RigidBody* bodyB = nullptr;
        if (coord.HasComponent<RigidBody>(m_bodyB))
            bodyB = &coord.ModifyComponent<RigidBody>(m_bodyB);

        Vec2 worldA = posA + m_anchorA;
        Vec2 worldB = posB + m_anchorB;

        Vec2 delta = worldB - worldA;
        float dist = delta.magnitude();

        if (dist < 0.00001f)
            return;

        Vec2 n = delta / dist;

        // =========================
        // Distance error
        // =========================
        float targetDist = Clamp(dist, m_minDistance, m_maxDistance);
        float error = dist - targetDist;

        // =========================
        // Relative velocity
        // =========================
        float relVel = -bodyA.vel.dot(n);

        if (bodyB)
            relVel += bodyB->vel.dot(n);

        // =========================
        // Baumgarte stabilization
        // =========================
        constexpr float beta = 0.2f; // positional correction strength
        float bias = beta * error;

        // =========================
        // Effective mass
        // =========================
        float invMassSum = bodyA.invMass + (bodyB ? bodyB->invMass : 0.f);
        if (invMassSum == 0.f)
            return;

        float lambda = -(relVel + bias) / invMassSum;

        lambda = Clamp(lambda, -m_maxForce, m_maxForce);

        Vec2 impulse = n * lambda;

        // =========================
        // Apply impulse
        // =========================
        bodyA.vel -= impulse * bodyA.invMass;

        if (!m_lockAnchorB && bodyB && bodyB->invMass > 0.f)
            bodyB->vel += impulse * bodyB->invMass;
    }

    // --------------------------------------------------

    void DistanceJoint2D::ApplyForce(RigidBody& body, const Vec2& force)
    {
        if (body.invMass == 0.f)
            return;

        float mag = force.magnitude();
        if (mag < 0.00001f)
            return;

        body.force.direction = force * (1.f / mag);
        body.force.magnitude = mag;
        body.force.isActive = true;
    }

    // --------------------------------------------------

    float DistanceJoint2D::Clamp(float v, float min, float max)
    {
        if (v < min) return min;
        if (v > max) return max;
        return v;
    }
}
#include "DistanceJoint2D.h"

namespace NightSnack
{

    void DistanceJoint2D::SetBodies(uint32_t idA, uint32_t idB)
    {
        m_bodyA = idA;
        m_bodyB = idB;
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

    void DistanceJoint2D::Solve(
        Coordinator& coord,
        const Vec2& posA,
        const Vec2& posB,
        float deltaTime)
    {
        if (!m_active)
            return;

        RigidBody& bodyA = coord.ModifyComponent<RigidBody>(m_bodyA);
        RigidBody& bodyB = coord.ModifyComponent<RigidBody>(m_bodyB);

        Vec2 worldA = posA + m_anchorA;
        Vec2 worldB = posB + m_anchorB;

        Vec2 delta = worldB - worldA;
        float currentDistance = delta.magnitude();

        if (currentDistance < 0.00001f)
            return;

        Vec2 normal = delta * (1.f / currentDistance);

        float targetDistance = Clamp(
            currentDistance,
            m_minDistance,
            m_maxDistance
        );

        float error = currentDistance - targetDistance;

        Vec2 relVel = bodyB.vel - bodyA.vel;
        float relVelN = relVel.dot(normal);

        float forceMag;

        if (m_enableSpring)
        {
            float omega = 2.f * 3.14159265359f * m_frequencyHz;

            float spring = -omega * omega * error;
            float damping = -2.f * m_dampingRatio * omega * relVelN;

            forceMag = spring + damping;
        }
        else
        {
            forceMag = -(error / deltaTime);
        }

        forceMag = Clamp(forceMag, -m_maxForce, m_maxForce);

        Vec2 force = normal * forceMag;

        ApplyForce(bodyA, force * -1.f);
        ApplyForce(bodyB, force);
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
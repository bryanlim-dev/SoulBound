/******************************************************************************
 * File:        DistanceJoint2D.cpp
 * Project:     NightSnack
 * Author(s):	Clement Ang Yang Ying (Primary, 100%)
 *
 * Description:
 * This file defines the the class DistanceJoint2D and its functions, it
 * connector to entity and rigidbodies together and allows a swing force to be
 * added to body A to swing about body B
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
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

    float DistanceJoint2D::GetDistance()
    {
        return m_distance;
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

    void DistanceJoint2D::SetAutoConfigure(bool autoConfig)
    {
        m_autoConfigure = autoConfig;
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
        (void)coord;

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
        float dt)
    {
        if (!m_active || dt <= 0.f)
            return;

        if (!coord.HasComponent<RigidBody>(m_bodyA))
            return;

        RigidBody& bodyA = coord.ModifyComponent<RigidBody>(m_bodyA);

        RigidBody* bodyB = nullptr;
        if (coord.HasComponent<RigidBody>(m_bodyB))
            bodyB = &coord.ModifyComponent<RigidBody>(m_bodyB);

        //World anchors
        Vec2 worldA = posA + m_anchorA;
        Vec2 worldB = posB + m_anchorB;

        Vec2 delta = worldB - worldA;
        float dist = delta.magnitude();

        if (dist < 0.00001f)
            return;

        Vec2 n = delta / dist;

        //Target distance
        const float targetDist = m_minDistance;
        float error = dist - targetDist;

        //Relative velocity along constraint axis
        float relVel = -bodyA.vel.dot(n);

        if (bodyB && !m_lockAnchorB)
            relVel += bodyB->vel.dot(n);

        //Baumgarte stabilization (some formula to help stablize the dist )
        constexpr float beta = 0.8f;
        float bias = (beta / dt) * error;

        //Effective mass
        float invMassSum = bodyA.invMass;

        if (bodyB && !m_lockAnchorB)
            invMassSum += bodyB->invMass;

        if (invMassSum <= 0.f)
            return;

        //Constraint impulse
        float lambda = -(relVel + bias) / invMassSum;

        lambda = Clamp(lambda, -m_maxForce, m_maxForce);

        Vec2 impulse = n * lambda;

        //Apply impulse
        bodyA.vel -= impulse * bodyA.invMass;

        if (bodyB && !m_lockAnchorB && bodyB->invMass > 0.f)
            bodyB->vel += impulse * bodyB->invMass;
    }

    // --------------------------------------------------

    void DistanceJoint2D::ApplySwingForce(
        Coordinator& coord,
        float forceMagnitude
    )
    {
        if (!m_active || forceMagnitude == 0)
            return;

        if (!coord.HasComponent<RigidBody>(m_bodyA))
            return;

        RigidBody& bodyA = coord.ModifyComponent<RigidBody>(m_bodyA);
        Vec2 posA = coord.ModifyComponent<Transform>(m_bodyA).pos;
        Vec2 posB = coord.ModifyComponent<Transform>(m_bodyB).pos;

        //World anchor positions
        Vec2 worldA = posA + m_anchorA;
        Vec2 worldB = posB + m_anchorB;

        Vec2 delta = worldA - worldB;
        float dist = delta.magnitude();

        if (dist < 0.00001f)
            return;

        //Rope direction (normalized)
        Vec2 n = delta / dist;

        //Tangent direction (perpendicular)
        Vec2 tangent(-n.y, n.x);

        // How vertical the rope is (1 = vertical, 0 = horizontal)
        float gravityFactor = fabs(n.y);

        forceMagnitude *= (0.3f + 0.7f * fabs(n.y));

        //Convert force to impulse

        //Current tangential velocity
        float tangentialVel = bodyA.vel.dot(tangent);

        // If barely moving, do not inject energy
        if (fabs(tangentialVel) < 0.01f)
            return;

        //Progressive boost

        const float refSpeed = 8; 
        float normalizedSpeed = (fabs(tangentialVel) / (refSpeed));

        // inear growth instead of exponential
        float speedFactor = 1.0f + (normalizedSpeed * 0.75f);

        //Only boost strongly near bottom of arc
        float bottomBoost = fabs(n.y); // 1 at bottom, 0 at sides

        //Sharp falloff away from bottom (prevents pivot explosion)
        bottomBoost = bottomBoost * bottomBoost;

        speedFactor *= bottomBoost;

        //Clamp to avoid instability
        speedFactor = Clamp(speedFactor, 0.0f, 2.0f);

        //Build impulse
        Vec2 impulse = tangent * forceMagnitude * speedFactor * coord.GetSystem<ApplicationSystem>()->GetDT();

        //Apply impulse
        bodyA.vel += impulse * bodyA.invMass;
    }

    // --------------------------------------------------

    float DistanceJoint2D::Clamp(float v, float min, float max)
    {
        if (v < min) return min;
        if (v > max) return max;
        return v;
    }
}
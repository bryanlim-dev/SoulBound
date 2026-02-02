/******************************************************************************
 * File:        Vector.h
 * Project:     NightSnack
 * Author(s):	Clement Ang Yang Ying (Primary, 100%)
 *
 * Description:
 * This file declares the the class DistanceJoint and its functions
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "../Core/ECS/Component.h"
#include <Core/ECS/Entity.h>
#include <Core/ECS/Coordinator.h>

namespace NightSnack
{
    class DistanceJoint2D
    {
    public:
        DistanceJoint2D() :
            m_bodyA(INVALID_ENTITY),
            m_bodyB(INVALID_ENTITY),
            m_active(false),
            m_anchorA(0.f, 0.f),
            m_anchorB(0.f, 0.f),
            m_lockAnchorB(false),
            m_enableCollision(false),
            m_autoConfigure(true),
            m_distance(0.f),
            m_minDistance(0.f),
            m_maxDistance(0.f),
            m_enableSpring(false),
            m_frequencyHz(5.f),
            m_dampingRatio(0.7f),
            m_maxForce(INFINITY) {

        }

        //Entity assignment (set externally)
        void SetBodies(Entity idA, Entity idB);
        void LockBodyB(bool isLocked);
        void SetActive(bool active);

        //Configuration
        void SetAnchors(const Vec2& a, const Vec2& b);
        void SetDistance(float d);
        void SetLimits(float minD, float maxD);

        void EnableSpring(bool enable);
        void SetSpring(float frequencyHz, float dampingRatio);

        void SetMaxForce(float maxForce);
        void EnableCollision(bool enable);

        //Must be called once after setup
        void Initialize(
            Coordinator& coord,
            const Vec2& posA,
            const Vec2& posB
        );

        //Called every update if active
        void Solve(Coordinator& coord, const Vec2& posA, const Vec2& posB, float deltaTime);

        void ApplyForce(RigidBody& body, const Vec2& force);
    
    private:
        static float Clamp(float v, float min, float max);

    private:
        //Entity
        Entity m_bodyA;
        Entity m_bodyB;

        bool m_active;

        //Anchors
        Vec2 m_anchorA;
        Vec2 m_anchorB;
        bool m_lockAnchorB;

        //Collision flag
        bool m_enableCollision;

        //Distance
        bool  m_autoConfigure;
        float m_distance;
        float m_minDistance;
        float m_maxDistance;

        //Spring
        bool  m_enableSpring;
        float m_frequencyHz;
        float m_dampingRatio;

        //Force clamp
        float m_maxForce;


    };
}
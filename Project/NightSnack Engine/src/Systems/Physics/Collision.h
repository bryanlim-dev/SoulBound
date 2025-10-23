/******************************************************************************
 * File:        Collision.h
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 100%) - Collision system with respective entity response
 *
 * Description:
 * This file declares the collision system. It handles collision detection
 * and response with entities in the game. It supports dynamic and static
 * collision checks such as circle and circle, AABB and AABB, circle and AABB etc
 * Handles collision responses such as velocity flips and dampening.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#ifndef COLLISION_HPP
#define COLLISION_HPP
#include "Core/ECS/Entity.h"
#include "Core/ECS/ISystem.h"
#include "Core/ECS/Coordinator.h"
#include "Core/ECS/Component.h"
#include "Systems/Application/IEvent.h"
#include <iostream>

namespace NightSnack {

    /******************************************************************************/
    /*!
    \class CollisionSystem
    \brief
    Handles collision detection and response between entities. The system checks
    for collisions between dynamic and static objects using AABB and circle shapes,
    updates collision states, applies bounce physics with energy loss, and dispatches
    collision events through the ECS event system.
    */
    /******************************************************************************/
    class CollisionSystem : public ISystem {
    public:
        /************************************************************************/
        /*!
        \brief
        Constructor. Registers the Position, Velocity, and Collider components as
        system requirements, and initializes the system with an event dispatcher

        \param disp
        Reference to the EventDispatcher used
        */
        /************************************************************************/
        CollisionSystem(EventDispatcher& disp)
        {
            // Register the components this system needs:
            // Position, Velocity, and Collider
            m_systemSignature.set(GetComponentTypeID<Transform>());
            m_systemSignature.set(GetComponentTypeID<RigidBody>());
            m_systemSignature.set(GetComponentTypeID<Collider>());

            Init(disp);
        }
        /************************************************************************/
        /*!
        \brief
        Init the collision system and subscribe to collision events

        \param dispatcher
        Reference to the EventDispatcher used
        */
        /************************************************************************/
        void Init(EventDispatcher& dispatcher);

        /************************************************************************/
        /*!
        \brief
        Update collision system each frame. reset collision flags and
        checks for entity collisions and apply appropriate responses.

        \param dt
        delta time

        \param coord
        Reference to ECS coordinator
        */
        /************************************************************************/
        void Update(float dt, Coordinator& coord) override;

        /************************************************************************/
        /*!
        \brief
        Returns name of the system

        \return
        String "CollisionSystem"
        */
        /************************************************************************/
        std::string GetName() const { return "CollisionSystem"; }

        /************************************************************************/
        /*!
        \brief
        Returns the system component signature

        \return
        Required component types
        */
        /************************************************************************/
        Signature GetSignature() const override { return m_systemSignature; }

        /************************************************************************/
        /*!
        \brief
        Checks for collision between two AABBs (Axis-Aligned Bounding Boxes).

        \param aPos
        Position of first AABB.

        \param a
        Collider of the first AABB.

        \param bPos
        Position of second AABB.

        \param b
        Collider of the second AABB.

        \return
        True if the AABBs overlap, else false
        */
        /************************************************************************/
        bool CheckAABBvsAABB(const Vec3& aPos, const Collider& a,
            const Vec3& bPos, const Collider& b);

        /************************************************************************/
        /*!
        \brief
        Checks for collision between two circles.

        \param aPos
        Position of the first circle.

        \param a
        Collider of the first circle.

        \param bPos
        Position of the second circle.

        \param b
        Collider of the second circle.

        \return
        True if the circles overlap, else false.
        */
        /************************************************************************/
        bool CheckCirclevsCircle(const Vec3& aPos, const Collider& a,
            const Vec3& bPos, const Collider& b);

        /************************************************************************/
        /*!
        \brief
        Checks for collision between a circle and an AABB.

        \param circlePos
        Position of the circle.

        \param circle
        Collider of the circle.

        \param aabbPos
        Position of the AABB.

        \param aabb
        Collider of the AABB.

        \return
        True if the circle intersects with the AABB, else false.
        */
        /************************************************************************/
        bool CheckCirclevsAABB(const Vec3& circlePos, const Collider& circle,
            const Vec3& aabbPos, const Collider& aabb);

        /************************************************************************/
        /*!
        \brief
        Clamps a value between a minimum and maximum

        \param value
        Value to clamp

        \param min
        Minimum value

        \param max
        Maximum value

        \return
        Clamped value
        */
        /************************************************************************/
        float Clamp(float value, float min, float max);

        /************************************************************************/
        /*!
        \brief
        Determines whether a velocity should be flipped based on positions and
        current movement direction.

        \param pos1
        Position of the first object

        \param pos2
        Position of the second object

        \param vel1
        Velocity of the first object

        \param dt
        Delta time

        \return
        True if the velocity should flip, else false
        */
        /************************************************************************/
        bool ShouldFlipVelocity(float pos1, float pos2, float vel1, float dt);

        float bounciness = 0.6f;      // bounciness (0 = no bounce, 1 = perfect bounce)
        float minBounceSpeed = 5.0f;  // cutoff to stop bouncing

    private:
        Signature m_systemSignature{};

        // System event dispatcher
        EventDispatcher* m_dispatcher = nullptr;
    };

}   // NightSnack

#endif // COLLISION_HPP
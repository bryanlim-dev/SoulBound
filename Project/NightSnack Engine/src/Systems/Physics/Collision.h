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

#include "Core/ECS/Coordinator.h"

namespace NightSnack {

    void CheckCollisions(std::array<Entity, MAX_ENTITIES> const&);

    bool CheckCollision(Entity e1, Entity e2);

    void ResolveCollisions(Entity e1, Entity e2);

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

}   // NightSnack

#endif // COLLISION_HPP
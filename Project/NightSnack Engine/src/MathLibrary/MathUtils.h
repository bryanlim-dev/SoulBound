/******************************************************************************
 * File:        MathUtils.h
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 100%) - Helper Functions
 *
 * Description:
 * Contains helper functions for angle calculation as well as
 * PI and Z_EPSILON values.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
#pragma once
#include <cmath>
#include <stdexcept>

namespace NightSnack {

    // PI value
    constexpr double PI = 3.14159265358979323846;
    // Z coordinate position offset value 
    constexpr float Z_EPSILON = 0.0001f;

    /************************************************************************/
    /*!
    \brief
    Convert degrees to radians.

    \param deg
    Angle in degrees.

    \return
    Angle in radians.
    */
    /************************************************************************/
    inline float deg2rad(float deg)
    {
        return static_cast<float>(deg * PI / 180.0);
    }

    /************************************************************************/
    /*!
    \brief
    Convert radians to degrees.

    \param rad
    Angle in radians.

    \return
    Angle in degrees.
    */
    /************************************************************************/
    inline float rad2deg(float rad)
    {
        return static_cast<float>(rad * 180.0 / PI);
    }

}   // NightSnack

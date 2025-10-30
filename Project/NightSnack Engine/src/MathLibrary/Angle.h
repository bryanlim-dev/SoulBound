/******************************************************************************
 * File:        Angle.h
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 100%) - Created Angle functions (sin,cos,tan).
 *
 * Description:
 * This file declares Angle functions such as conversion from angle to radians and back,
 * as well as trigonometric functions like sine, cosine and tangent.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
#pragma once
#include <cmath>
#include "MathUtils.h"

namespace NightSnack {

    struct Angle {
        float radians;  // angle value in radians

        /************************************************************************/
        /*!
        \brief
        Creates a new Angle from radians.

        \param rad
        Angle value in radians.
        */
        /************************************************************************/
        Angle(float rad = 0.0);

        /************************************************************************/
        /*!
        \brief
        Create an Angle using degree value.

        \param rad
        Angle in degrees.

        \return
        A new Angle object stored as radians.
        */
        /************************************************************************/
        static Angle fromDegrees(float deg);

        /************************************************************************/
        /*!
        \brief
        Converts Angle back to degrees.

        \return
        The value of the angle in degrees.
        */
        /************************************************************************/
        float toDegrees() const;

        /************************************************************************/
        /*!
        \brief
        Gets the sine of Angle.

        \return
        The sine value of the radians.
        */
        /************************************************************************/
        float sin() const;

        /************************************************************************/
        /*!
        \brief
        Gets the cosine of Angle.

        \return
        The cosine value of the radians.
        */
        /************************************************************************/
        float cos() const;

        /************************************************************************/
        /*!
        \brief
        Gets the tangent of Angle.

        \return
        The tangent value of the radians.
        */
        /************************************************************************/
        float tan() const;
    };

}   // NightSnack

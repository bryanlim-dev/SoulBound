/******************************************************************************
 * File:        Vector.h
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 100%) - Vec2 and Vec3 functions
 *
 * Description:
 * This file declares the functionality for vec2 and vec3 classes.
 * Handles arithmetic operators, dot and cross product, magnitude, normalization and
 * formmatted output for debug.
 * 
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
#pragma once
#include <iostream>
#include <cmath>
#include <stdexcept>

namespace NightSnack {
    
    struct Vec2;
    struct Vec3;

    // Vec2
    struct Vec2 {
        float x, y;

        /************************************************************************/
        /*!
        \brief
        2D vector with given values, defaulting to (0,0).

        \param x
        x of the vector.

        \param y
        y of the vector.
        */
        /************************************************************************/
        Vec2(float x = 0.0, float y = 0.0);

        Vec2(Vec3 const&);
        
        Vec2& operator+=(Vec2 const& rhs);

        Vec2& operator-=(Vec2 const& rhs);

        Vec2& operator=(Vec3 const& rhs);

        Vec2 operator-() {
            return Vec2{ -x, -y };
        }

        /************************************************************************/
        /*!
        \brief
        Compute dot product between vector and other.

        \param other
        Other vector.

        \return
        Scalar of the dot product.
        */
        /************************************************************************/
        float dot(const Vec2& other) const;

        /************************************************************************/
        /*!
        \brief
        Compute 2d cross product.

        \param other
        Other vector.

        \return
        Scalar of the cross product.
        */
        /************************************************************************/
        float cross(const Vec2& other) const;

        /************************************************************************/
        /*!
        \brief
        Addition of vector

        \param other
        Other vector to add.

        \return
        Sum of the new vector.
        */
        /************************************************************************/
        Vec2 operator+(const Vec2& other) const;

        /************************************************************************/
        /*!
        \brief
        Subtraction of vector.

        \param other
        Other vector to subtract.

        \return
        New vector after subtracting.
        */
        /************************************************************************/
        Vec2 operator-(const Vec2& other) const;

        /************************************************************************/
        /*!
        \brief
        Multiplication of scalar.

        \param scalar
        Scalar to multiply.

        \return
        New scalar after multiplying.
        */
        /************************************************************************/
        Vec2 operator*(float scalar) const;

        /************************************************************************/
        /*!
        \brief
        Division of scalar.

        \param scalar
        Scalar to divide.

        \return
        New scalar after dividing.
        */
        /************************************************************************/
        Vec2 operator/(float scalar) const;

        /************************************************************************/
        /*!
        \brief
        Overloaded operator for dot product.

        \param other
        Other vector.

        \return
        New scalar after multiplying.
        */
        /************************************************************************/
        float operator*(const Vec2& other) const;

        /************************************************************************/
        /*!
        \brief
        Overloaded operator for cross product.

        \param other
        Other vector.

        \return
        New scalar after cross product.
        */
        /************************************************************************/
        float operator^(const Vec2& other) const;

        /************************************************************************/
        /*!
        \brief
        Compute length of the vector.

        \return
        Magnitude.
        */
        /************************************************************************/
        float magnitude() const;

        /************************************************************************/
        /*!
        \brief
        Return normalized copy of this vector.

        \return
        Unit vector pointing in same direction.
        */
        /************************************************************************/
        Vec2 normalized() const;  // returns a unit vector

        /************************************************************************/
        /*!
        \brief
        Normalize this vector in place.
        */
        /************************************************************************/
        void normalize();         // modifies this vector in-place
    };

    /************************************************************************/
    /*!
    \brief
    Overloaded output stream operator for printing Vec2.

    \param os
    Output stream.

    \param v
    Vector to print.

    \return
    Modified output stream with vector formatted as x,y
    */
    /************************************************************************/
    std::ostream& operator<<(std::ostream& os, const Vec2& v);


    // Vec3
    struct Vec3 {
        float x, y, z;

        /************************************************************************/
        /*!
        \brief
        Construct a 3D vector with default to (0,0,0).

        \param x
        x of the vector.

        \param y
        y of the vector.

        \param z
        Z of the vector.
        */
        /************************************************************************/
        Vec3(float x = 0.0, float y = 0.0, float z = 0.0);

        Vec3& operator+=(Vec3 const& rhs);

        Vec3& operator-=(Vec3 const& rhs);

        Vec3 operator-() {
            return Vec3{ -x, -y };
        }


        /************************************************************************/
        /*!
        \brief
        Compute dot product between vector and other.

        \param other
        Other vector.

        \return
        Scalar of the dot product.
        */
        /************************************************************************/
        float dot(const Vec3& other) const;

        /************************************************************************/
        /*!
        \brief
        Compute 3d cross product.

        \param other
        Other vector.

        \return
        New vec3 cross product result.
        */
        /************************************************************************/
        Vec3 cross(const Vec3& other) const;

        /************************************************************************/
        /*!
        \brief
        Addition of vectors.

        \param other
        Vector to add.

        \return
        Sum of the two vectors.
        */
        /************************************************************************/
        Vec3 operator+(const Vec3& other) const;

        /************************************************************************/
        /*!
        \brief
        Subtraction of vectors.

        \param other
        Vector to subtract.

        \return
        Result of subtracting the two vectors.
        */
        /************************************************************************/
        Vec3 operator-(const Vec3& other) const;

        /************************************************************************/
        /*!
        \brief
        Multiplication of vectors.

        \param scalar
        Scalar to multiply.

        \return
        Scaled vector.
        */
        /************************************************************************/
        Vec3 operator*(float scalar) const;

        /************************************************************************/
        /*!
        \brief
        Division of vectors.

        \param scalar
        Scalar to divide.

        \return
        Scaled vector.
        */
        /************************************************************************/
        Vec3 operator/(float scalar) const;

        /************************************************************************/
        /*!
        \brief
        Overloaded dot product.

        \param other
        Other vector.

        \return
        Scalar dot product.
        */
        /************************************************************************/
        float operator*(const Vec3& other) const;

        /************************************************************************/
        /*!
        \brief
        Overloaded cross product.

        \param other
        Other vector.

        \return
        Scalar cross product.
        */
        /************************************************************************/
        Vec3 operator^(const Vec3& other) const;   // cross

        /************************************************************************/
        /*!
        \brief
        Compute length of the vector.

        \return
        Magnitude.
        */
        /************************************************************************/
        float magnitude() const;

        /************************************************************************/
        /*!
        \brief
        Return normalized copy of this vector.

        \return
        Unit vector pointing in same direction.
        */
        /************************************************************************/
        Vec3 normalized() const;  // returns a unit vector

        /************************************************************************/
        /*!
        \brief
        Normalize this vector in place.
        */
        /************************************************************************/
        void normalize();         // modifies this vector in-place
    };

    /************************************************************************/
    /*!
    \brief
    Overloaded output stream operator for printing Vec3.

    \param os
    Output stream.

    \param v
    Vector to print.

    \return
    Modified output stream with vector formatted as x,y,z
    */
    /************************************************************************/
    std::ostream& operator<<(std::ostream& os, const Vec3& v);

}   // NightSnack

/******************************************************************************
 * File:        Vector.cpp
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 100%) - Vec2 and Vec3 functions
 *
 * Description:
 * This file defines the functionality for vec2 and vec3 classes.
 * Handles arithmetic operators, dot and cross product, magnitude, normalization and
 * formmatted output for debug.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "Vector.h"

namespace NightSnack {

    // Vec2
    Vec2::Vec2(float x, float y) : x(x), y(y) {}

    // Dot product
    float Vec2::dot(const Vec2& other) const
    {
        return x * other.x + y * other.y;
    }

    // 2D Cross product (returns scalar)
    float Vec2::cross(const Vec2& other) const
    {
        return x * other.y - y * other.x;
    }

    // Arithmetic operators
    Vec2 Vec2::operator+(const Vec2& other) const
    {
        return Vec2(x + other.x, y + other.y);
    }

    Vec2 Vec2::operator-(const Vec2& other) const
    {
        return Vec2(x - other.x, y - other.y);
    }

    Vec2 Vec2::operator*(float scalar) const
    {
        return Vec2(x * scalar, y * scalar);
    }

    Vec2 Vec2::operator/(float scalar) const
    {
        if (scalar == 0.0) throw std::runtime_error("Divide by zero");
        return Vec2(x / scalar, y / scalar);
    }

    // Overload dot
    float Vec2::operator*(const Vec2& other) const
    {
        return dot(other);
    }

    // Overload cross
    float Vec2::operator^(const Vec2& other) const
    {
        return cross(other);
    }

    // Magnitude (length)
    float Vec2::magnitude() const
    {
        return std::sqrt(x * x + y * y);
    }

    // Normalized copy
    Vec2 Vec2::normalized() const
    {
        float mag = magnitude();
        if (mag == 0.0) throw std::runtime_error("Cannot normalize zero-length vector");
        return Vec2(x / mag, y / mag);
    }

    // Normalize in-place
    void Vec2::normalize()
    {
        float mag = magnitude();
        if (mag == 0.0) throw std::runtime_error("Cannot normalize zero-length vector");
        x /= mag;
        y /= mag;
    }

    // Stream output for Vec2
    std::ostream& operator<<(std::ostream& os, const Vec2& v)
    {
        return os << "(" << v.x << ", " << v.y << ")";
    }



    // Vec3
    Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    // Dot product
    float Vec3::dot(const Vec3& other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }

    // Cross product (returns vector)
    Vec3 Vec3::cross(const Vec3& other) const
    {
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    // Arithmetic operators
    Vec3 Vec3::operator+(const Vec3& other) const
    {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    Vec3 Vec3::operator-(const Vec3& other) const
    {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    Vec3 Vec3::operator*(float scalar) const
    {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }

    Vec3 Vec3::operator/(float scalar) const
    {
        if (scalar == 0.0) throw std::runtime_error("Divide by zero");
        return Vec3(x / scalar, y / scalar, z / scalar);
    }

    // Overload dot
    float Vec3::operator*(const Vec3& other) const
    {
        return dot(other);
    }

    // Overload cross
    Vec3 Vec3::operator^(const Vec3& other) const
    {
        return cross(other);
    }

    // Magnitude (length)
    float Vec3::magnitude() const
    {
        return std::sqrt(x * x + y * y + z * z);
    }

    // Normalized copy
    Vec3 Vec3::normalized() const
    {
        float mag = magnitude();
        if (mag == 0.0) throw std::runtime_error("Cannot normalize zero-length vector");
        return Vec3(x / mag, y / mag, z / mag);
    }

    // Normalize in-place
    void Vec3::normalize()
    {
        float mag = magnitude();
        if (mag == 0.0) throw std::runtime_error("Cannot normalize zero-length vector");
        x /= mag;
        y /= mag;
        z /= mag;
    }

    // Stream output for Vec3
    std::ostream& operator<<(std::ostream& os, const Vec3& v)
    {
        return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    }

}   // NightSnack

#pragma once
#include <iostream>
#include <cmath>
#include <stdexcept>

// ============================
// Vec2
// ============================
struct Vec2 {
    float x, y;

    // Constructor (defaults to 0,0)
    Vec2(float x = 0.0, float y = 0.0);

    // Dot product
    float dot(const Vec2& other) const;

    // 2D Cross product (returns scalar)
    float cross(const Vec2& other) const;

    // Arithmetic operators
    Vec2 operator+(const Vec2& other) const;
    Vec2 operator-(const Vec2& other) const;
    Vec2 operator*(float scalar) const;
    Vec2 operator/(float scalar) const;

    // Overloads
    float operator*(const Vec2& other) const; // dot
    float operator^(const Vec2& other) const; // cross

    // Magnitude (length)
    float magnitude() const;

    //  Normalization
    Vec2 normalized() const;  // returns a unit vector
    void normalize();         // modifies this vector in-place
};

// Output stream operator for Vec2
std::ostream& operator<<(std::ostream& os, const Vec2& v);


// ============================
// Vec3
// ============================
struct Vec3 {
    float x, y, z;

    // Constructor (defaults to 0,0,0)
    Vec3(float x = 0.0, float y = 0.0, float z = 0.0);

    // Dot product
    float dot(const Vec3& other) const;

    // Cross product (returns vector)
    Vec3 cross(const Vec3& other) const;

    // Arithmetic operators
    Vec3 operator+(const Vec3& other) const;
    Vec3 operator-(const Vec3& other) const;
    Vec3 operator*(float scalar) const;
    Vec3 operator/(float scalar) const;

    // Overloads
    float operator*(const Vec3& other) const; // dot
    Vec3 operator^(const Vec3& other) const;   // cross

    // Magnitude (length)
    float magnitude() const;

    // Normalization
    Vec3 normalized() const;  // returns a unit vector
    void normalize();         // modifies this vector in-place
};

// Output stream operator for Vec3
std::ostream& operator<<(std::ostream& os, const Vec3& v);

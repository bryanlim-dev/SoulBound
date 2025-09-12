#pragma once
#include <iostream>
#include <cmath>
#include <stdexcept>

// Vec2
struct Vec2 {
    double x, y;

    // Constructor (defaults to 0,0)
    Vec2(double x = 0.0, double y = 0.0);

    // Dot product
    double dot(const Vec2& other) const;

    // 2D Cross product (returns scalar)
    double cross(const Vec2& other) const;

    // Arithmetic operators
    Vec2 operator+(const Vec2& other) const;
    Vec2 operator-(const Vec2& other) const;
    Vec2 operator*(double scalar) const;
    Vec2 operator/(double scalar) const;

    // Overloads
    double operator*(const Vec2& other) const; // dot
    double operator^(const Vec2& other) const; // cross

    // Magnitude (length)
    double magnitude() const;
};

// Output stream operator for Vec2
std::ostream& operator<<(std::ostream& os, const Vec2& v);


// Vec3
struct Vec3 {
    double x, y, z;

    // Constructor (defaults to 0,0,0)
    Vec3(double x = 0.0, double y = 0.0, double z = 0.0);

    // Dot product
    double dot(const Vec3& other) const;

    // Cross product (returns vector)
    Vec3 cross(const Vec3& other) const;

    // Arithmetic operators
    Vec3 operator+(const Vec3& other) const;
    Vec3 operator-(const Vec3& other) const;
    Vec3 operator*(double scalar) const;
    Vec3 operator/(double scalar) const;

    // Overloads
    double operator*(const Vec3& other) const; // dot
    Vec3 operator^(const Vec3& other) const;   // cross

    // Magnitude (length)
    double magnitude() const;

};

// Output stream operator for Vec3
std::ostream& operator<<(std::ostream& os, const Vec3& v);


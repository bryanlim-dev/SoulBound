#pragma once
#include <iostream>
#include <stdexcept>

// ========================= Scalar Math =========================
double add(double a, double b);
double subtract(double a, double b);
double multiply(double a, double b);
double divide(double a, double b);

// ========================= Vec2 =========================
struct Vec2 {
    double x, y;

    Vec2(double x = 0, double y = 0);

    double dot(const Vec2& other) const;
    double cross(const Vec2& other) const;

    Vec2 operator+(const Vec2& other) const;
    Vec2 operator-(const Vec2& other) const;
    Vec2 operator*(double scalar) const;
    Vec2 operator/(double scalar) const;

    double operator*(const Vec2& other) const; // overload dot product
    double operator^(const Vec2& other) const; // overload cross product
};

std::ostream& operator<<(std::ostream& os, const Vec2& v); // overload << to print to cout

// ========================= Vec3 =========================
struct Vec3 {
    double x, y, z;

    Vec3(double x = 0, double y = 0, double z = 0);

    double dot(const Vec3& other) const;
    Vec3 cross(const Vec3& other) const;

    Vec3 operator+(const Vec3& other) const;
    Vec3 operator-(const Vec3& other) const;
    Vec3 operator*(double scalar) const;
    Vec3 operator/(double scalar) const;

    double operator*(const Vec3& other) const; // overload dot product
    Vec3 operator^(const Vec3& other) const;   // overload cross product
};

std::ostream& operator<<(std::ostream& os, const Vec3& v); // overload << to print to cout

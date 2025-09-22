#pragma once
#include <cmath>
#include <stdexcept>

constexpr double PI = 3.14159265358979323846;

// Degrees to Radians
inline float deg2rad(float deg)
{
    return static_cast<float>(deg * PI / 180.0);
}

// Radians to Degrees
inline float rad2deg(float rad)
{
    return static_cast<float>(rad * 180.0 / PI);
}

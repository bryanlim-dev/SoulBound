#pragma once
#include <cmath>
#include <stdexcept>

constexpr double PI = 3.14159265358979323846;

// use inline to avoid multiple definition error

// Add two scalars
inline float add(float a, float b) { return a + b; }

// Subtract two scalars
inline float subtract(float a, float b) { return a - b; }

// Multiply two scalars
inline float multiply(float a, float b) { return a * b; }

// Divide two scalars
inline float divide(float a, float b) {
    if (b == 0) throw std::runtime_error("Divide by zero");
    return a / b;
}
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

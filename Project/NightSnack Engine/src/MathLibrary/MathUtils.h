#pragma once
#include <cmath>
#include <stdexcept>

constexpr double PI = 3.14159265358979323846;

// use inline to avoid multiple definition error

// Add two scalars
inline double add(double a, double b) { return a + b; }

// Subtract two scalars
inline double subtract(double a, double b) { return a - b; }

// Multiply two scalars
inline double multiply(double a, double b) { return a * b; }

// Divide two scalars
inline double divide(double a, double b) {
    if (b == 0) throw std::runtime_error("Divide by zero");
    return a / b;
}
// Degrees to Radians
inline double deg2rad(double deg) 
{
    return deg * PI / 180.0;
}

// Radians to Degrees
inline double rad2deg(double rad) 
{
    return rad * 180.0 / PI;
}

#pragma once
#include <cmath>
#include "MathUtils.h"

struct Angle {
    float radians;  // angle value in radians

    // Constructor: takes radians (default = 0.0)
    Angle(float rad = 0.0);

    // Create an Angle from degrees
    static Angle fromDegrees(float deg);

    // Convert radians to degrees
    float toDegrees() const;

    // Trigonometric functions using radians
    float sin() const;
    float cos() const;
    float tan() const;
};

#pragma once
#include <cmath>
#include "MathUtils.h"

struct Angle {
    double radians;  // angle value in radians

    // Constructor: takes radians (default = 0.0)
    Angle(double rad = 0.0);

    // Create an Angle from degrees
    static Angle fromDegrees(double deg);

    // Convert radians to degrees
    double toDegrees() const;

    // Trigonometric functions using radians
    double sin() const;
    double cos() const;
    double tan() const;
};

#include "Angle.h"

// Constructor
Angle::Angle(float rad) : radians(rad) {}

// Create an Angle from degrees
Angle Angle::fromDegrees(float deg)
{
    return Angle(deg2rad(deg));
}

// Convert radians back to degrees
float Angle::toDegrees() const
{
    return rad2deg(radians);
}

// Sine
float Angle::sin() const
{ 
    return std::sin(radians); 
}

// Cosine
float Angle::cos() const
{ 
    return std::cos(radians); 
}

// Tangent
float Angle::tan() const
{ 
    return std::tan(radians); 
}

#include "Angle.h"

// Constructor
Angle::Angle(double rad) : radians(rad) {}

// Create an Angle from degrees
Angle Angle::fromDegrees(double deg)
{
    return Angle(deg2rad(deg));
}

// Convert radians back to degrees
double Angle::toDegrees() const
{
    return rad2deg(radians);
}

// Sine
double Angle::sin() const 
{ 
    return std::sin(radians); 
}

// Cosine
double Angle::cos() const 
{ 
    return std::cos(radians); 
}

// Tangent
double Angle::tan() const 
{ 
    return std::tan(radians); 
}

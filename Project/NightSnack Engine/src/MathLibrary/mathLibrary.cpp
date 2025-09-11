#include "mathLibrary.h"

// ========================= Scalar Math =========================

// Add two scalars (a + b)
double add(double a, double b) { return a + b; }

// Subtract two scalars (a - b)
double subtract(double a, double b) { return a - b; }

// Multiply two scalars (a * b)
double multiply(double a, double b) { return a * b; }

// Divide two scalars (a / b)
// Throws runtime_error if dividing by zero
double divide(double a, double b) {
    if (b == 0.0)
    {
        throw std::runtime_error("Division by zero!");
    }
    return a / b;
}

// ========================= Vec2 =========================

// Constructor for Vec2, default values 0.0 if not provided
Vec2::Vec2(double x, double y) : x(x), y(y) {}

// Dot product (x1*x2 + y1*y2)
double Vec2::dot(const Vec2& other) const 
{ 
    return x * other.x + y * other.y; 
}

// Cross product (x1*y2 - y1*x2)
double Vec2::cross(const Vec2& other) const 
{ 
    return x * other.y - y * other.x; 
}

// Addition of two Vec2 vectors
Vec2 Vec2::operator+(const Vec2& other) const 
{ 
    return Vec2(x + other.x, y + other.y); 
}

// Subtraction of two Vec2 vectors
Vec2 Vec2::operator-(const Vec2& other) const 
{ 
    return Vec2(x - other.x, y - other.y); 
}

// Multiply each component by a scalar value
Vec2 Vec2::operator*(double scalar) const 
{ 
    return Vec2(x * scalar, y * scalar); 
}

// Divide each component by a scalar value
// Throws runtime_error if scalar is zero
Vec2 Vec2::operator/(double scalar) const 
{
    if (scalar == 0.0)
    {
        throw std::runtime_error("Division by zero!");
    }
    return Vec2(x / scalar, y / scalar);
}

// Overload * operator for dot product between two Vec2
double Vec2::operator*(const Vec2& other) const { return this->dot(other); }

// Overload ^ operator for cross product between two Vec2 (returns scalar in 2D)
double Vec2::operator^(const Vec2& other) const { return this->cross(other); }

// Overload << operator for easy printing of Vec2
std::ostream& operator<<(std::ostream& os, const Vec2& v) {
    os << "(" << v.x << ", " << v.y << ")";
    return os;
}

// ========================= Vec3 =========================

// Constructor for Vec3, default values 0.0 if not provided
Vec3::Vec3(double x, double y, double z) : x(x), y(y), z(z) {}

// Dot product: returns scalar (x1*x2 + y1*y2 + z1*z2)
double Vec3::dot(const Vec3& other) const 
{ 
    return x * other.x + y * other.y + z * other.z; 
}

// Cross product: returns Vec3 perpendicular to both vectors
Vec3 Vec3::cross(const Vec3& other) const 
{
    return Vec3(
        y * other.z - z * other.y,  // x-component of cross
        z * other.x - x * other.z,  // y-component of cross
        x * other.y - y * other.x   // z-component of cross
    );
}

// Component-wise addition of two Vec3 vectors
Vec3 Vec3::operator+(const Vec3& other) const 
{ 
    return Vec3(x + other.x, y + other.y, z + other.z); 
}

// Component-wise subtraction of two Vec3 vectors
Vec3 Vec3::operator-(const Vec3& other) const 
{ 
    return Vec3(x - other.x, y - other.y, z - other.z); 
}

// Multiply each component by a scalar value
Vec3 Vec3::operator*(double scalar) const 
{ 
    return Vec3(x * scalar, y * scalar, z * scalar); 
}

// Divide each component by a scalar value
// Throws runtime_error if scalar is zero
Vec3 Vec3::operator/(double scalar) const 
{
    if (scalar == 0.0)
    {
        throw std::runtime_error("Division by zero!");
    }
    return Vec3(x / scalar, y / scalar, z / scalar);
}

// Overload * operator for dot product between two Vec3
double Vec3::operator*(const Vec3& other) const { return this->dot(other); }

// Overload ^ operator for cross product between two Vec3
Vec3 Vec3::operator^(const Vec3& other) const { return this->cross(other); }

// Overload << operator for easy printing of Vec3
std::ostream& operator<<(std::ostream& os, const Vec3& v) 
{
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

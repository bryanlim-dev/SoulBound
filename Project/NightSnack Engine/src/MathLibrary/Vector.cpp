#include "Vector.h"

// Vec2
// Constructor (defaults to 0,0)
Vec2::Vec2(double x, double y) : x(x), y(y) {}

// Dot product
double Vec2::dot(const Vec2& other) const 
{
    return x * other.x + y * other.y;
}

// 2D Cross product (returns scalar)
double Vec2::cross(const Vec2& other) const 
{
    return x * other.y - y * other.x;
}

// Arithmetic operators
// Addition
Vec2 Vec2::operator+(const Vec2& other) const 
{
    return Vec2(x + other.x, y + other.y);
}

// Subtraction
Vec2 Vec2::operator-(const Vec2& other) const 
{
    return Vec2(x - other.x, y - other.y);
}

// Multiplication
Vec2 Vec2::operator*(double scalar) const 
{
    return Vec2(x * scalar, y * scalar);
}

// Division
Vec2 Vec2::operator/(double scalar) const 
{
    if (scalar == 0.0) throw std::runtime_error("Divide by zero!");
    return Vec2(x / scalar, y / scalar);
}

// Overload dot
double Vec2::operator*(const Vec2& other) const 
{
    return dot(other);
}

// Overload cross
double Vec2::operator^(const Vec2& other) const 
{
    return cross(other);
}

// Magnitude (length) using pythagoras theorem
double Vec2::magnitude() const 
{
    return std::sqrt(x * x + y * y);
}

// Stream output for Vec2: prints in form (x, y)
std::ostream& operator<<(std::ostream& os, const Vec2& v) {
    return os << "(" << v.x << ", " << v.y << ")";
}

// Vec3
Vec3::Vec3(double x, double y, double z) : x(x), y(y), z(z) {}

// Dot product
double Vec3::dot(const Vec3& other) const 
{
    return x * other.x + y * other.y + z * other.z;
}

// Cross product (returns vector)
Vec3 Vec3::cross(const Vec3& other) const 
{
    return Vec3(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    );
}

// Arithmetic operators
// Addition
Vec3 Vec3::operator+(const Vec3& other) const 
{
    return Vec3(x + other.x, y + other.y, z + other.z);
}

// Subtraction
Vec3 Vec3::operator-(const Vec3& other) const 
{
    return Vec3(x - other.x, y - other.y, z - other.z);
}

//Multiplication
Vec3 Vec3::operator*(double scalar) const 
{
    return Vec3(x * scalar, y * scalar, z * scalar);
}

// Division
Vec3 Vec3::operator/(double scalar) const 
{
    if (scalar == 0.0) throw std::runtime_error("Divide by zero!");
    return Vec3(x / scalar, y / scalar, z / scalar);
}

// Overload dot
double Vec3::operator*(const Vec3& other) const 
{
    return dot(other);
}

// Overload cross
Vec3 Vec3::operator^(const Vec3& other) const {
    return cross(other);
}

// Magnitude (length) using pythagoras theorem
double Vec3::magnitude() const 
{
    return std::sqrt(x * x + y * y + z * z);
}

// Stream output for Vec3: prints in form (x, y, z)
std::ostream& operator<<(std::ostream& os, const Vec3& v) {
    return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

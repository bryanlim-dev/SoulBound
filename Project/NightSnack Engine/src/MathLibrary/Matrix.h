#pragma once
#include <array>
#include <iostream>
#include <stdexcept>

struct Mat3 {
    // 3x3 matrix stored as a 2D array
    std::array<std::array<float, 3>, 3> Matrix3x3;

    // Constructors
    Mat3();  // Default
    Mat3(const std::array<std::array<float, 3>, 3>& values);

    Mat3 operator+(const Mat3& other) const;  // Matrix addition
    Mat3 operator-(const Mat3& other) const;  // Matrix subtraction
    Mat3 operator*(float scalar) const;      // Scalar multiplication
    Mat3 operator/(float scalar) const;      // Scalar division
    Mat3 operator*(const Mat3& other) const;  // Matrix multiplication

    float det() const;       // Determinant of the matrix
    Mat3 transpose() const;   // Transpose (swap rows/columns)
};

// Output Matrix
std::ostream& operator<<(std::ostream& os, const Mat3& mat);
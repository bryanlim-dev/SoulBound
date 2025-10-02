/******************************************************************************
 * File:        Matrix.cpp
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 100%) - 3x3 Matrix class
 *
 * Description:
 * This file declares the 3x3 matrix class and algebra operations.
 * Handles matrix addition, subtraction, scalar multiplication and divison, as well
 * as matrix multiplication. Has formatted output included as well for debug.
 * 
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "Matrix.h"

// Default constructor with value of 0
Mat3::Mat3() 
{
    Matrix3x3 = { { {0, 0, 0}, {0, 0, 0}, {0, 0, 0} } };
}

// Construct from a 2D array of values
Mat3::Mat3(const std::array<std::array<float, 3>, 3>& values)
    : Matrix3x3(values) {}

// Matrix addition
Mat3 Mat3::operator+(const Mat3& other) const 
{
    Mat3 result;
    for (int i = 0; i < 3; i++) 
    {
        for (int j = 0; j < 3; j++) 
        {
            result.Matrix3x3[i][j] = Matrix3x3[i][j] + other.Matrix3x3[i][j];
        }
    }
    return result;
}

// Matrix subtraction
Mat3 Mat3::operator-(const Mat3& other) const 
{
    Mat3 result;
    for (int i = 0; i < 3; i++) 
    {
        for (int j = 0; j < 3; j++) 
        {
            result.Matrix3x3[i][j] = Matrix3x3[i][j] - other.Matrix3x3[i][j];
        }
    }
    return result;
}

// Scalar multiplication
Mat3 Mat3::operator*(float scalar) const
{
    Mat3 result;
    for (int i = 0; i < 3; i++) 
    {
        for (int j = 0; j < 3; j++) 
        {
            result.Matrix3x3[i][j] = Matrix3x3[i][j] * scalar;
        }
    }
    return result;
}

// Scalar division
Mat3 Mat3::operator/(float scalar) const
{
    if (scalar == 0.0)
        throw std::runtime_error("Divide by zero");

    Mat3 result;
    for (int i = 0; i < 3; i++) 
    {
        for (int j = 0; j < 3; j++) 
        {
            result.Matrix3x3[i][j] = Matrix3x3[i][j] / scalar;
        }
    }
    return result;
}

// Matrix multiplication
Mat3 Mat3::operator*(const Mat3& other) const 
{
    Mat3 result;
    for (int i = 0; i < 3; i++) 
    {
        for (int j = 0; j < 3; j++) 
        {
            result.Matrix3x3[i][j] = 0; // initialize cell
            for (int k = 0; k < 3; k++) 
            {
                result.Matrix3x3[i][j] += Matrix3x3[i][k] * other.Matrix3x3[k][j];
            }
        }
    }
    return result;
}

// Determinant of 3x3 matrix
float Mat3::det() const
{
    return
        Matrix3x3[0][0] * (Matrix3x3[1][1] * Matrix3x3[2][2] - Matrix3x3[1][2] * Matrix3x3[2][1]) -
        Matrix3x3[0][1] * (Matrix3x3[1][0] * Matrix3x3[2][2] - Matrix3x3[1][2] * Matrix3x3[2][0]) +
        Matrix3x3[0][2] * (Matrix3x3[1][0] * Matrix3x3[2][1] - Matrix3x3[1][1] * Matrix3x3[2][0]);
}

// Transpose of the matrix (swap rows and columns)
Mat3 Mat3::transpose() const 
{
    Mat3 result;
    for (int i = 0; i < 3; i++) 
    {
        for (int j = 0; j < 3; j++) 
        {
            result.Matrix3x3[i][j] = Matrix3x3[j][i];
        }
    }
    return result;
}

// Output matrix
std::ostream& operator<<(std::ostream& os, const Mat3& mat) 
{
    for (int i = 0; i < 3; i++) 
    {
        os << "| ";
        for (int j = 0; j < 3; j++) 
        {
            os << mat.Matrix3x3[i][j] << " ";
        }
        os << "|\n";
    }
    return os;
}

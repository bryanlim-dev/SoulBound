#include "Matrix.h"

// Default constructor with value of 0
Mat3::Mat3() 
{
    m = { { {0, 0, 0}, {0, 0, 0}, {0, 0, 0} } };
}

// Construct from a 2D array of values
Mat3::Mat3(const std::array<std::array<double, 3>, 3>& values)
    : m(values) {}

// Matrix addition
Mat3 Mat3::operator+(const Mat3& other) const 
{
    Mat3 result;
    for (int i = 0; i < 3; i++) 
    {
        for (int j = 0; j < 3; j++) 
        {
            result.m[i][j] = m[i][j] + other.m[i][j];
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
            result.m[i][j] = m[i][j] - other.m[i][j];
        }
    }
    return result;
}

// Scalar multiplication
Mat3 Mat3::operator*(double scalar) const 
{
    Mat3 result;
    for (int i = 0; i < 3; i++) 
    {
        for (int j = 0; j < 3; j++) 
        {
            result.m[i][j] = m[i][j] * scalar;
        }
    }
    return result;
}

// Scalar division
Mat3 Mat3::operator/(double scalar) const 
{
    if (scalar == 0.0)
        throw std::runtime_error("Divide by zero!");

    Mat3 result;
    for (int i = 0; i < 3; i++) 
    {
        for (int j = 0; j < 3; j++) 
        {
            result.m[i][j] = m[i][j] / scalar;
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
            result.m[i][j] = 0; // initialize cell
            for (int k = 0; k < 3; k++) 
            {
                result.m[i][j] += m[i][k] * other.m[k][j];
            }
        }
    }
    return result;
}

// Determinant of 3x3 matrix
double Mat3::det() const 
{
    return
        m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
        m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
        m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
}

// Transpose of the matrix (swap rows and columns)
Mat3 Mat3::transpose() const 
{
    Mat3 result;
    for (int i = 0; i < 3; i++) 
    {
        for (int j = 0; j < 3; j++) 
        {
            result.m[i][j] = m[j][i];
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
            os << mat.m[i][j] << " ";
        }
        os << "|\n";
    }
    return os;
}

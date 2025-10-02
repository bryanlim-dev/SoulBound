/******************************************************************************
 * File:        Matrix.h
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 100%) - 3x3 Matrix class
 *
 * Description:
 * This file defines the 3x3 matrix class and algebra operations.
 * Handles matrix addition, subtraction, scalar multiplication and divison, as well
 * as matrix multiplication. Has formatted output included as well for debug.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
#pragma once
#include <array>
#include <iostream>
#include <stdexcept>

struct Mat3 {
    /************************************************************************/
    /*!
    \brief
    3x3 matrix stored as 2d array
    */
    /************************************************************************/
    std::array<std::array<float,3>,3> Matrix3x3;

    /************************************************************************/
    /*!
    \brief
    Default constructor
    */
    /************************************************************************/
    Mat3();

    /************************************************************************/
    /*!
    \brief
    Construct matrix from 2d array of values.

    \param values
    3x3 array of matrix values
    */
    /************************************************************************/
    Mat3(const std::array<std::array<float,3>,3>& values);

    /************************************************************************/
    /*!
    \brief
    Matrix addition with another matrix.

    \param other
    Matrix to add.

    \return
    Sum of matrix.
    */
    /************************************************************************/
    Mat3 operator+(const Mat3& other) const;

    /************************************************************************/
    /*!
    \brief
    Matrix subtraction with another matrix.

    \param other
    Matrix to subtract.

    \return
    Resultant matrix.
    */
    /************************************************************************/
    Mat3 operator-(const Mat3& other) const; 

    /************************************************************************/
    /*!
    \brief
    Matrix multiplication with a scalar.

    \param scalar
    Scalar value.

    \return
    Scaled mat3
    */
    /************************************************************************/
    Mat3 operator*(float scalar) const;

    /************************************************************************/
    /*!
    \brief
    Matrix division with a scalar.

    \param scalar
    Scalar value.

    \return
    Scaled mat3
    */
    /************************************************************************/
    Mat3 operator/(float scalar) const; 

    /************************************************************************/
    /*!
    \brief
    Matrix multiplication with another matrix.

    \param other
    Other matrix

    \return
    New mat3.
    */
    /************************************************************************/
    Mat3 operator*(const Mat3& other) const; 

    /************************************************************************/
    /*!
    \brief
    Determinant of 3x3 matrix.

    \return
    float determinant value
    */
    /************************************************************************/
    float det() const;

    /************************************************************************/
    /*!
    \brief
    Transpose of 3x3 matrix. (row and column swap)

    \return
    transposed mat3
    */
    /************************************************************************/
    Mat3 transpose() const;
};

/************************************************************************/
/*!
\brief
Overloaded output stream operator for printing mat3.

\param os
Output stream.

\param v
Matrix to print.

\return
Modified output stream
*/
/************************************************************************/
std::ostream& operator<<(std::ostream& os, const Mat3& mat);
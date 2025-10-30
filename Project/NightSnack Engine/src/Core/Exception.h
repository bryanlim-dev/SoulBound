/******************************************************************************
 * File:        Exception.h
 * Project:     NightSnack
 * Author(s):   Lim Wei Jie (Primary, 100%) - Created exception class to handle
 *              different types of exceptions for easy debugging
 *
 * Description: This file contains the declaration of the IException class,
 *              which encapsulates error messages, types, and sources for exceptions.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include <string>
#include <utility>
#include <iostream>

// Macro to throw an IException with file info
#define THROW_IEXCEPTION(msg, type) IException((msg), (type), __FILE__)

namespace NightSnack {

    // Different types of exceptions
    enum class ExceptionType
    {
        General,
        AppWindow,
        Input,
        Collision,
        Graphics,
        Physics,
        ECS
    };

    /************************************************************************/
    /*!
    \brief
    Converts an ExceptionType enum value to its corresponding string representation.

    \param[in] type
    The ExceptionType enum value to convert.
    */
    /************************************************************************/
    inline const char* ExceptionTypeToString(ExceptionType type)
    {
        switch (type)
        {
        case ExceptionType::General:   return "General";
        case ExceptionType::AppWindow: return "AppWindow";
        case ExceptionType::Input:     return "Input";
        case ExceptionType::Collision: return "Collision";
        case ExceptionType::Graphics:  return "Graphics";
        case ExceptionType::Physics:   return "Physics";
        case ExceptionType::ECS:       return "ECS";
        default:                       return "Unknown";
        }
    }

    // IException class encapsulates error messages, types, and sources for exceptions.
    class IException
    {
    public:
        /************************************************************************/
        /*!
        \brief
        Default constructor to setup initial IException values;
        */
        /************************************************************************/
        IException(std::string message, ExceptionType type = ExceptionType::General, std::string source = __FILE__)
            : m_message(std::move(message)), m_type(type), m_source(std::move(source)) {
        }

        // Getters
        ExceptionType type() const noexcept
        {
            return m_type;
        }
        const std::string& message() const noexcept
        {
            return m_message;
        }
        const std::string& source() const noexcept
        {
            return m_source;
        }

        /************************************************************************/
        /*!
        \brief
        Prints the exception details to standard error output.
        */
        /************************************************************************/
        void print() const
        {
            std::cerr << "[Exception]"
                << "[" << ExceptionTypeToString(m_type) << "] \n"
                << "[" << (m_source.empty() ? "Unknown" : m_source) << "]\n"
                << m_message << std::endl;
        }

    private:
        // Exception details
        ExceptionType m_type;
        std::string m_message;
        std::string m_source;
    };

}   // NightSnack

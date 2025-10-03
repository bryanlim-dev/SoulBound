/******************************************************************************
 * File:        Exception.h
 * Project:     NightSnack
 * Author(s):
 *
 * Description:
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include <string>
#include <utility>
#include <iostream>

#define THROW_IEXCEPTION(msg, type) IException((msg), (type), __FILE__)

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

class IException
{
public:
    IException(std::string message, ExceptionType type = ExceptionType::General, std::string source = __FILE__)
        : m_message(std::move(message)), m_type(type), m_source(std::move(source)) {
    }

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

    void print() const
    {
        std::cerr << "[Exception]"
            << "[" << ExceptionTypeToString(m_type) << "] \n"
            << "[" << (m_source.empty() ? "Unknown" : m_source) << "]\n"
            << m_message << std::endl;
    }

private:
    ExceptionType m_type;
    std::string m_message;
    std::string m_source;
};

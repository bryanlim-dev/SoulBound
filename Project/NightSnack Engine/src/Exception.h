#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <string>
#include <utility>
#include <iostream>

enum class ExceptionType
{
    GENERAL,
    INPUT,
    COLLISION,
    GRAPHICS,
    ECS
};

class IException
{
public:
    IException(std::string message, ExceptionType type = ExceptionType::GENERAL)
        : m_message(std::move(message)), m_type(type) {
    }

    ExceptionType type() const noexcept
    { 
        return m_type; 
    }

    const std::string& message() const noexcept
    {
        return m_message;
    }

    void print() const
    {
        std::cerr << "[Exception][" << static_cast<int>(m_type) << "] "
            << m_message << std::endl;
	}

private:
    ExceptionType m_type;

    std::string m_message;
};

#endif // EXCEPTION_HPP

#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <string>
#include <utility>

class IException
{
public:
    explicit IException(std::string message)
        : m_message(std::move(message)) {}

    const std::string& message() const noexcept
    {
        return m_message;
    }

private:
    std::string m_message;
};

#endif // EXCEPTION_HPP

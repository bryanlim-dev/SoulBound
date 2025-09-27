#include "Timer.h"
#include <GLFW/glfw3.h>

Timer::Timer()
    : m_lastFrameTime(static_cast<float>(glfwGetTime())), m_currentFrameTime(0.f), m_endFrameTime(0.f), m_deltaTime(0.f), 
    m_fps(0.f), m_frameCount(0), m_lastFpsUpdateTime(static_cast<float>(glfwGetTime())), m_totalSystemTime(0.f)
{
	glfwSwapInterval(1); // Enable VSync
}

void Timer::StartFrame() 
{
    // Delta time calculation
    m_currentFrameTime = static_cast<float>(glfwGetTime());
    m_deltaTime = m_currentFrameTime - m_lastFrameTime;
    m_lastFrameTime = m_currentFrameTime;
    m_frameCount++;

	// FPS updated every second
    if (m_currentFrameTime - m_lastFpsUpdateTime >= 1.f)
    {
        m_fps = m_frameCount / (m_currentFrameTime - m_lastFpsUpdateTime);
        m_lastFpsUpdateTime = m_currentFrameTime;
        m_frameCount = 0;
    }
}

void Timer::EndFrame() 
{
	m_endFrameTime = static_cast<float>(glfwGetTime());
	m_totalSystemTime += (m_endFrameTime - m_currentFrameTime);
}

void Timer::FrameCap(float targetFPS)
{
    if (targetFPS <= 0.f) return;

    float targetFrameTime = 1.f / targetFPS;
    float frameEnd = static_cast<float>(glfwGetTime());
    while (frameEnd - m_currentFrameTime < targetFrameTime)
    {
        frameEnd = static_cast<float>(glfwGetTime());
    }
}

void Timer::StartSystemTimer(const std::string& name)
{
    m_systemStartTimes[name] = static_cast<float>(glfwGetTime());
}

void Timer::EndSystemTimer(const std::string& name)
{
    float end = static_cast<float>(glfwGetTime());
    float elapsed = end - m_systemStartTimes[name];
    m_systemsTimes[name] += elapsed;
}

void Timer::DisplaySystemTimers() const
{
    if (m_systemsTimes.empty() || m_totalSystemTime == 0.f)
    {
        return;
    }

    for (const auto& [name, time] : m_systemsTimes)
    {
        float percent = (time / m_totalSystemTime) * 100.f;
        std::cout << std::left << std::setw(36) << (name + " Time Consumption :")
            << std::right << std::setw(8) << std::fixed << std::setprecision(4) << percent
            << "% |" << std::endl;
    }
    std::cout << std::endl;
}

void Timer::ResetSystemsTimers()
{
    m_systemsTimes.clear();
	m_systemStartTimes.clear();
    m_totalSystemTime = 0.f;
}

float Timer::GetDT() const
{
    return m_deltaTime;
}

float Timer::GetFPS() const
{
    return m_fps;
}

float Timer::GetTotalSystemTime() const
{
    return m_totalSystemTime;
}

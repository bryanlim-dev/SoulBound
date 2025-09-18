#include "Timer.h"
#include <GLFW/glfw3.h>

Timer::Timer()
    : m_lastFrameTime(glfwGetTime()), m_currentFrameTime(0.0), m_endFrameTime(0.0), m_deltaTime(0.0), 
    m_fps(0.0), m_frameCount(0), m_lastFpsUpdateTime(glfwGetTime()), m_totalSystemTime(0.0) {}

void Timer::StartFrame() 
{
    // Delta time calculation
    m_currentFrameTime = glfwGetTime();
    m_deltaTime = m_currentFrameTime - m_lastFrameTime;
    m_lastFrameTime = m_currentFrameTime;
    m_frameCount++;

	// FPS updated every second
    if (m_currentFrameTime - m_lastFpsUpdateTime >= 1.0)
    {
        m_fps = m_frameCount / (m_currentFrameTime - m_lastFpsUpdateTime);
        m_lastFpsUpdateTime = m_currentFrameTime;
        m_frameCount = 0;
    }
}

void Timer::EndFrame() 
{
	m_endFrameTime = glfwGetTime();
	m_totalSystemTime += (m_endFrameTime - m_currentFrameTime);
}

double Timer::GetDT() const 
{ 
    return m_deltaTime; 
}

double Timer::GetFPS() const
{
    return m_fps;
}

double Timer::GetTotalSystemTime() const
{
    return m_totalSystemTime;
}

void Timer::StartSystemTimer(const std::string& name)
{
    m_systemStartTimes[name] = glfwGetTime();
}

void Timer::EndSystemTimer(const std::string& name)
{
    double end = glfwGetTime();
    double elapsed = end - m_systemStartTimes[name];
    m_systemsTimes[name] += elapsed;
    m_totalSystemTime += elapsed;
}

void Timer::DisplaySystemTimers() const
{
    if (m_systemsTimes.empty() || m_totalSystemTime == 0.0)
    {
        return;
    }

    for (const auto& [name, time] : m_systemsTimes)
    {
        double percent = (time / m_totalSystemTime) * 100.0;
        std::cout << name << ": " << std::fixed << std::setprecision(4)
            << percent << "% of game loop | ";
    }
    std::cout << std::endl;
}

void Timer::ResetSystemsTimers()
{
    m_systemsTimes.clear();
	m_systemStartTimes.clear();
    m_totalSystemTime = 0.0;
}

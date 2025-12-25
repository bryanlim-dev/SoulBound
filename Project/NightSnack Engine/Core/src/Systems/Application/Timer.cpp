/******************************************************************************
 * File:        Timer.cpp
 * Project:     NightSnack
 * Author(s):   Lim Wei Jie (Primary, 100%) - Created a Timer class to handle
 *              frame timing, delta time, FPS calculation and system time tracking
 *
 * Description: This file contains the implementation of the Timer class.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "Timer.h"
#include <imgui.h>
#include <GLFW/glfw3.h>

namespace NightSnack {

    // Default constructor to setup initial Timer values;
    Timer::Timer()
        : m_lastFrameTime(static_cast<float>(glfwGetTime())), m_currentFrameTime(m_lastFrameTime), m_endFrameTime(m_lastFrameTime), m_deltaTime(0.f),
        m_fps(0.f), m_frameCount(0), m_lastFpsUpdateTime(static_cast<float>(glfwGetTime())), m_totalSystemTime(0.f)
    {
        EnableVSync(1);
    }

    // Starts game loop timer, delta time and FPS calculation
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

    // Ends game loop timer and accumulates total game loop time
    void Timer::EndFrame()
    {
        m_endFrameTime = static_cast<float>(glfwGetTime());
        m_totalSystemTime += (m_endFrameTime - m_currentFrameTime);
    }

    // Toggles VSync mode
    void Timer::EnableVSync(bool enabled)
    {
        glfwSwapInterval(enabled);
    }

    // Caps the frame rate to the target FPS
    void Timer::FrameCap(float targetFPS)
    {
        if (targetFPS <= 0.f) return;

        // Calculate the time to wait until the next frame
        float targetFrameTime = 1.f / targetFPS;
        float frameEnd = static_cast<float>(glfwGetTime());
        while (frameEnd - m_currentFrameTime < targetFrameTime)
        {
            frameEnd = static_cast<float>(glfwGetTime());
        }
    }

    // Starts a system timer with the given name
    void Timer::StartSystemTimer(const std::string& name)
    {
        m_systemStartTimes[name] = static_cast<float>(glfwGetTime());
    }

    // Ends a system timer with the given name and accumulates elapsed time
    void Timer::EndSystemTimer(const std::string& name)
    {
        float end = static_cast<float>(glfwGetTime());
        float elapsed = end - m_systemStartTimes[name];
        m_systemsTimes[name] += elapsed;
    }

    // Displays the percentage time consumption of each system
    void Timer::DisplaySystemTimers() const
    {
        if (m_systemsTimes.empty() || m_totalSystemTime == 0.f)
        {
            return;
        }

        ImGui::BeginChild("SystemTimers", ImVec2(0, 0), false);

        // Max label length for alignment
        std::string label;
        size_t maxLabelLen = 0;       
        for (const auto& [name, _] : m_systemsTimes)
        {
            label = name + " :";
            size_t len = (label).length();
            if (len > maxLabelLen) { maxLabelLen = len; }
        }

        // Print system timers
        for (const auto& [name, time] : m_systemsTimes)
        {
            float percent = (time / m_totalSystemTime) * 100.f;
            label = name + " :";
            std::ostringstream oss;
            oss << std::fixed << std::setw(8) << std::setprecision(4) << percent << "% |";

            // Pad label to max length
            std::string paddedLabel = label + std::string(maxLabelLen - label.length(), ' ');

            ImGui::Text("%s%s", paddedLabel.c_str(), oss.str().c_str());
        }

        ImGui::EndChild();

        // Display formatted table of system times to console
        /*for (const auto& [name, time] : m_systemsTimes)
        {
            float percent = (time / m_totalSystemTime) * 100.f;
            //std::cout << std::left << std::setw(36) << (name + " Time Consumption :")
                << std::right << std::setw(8) << std::fixed << std::setprecision(4) << percent
                << "% |" << std::endl;
        }
        //std::cout << std::endl;*/
    }

    // Resets all system timers and total system time
    void Timer::ResetSystemsTimers()
    {
        m_systemsTimes.clear();
        m_systemStartTimes.clear();
        m_totalSystemTime = 0.f;
    }

    float Timer::GetDT() const
    {
        static bool firstFrame;
        if (firstFrame) { return 0.f; }
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

}   // NightSnack

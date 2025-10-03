/******************************************************************************
 * File:        Timer.h
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
#include <unordered_map>
#include <functional>
#include <vector>
#include <iostream>
#include <iomanip>

class Timer
{
public:
	// Default constructor to setup initial Timer values;
    Timer();

    // Frame timing
    void    StartFrame();
    void    EndFrame();
	void    FrameCap(float targetFPS);

    // System timing
    void    StartSystemTimer(const std::string& name);
    void    EndSystemTimer(const std::string& name);
    void    DisplaySystemTimers() const;
    void    ResetSystemsTimers();

    // Getters
    float  GetDT() const;
    float  GetFPS() const;
    float  GetTotalSystemTime() const;


private:
    // Frame timing
    int     m_frameCount;
    float  m_currentFrameTime;
    float  m_lastFrameTime;
    float  m_endFrameTime;

    float  m_deltaTime;
    float  m_fps;
    float  m_lastFpsUpdateTime;

    // System timing
    std::unordered_map<std::string, float> m_systemsTimes;
    std::unordered_map<std::string, float> m_systemStartTimes;
    float  m_totalSystemTime;
};

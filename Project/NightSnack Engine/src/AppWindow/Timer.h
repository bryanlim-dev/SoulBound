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
    double  GetDT() const;
    double  GetFPS() const;
    double  GetTotalSystemTime() const;

    // System timing
    void    StartSystemTimer(const std::string& name);
    void    EndSystemTimer(const std::string& name);
	void    DisplaySystemTimers() const;
    void    ResetSystemsTimers();

private:
    // Frame timing
    int     m_frameCount;
	double  m_currentFrameTime;
    double  m_lastFrameTime;
	double  m_endFrameTime;

    double  m_deltaTime;
    double  m_fps;
    double  m_lastFpsUpdateTime;

    // System timing
    std::unordered_map<std::string, double> m_systemsTimes;
    std::unordered_map<std::string, double> m_systemStartTimes;
    double  m_totalSystemTime;
};

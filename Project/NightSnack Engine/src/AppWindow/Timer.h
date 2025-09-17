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
    int     frameCount_;
	double  currentFrameTime_;
    double  lastFrameTime_;
	double  endFrameTime_;

    double  deltaTime_;
    double  fps_;
    double  lastFpsUpdateTime_;

    // System timing
    std::unordered_map<std::string, double> systemsTimes_;
    std::unordered_map<std::string, double> systemStartTimes_;
    double  totalSystemTime_;
};

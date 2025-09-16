#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>

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

private:
    // Frame timing
    int     frameCount_;
    double  lastFrameTime_;
    double  deltaTime_;
    double  fps_;
    double  lastFpsUpdateTime_;
};

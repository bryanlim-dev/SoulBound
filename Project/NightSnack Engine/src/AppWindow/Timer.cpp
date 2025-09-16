#include "Timer.h"
#include <GLFW/glfw3.h>

Timer::Timer()
    : lastFrameTime_(glfwGetTime()), deltaTime_(0.0), fps_(0.0),
    frameCount_(0), lastFpsUpdateTime_(glfwGetTime()) {}

void Timer::StartFrame() 
{
	// Delta time calculation
    double currentFrameTime_ = glfwGetTime();
    deltaTime_ = currentFrameTime_ - lastFrameTime_;
    lastFrameTime_ = currentFrameTime_;
    frameCount_++;

	// FPS updated every second
    if (currentFrameTime_ - lastFpsUpdateTime_ >= 1.0)
    {
        fps_ = frameCount_ / (currentFrameTime_ - lastFpsUpdateTime_);
        lastFpsUpdateTime_ = currentFrameTime_;
        frameCount_ = 0;
    }
}

void Timer::EndFrame() 
{
    // Reserved for future use
}

double Timer::GetDT() const 
{ 
    return deltaTime_;
}

double Timer::GetFPS() const 
{ 
    return fps_;
}

#include "Timer.h"
#include <GLFW/glfw3.h>

Timer::Timer()
    : lastFrameTime_(glfwGetTime()), currentFrameTime_(0.0), endFrameTime_(0.0), deltaTime_(0.0), 
    fps_(0.0), frameCount_(0), lastFpsUpdateTime_(glfwGetTime()), totalSystemTime_(0.0) {}

void Timer::StartFrame() 
{
    // Delta time calculation
    currentFrameTime_ = glfwGetTime();
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
	endFrameTime_ = glfwGetTime();
	totalSystemTime_ += (endFrameTime_ - currentFrameTime_);
}

double Timer::GetDT() const 
{ 
    return deltaTime_; 
}

double Timer::GetFPS() const
{
    return fps_;
}

double Timer::GetTotalSystemTime() const
{
    return totalSystemTime_;
}

void Timer::StartSystemTimer(const std::string& name)
{
    systemStartTimes_[name] = glfwGetTime();
}

void Timer::EndSystemTimer(const std::string& name)
{
    double end = glfwGetTime();
    double elapsed = end - systemStartTimes_[name];
    systemsTimes_[name] += elapsed;
    totalSystemTime_ += elapsed;
}

void Timer::DisplaySystemTimers() const
{
    if (systemsTimes_.empty() || totalSystemTime_ == 0.0)
    {
        return;
    }

    for (const auto& [name, time] : systemsTimes_)
    {
        double percent = (time / totalSystemTime_) * 100.0;
        std::cout << name << ": " << std::fixed << std::setprecision(4)
            << percent << "% of game loop | ";
    }
    std::cout << std::endl;
}

void Timer::ResetSystemsTimers()
{
    systemsTimes_.clear();
	systemStartTimes_.clear();
    totalSystemTime_ = 0.0;
}

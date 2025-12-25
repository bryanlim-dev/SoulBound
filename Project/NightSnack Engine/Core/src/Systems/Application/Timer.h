/******************************************************************************
 * File:        Timer.h
 * Project:     NightSnack
 * Author(s):   Lim Wei Jie (Primary, 100%) - Created a Timer class to handle
 *              frame timing, delta time, FPS calculation and system time tracking
 *
 * Description: This file contains the declaration of the Timer class.
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
#include <sstream>
#include <iomanip>

namespace NightSnack {

    class Timer
    {
    public:
        /************************************************************************/
        /*!
        \brief
        Default constructor to setup initial Timer values;
        */
        /************************************************************************/
        Timer();

        /************************************************************************/
        /*!
        \brief
        Starts game loop timer, delta time and FPS calculation
        */
        /************************************************************************/
        void    StartFrame();

        /************************************************************************/
        /*!
        \brief
        Ends game loop timer and accumulates total game loop time
        */
        /************************************************************************/
        void    EndFrame();

        /************************************************************************/
        /*!
        \brief
        Toggles VSync mode

        \param[in] enabled
        Flag to set VSync
        */
        /************************************************************************/
        void    EnableVSync(bool enabled = true);

        /************************************************************************/
        /*!
        \brief
        Caps the frame rate to the target FPS

        \param[in] targetFPS
        The desired frames per second to cap the game loop to
        */
        /************************************************************************/
        void    FrameCap(float targetFPS);

        /************************************************************************/
        /*!
        \brief
        Starts timing a specific system by parameter name

        \param[in] name
        The name of the system to start timing
        */
        /************************************************************************/
        void    StartSystemTimer(const std::string& name);

        /************************************************************************/
        /*!
        \brief
        Ends timing a specific system by parameter name and accumulates the time taken

        \param[in] name
        The name of the system to end timing
        */
        /************************************************************************/
        void    EndSystemTimer(const std::string& name);

        /************************************************************************/
        /*!
        \brief
        Displays the accumulated times for all systems to the console in a formatted table
        */
        /************************************************************************/
        void    DisplaySystemTimers() const;

        /************************************************************************/
        /*!
        \brief
        Resets all system timers and total system time to zero
        */
        /************************************************************************/
        void    ResetSystemsTimers();

        /************************************************************************/
        /*!
        \brief
        Gets the delta time
        */
        /************************************************************************/
        float  GetDT() const;

        /************************************************************************/
        /*!
        \brief
        Gets the current FPS
        */
        /************************************************************************/
        float  GetFPS() const;

        /************************************************************************/
        /*!
        \brief
        Gets the total accumulated game loop time
        */
        /************************************************************************/
        float  GetTotalSystemTime() const;


    private:
        // Frame timing
        int    m_frameCount;
        float  m_currentFrameTime;
        float  m_lastFrameTime;
        float  m_endFrameTime;

        // Delta time and FPS
        float  m_deltaTime;
        float  m_fps;
        float  m_lastFpsUpdateTime;

        // System timing
        std::unordered_map<std::string, float> m_systemsTimes;
        std::unordered_map<std::string, float> m_systemStartTimes;
        float  m_totalSystemTime;
    };

}   // NightSnack

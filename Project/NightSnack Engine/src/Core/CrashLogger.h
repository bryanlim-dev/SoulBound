/******************************************************************************
 * File:        CrashLogger.h
 * Project:     NightSnack
 * Author(s):   Lim Wei Jie (Primary, 100%) - Created CrashLogger to log 
 *              exceptions to a file
 *
 * Description: This file contains the declaration of the CrashLogger class,
 *              which logs crash information from IException to a text file in
 *              the folder "CrashLog".
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include <string>
#include <ctime>
#include <filesystem>
#include <fstream>
#include "Exception.h"
#include "ResourceManager/ResourceManager.h"

namespace NightSnack {

    class CrashLogger
    {
    public:
        /************************************************************************/
        /*!
        \brief
        Logs crash information from an IException to a text file "crash_log.txt"
        in the "CrashLog" directory. Each log entry includes a timestamp, exception
        type, source, and message. If the log file already exists, the new entry
        is appended below the existing content.

        \param[in] ex
        The IException instance containing crash details.
        */
        /************************************************************************/
        static void LogCrash(const IException& ex) noexcept
        {
            // Exception safety: ensure no exceptions escape this function
            try
            {
                // Ensure the CrashLog directory exists
                std::string folder = "CrashLog";
                std::string filePath = folder + "/crash_log.txt";
                std::filesystem::create_directories(folder);

                // Get current local time
                std::time_t now = std::time(nullptr);
                struct tm timeinfo;
                localtime_s(&timeinfo, &now);

                // Format log entry
                char timeStr[26]; // Buffer for time string
                asctime_s(timeStr, sizeof(timeStr), &timeinfo); // fills timeStr with formatted time
                std::string logEntry =
                    "[CRASH] " + std::string(timeStr) +
                    "[" + std::string(ExceptionTypeToString(ex.type())) + "]\n" +
                    "[" + (ex.source().empty() ? "Unknown" : ex.source()) + "]\n" +
                    "Message: " + ex.message() + "\n" +
                    "------------------------------------";

                // Open file in append mode and write the log entry
                std::ofstream logFile(filePath, std::ios::app);
                if (logFile.is_open())
                {
                    logFile << logEntry;
                    logFile.close();
                }

            }
            catch (...)
            {
                // Exception safety: do nothing if logging fails
            }
        }
        /************************************************************************/
        /*!
        \brief
        Forcefully throw an IException for testing purposes.

        \param[in] message
        The message to include in the forced exception.
        */
        /************************************************************************/
        static void ForceCrash(const std::string& message = "Forced crash for testing")
        {
            IException ex(message, ExceptionType::General, __FILE__);
            throw ex;
        }
    };
    // Check NightSnack Engine/build/CrashLog/crash_log.txt for the entry

}   // NightSnack;


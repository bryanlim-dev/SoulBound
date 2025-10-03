#pragma once

#include <string>
#include <ctime>
#include <filesystem>
#include "Exception.h"
#include "ResourceManager/ResourceManager.h"

class CrashLogger 
{
public:
    static void LogCrash(const IException& ex) noexcept 
    {
        try 
        {
            std::string folder = "CrashLog";
            std::string filePath = folder + "/crash_log.txt";
            std::filesystem::create_directories(folder);

            std::time_t now = std::time(nullptr);
            struct tm timeinfo;
            localtime_s(&timeinfo, &now);

            char timeStr[26]; // Buffer for time string
            asctime_s(timeStr, sizeof(timeStr), &timeinfo); // fills timeStr with formatted time
            std::string logEntry =
                "[CRASH] " + std::string(timeStr) +
                "[" + std::string(ExceptionTypeToString(ex.type())) + "]\n" +
                "[" + (ex.source().empty() ? "Unknown" : ex.source()) + "]\n" +
                "Message: " + ex.message() + "\n" +
                "------------------------------------";

            // Load existing log file if it exists
            std::size_t txtKey = 0;
            std::vector<std::string> logVec;
            ResourceManager* rm = ResourceManager::GetInstance();
            if (rm->LoadTxtFile(filePath, txtKey) == LOAD_SUCCESS) {
                logVec = rm->GetTxtFile(txtKey);
            }
            logVec.push_back(logEntry);

            rm->SaveTxtFile(filePath, logVec);
        }
        catch (...) 
        {
            // Exception safety: do nothing if logging fails
        }
    }

    static void ForceCrash(const std::string& message = "Forced crash for testing")
    {
        IException ex(message, ExceptionType::General, __FILE__);
        throw ex;
    }
};

// Check NightSnack Engine/build/CrashLog/crash_log.txt for the entry

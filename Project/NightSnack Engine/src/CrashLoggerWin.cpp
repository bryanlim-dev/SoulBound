#pragma once

#ifdef _WIN32
#ifdef APIENTRY
#undef APIENTRY
#endif
#include <windows.h>
#include "CrashLogger.h"
#include <fstream>

LONG WINAPI GlobalExceptionHandler(EXCEPTION_POINTERS* ExceptionInfo)
{
    DWORD code = ExceptionInfo->ExceptionRecord->ExceptionCode;
    void* addr = ExceptionInfo->ExceptionRecord->ExceptionAddress;

    // Simple message
    std::string msg = "Unhandled Exception: Code 0x" + std::to_string(code) +
        " at address " + std::to_string(reinterpret_cast<uintptr_t>(addr));

    CrashLogger::LogCrash(IException(msg, ExceptionType::General, __FILE__));

    return EXCEPTION_EXECUTE_HANDLER;
}

void CrashLogger::InstallGlobalHandler() {
#ifdef _WIN32
    SetUnhandledExceptionFilter(GlobalExceptionHandler);
#endif
}

#endif

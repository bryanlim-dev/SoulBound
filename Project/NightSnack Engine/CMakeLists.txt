# ------------------------------------------------------------ #
# CMake file for NightSnack Engine
# Team: Midnight Munchies
# Module: GAM_200
# ------------------------------------------------------------ #

# Minimum version of CMake required
cmake_minimum_required(VERSION 3.30.3)

# ------------------------------------------------------------
# Project Setup
# ------------------------------------------------------------
project(NightSnack LANGUAGES CXX)

# Global C++ settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Use static MSVC runtime (/MT or /MTd)
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Set output directories to organized folders with config subdirectories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/NightSnack/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/NightSnack/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/NightSnack/$<CONFIG>/lib)

# Make sure all configurations output to separate directories
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG} CONFIG_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/NightSnack/${CONFIG})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/NightSnack/${CONFIG})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/NightSnack/${CONFIG}/lib)
endforeach()

# Enable folder organization in Visual Studio
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# ------------------------------------------------------------
# Third-Party Dependencies (shared across projects)
# ------------------------------------------------------------
add_subdirectory(extern)

# ------------------------------------------------------------
# Engine Projects
# ------------------------------------------------------------
add_subdirectory(Core)
add_subdirectory(ManagedScripts)
add_subdirectory(Game)

# Set startup project for Visual Studio
if(MSVC)
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Game)
endif()
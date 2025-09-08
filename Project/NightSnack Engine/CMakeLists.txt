# ------------------------------------------------------------ #
# CMake file for NightSnack Engine
# Team: Midnight Munchies
# Module: GAM_200
# ------------------------------------------------------------ #

# Minimum version of CMake required
cmake_minimum_required(VERSION 3.5)

# ------------------------------------------------------------
# Project Setup
# ------------------------------------------------------------
project(NightSnack LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use static MSVC runtime (/MT or /MTd)
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# ------------------------------------------------------------
# Gather Source and Header Files
# ------------------------------------------------------------
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE ENGINE_HEADERS CONFIGURE_DEPENDS src/*.h)

add_executable(${PROJECT_NAME} ${ENGINE_SOURCES} ${ENGINE_HEADERS})

# ------------------------------------------------------------
# Organize files in Visual Studio Solution Explorer
# ------------------------------------------------------------
# Engine sources -> mirror folder tree under "Engine"
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" 
  PREFIX "Engine" 
  FILES ${ENGINE_SOURCES} ${ENGINE_HEADERS}
)

# ------------------------------------------------------------
# Assets Handling
# ------------------------------------------------------------
# Define resource path relative to executable
target_compile_definitions(${PROJECT_NAME} PUBLIC RESOURCE_PATH=\"./assets/\")

# Copy assets folder to output directory (per config: Debug/Release)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)

# ------------------------------------------------------------
# External Dependencies
# ------------------------------------------------------------
# GLFW
add_subdirectory(extern/glfw EXCLUDE_FROM_ALL)

# Glad
add_library(glad extern/glad/src/glad.c)
target_include_directories(glad PUBLIC extern/glad/include)

# stb_image (header-only)
add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE extern/stb_image/include)

# fmod
add_library(fmod INTERFACE)
target_include_directories(fmod INTERFACE extern/fmod/inc)
#copy fmod dll
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/fmod/lib
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/extern/fmod/lib
)

# ------------------------------------------------------------
# Linking
# ------------------------------------------------------------
target_link_libraries(${PROJECT_NAME}
  PRIVATE
    glfw
    glad
    stb_image
    fmod
)

# ------------------------------------------------------------
# Include Directories
# ------------------------------------------------------------
target_include_directories(${PROJECT_NAME}
  PRIVATE
    include
    src
    extern/glad/include
    extern/glfw/include
    extern/glm
    extern/stb_image/include
    extern/fmod
)

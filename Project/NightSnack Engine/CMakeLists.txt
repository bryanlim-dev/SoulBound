#################################################
# CMake file for NightSnack Engine
# Team: Midnight Munchies
# Module: GAM_200
#################################################

# Minimum version of CMake required
cmake_minimum_required(VERSION 3.5)

# Use static MSVC runtime (/MT or /MTd)
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Project Setup
project(NightSnack LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Gather Source and Header Files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS src/*.h)
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Visual Studio virtual folder hierarchy
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Header Files" FILES ${HEADERS})

# Assets
target_compile_definitions(${PROJECT_NAME} PUBLIC RESOURCE_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/assets/\")
# target_compile_definitions(${PROJECT_NAME} PUBLIC RESOURCE_PATH=\"./assets/\") # For portability

# Extern
add_subdirectory(extern/glfw EXCLUDE_FROM_ALL)

add_library(glad extern/glad/src/glad.c)
target_include_directories(glad PUBLIC extern/glad/include)

add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE extern/stb_image/include)

# Link Libraries
target_link_libraries(${PROJECT_NAME}
  PRIVATE
    glfw
    glad
    stb_image
)

# Include directories
target_include_directories(${PROJECT_NAME}
  PRIVATE
    include
    src
    extern/glad/include
    extern/glfw/include
    extern/glm
    extern/stb_image/include
)

# Organize external libraries in Visual Studio
source_group("extern/glfw" FILES ${GLFW_SOURCES})
source_group("extern/glad" FILES ${GLAD_SOURCES})

set(STB_IMAGE_HEADERS extern/stb_image/include/stb_image.h)
source_group("extern/stb_image" FILES ${STB_IMAGE_HEADERS})
target_sources(${PROJECT_NAME} PRIVATE ${STB_IMAGE_HEADERS})
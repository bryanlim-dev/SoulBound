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
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use static MSVC runtime (/MT or /MTd)
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

if (MSVC)
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT NightSnack)
endif()

# ------------------------------------------------------------
# Gather Source and Header Files
# ------------------------------------------------------------
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE ENGINE_HEADERS CONFIGURE_DEPENDS src/*.h)

add_executable(${PROJECT_NAME} ${ENGINE_SOURCES} ${ENGINE_HEADERS})

# Set warning level for project
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

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

# Copy to build/assets
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "${CMAKE_BINARY_DIR}/assets"

# fmod debug ver dll
   COMMAND ${CMAKE_COMMAND} -E copy_if_different
     "${CMAKE_CURRENT_SOURCE_DIR}/extern/fmod/lib/fmodL.dll"
     "$<TARGET_FILE_DIR:${PROJECT_NAME}>"

# fmod release ver dll
   COMMAND ${CMAKE_COMMAND} -E copy_if_different
     "${CMAKE_CURRENT_SOURCE_DIR}/extern/fmod/lib/fmod.dll"
     "$<TARGET_FILE_DIR:${PROJECT_NAME}>"

#freetype dll
   COMMAND ${CMAKE_COMMAND} -E copy_if_different
     "${CMAKE_CURRENT_SOURCE_DIR}/extern/freetype/freetype.dll"
     "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
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
target_include_directories(stb_image SYSTEM INTERFACE extern/stb_image/include)

# fmod
add_library(fmod INTERFACE)
target_include_directories(fmod SYSTEM INTERFACE extern/fmod/inc)
# diff fmod links based on config
target_link_libraries(fmod INTERFACE
    debug "${CMAKE_CURRENT_SOURCE_DIR}/extern/fmod/lib/fmodL_vc.lib"
    optimized "${CMAKE_CURRENT_SOURCE_DIR}/extern/fmod/lib/fmod_vc.lib"
)

# nlohmann_json
add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json SYSTEM INTERFACE extern/nlohmann)

# IMGui
set(IMGUI_SRC
    extern/imgui/imgui.cpp
    extern/imgui/imgui_draw.cpp
    extern/imgui/imgui_tables.cpp
    extern/imgui/imgui_widgets.cpp
    extern/imgui/backends/imgui_impl_glfw.cpp
    extern/imgui/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SRC})
target_include_directories(imgui PUBLIC
    extern/imgui
    extern/imgui/backends
    extern/glfw/include
)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui)

#freetype
add_library(freetype INTERFACE)
target_include_directories(freetype SYSTEM INTERFACE extern/freetype)
target_link_libraries(freetype INTERFACE 
	"${CMAKE_CURRENT_SOURCE_DIR}/extern/freetype/freetype.lib"
)

# Hide library folders in Solution Explorer
set_target_properties(glad PROPERTIES FOLDER "ThirdParty")
set_target_properties(glfw PROPERTIES FOLDER "ThirdParty")
set_target_properties(stb_image PROPERTIES FOLDER "ThirdParty")
set_target_properties(fmod PROPERTIES FOLDER "ThirdParty")
set_target_properties(nlohmann_json PROPERTIES FOLDER "ThirdParty")
set_target_properties(imgui PROPERTIES FOLDER "ThirdParty")
set_target_properties(freetype PROPERTIES FOLDER "ThirdParty")

target_link_options(${PROJECT_NAME} PRIVATE /NODEFAULTLIB:LIBCMT)

# ------------------------------------------------------------
# Linking
# ------------------------------------------------------------
target_link_libraries(${PROJECT_NAME}
  PRIVATE
    glfw
    glad
    stb_image
    fmod
    nlohmann_json
    imgui
    freetype
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
    extern/imgui
    extern/imgui/backends
    extern/freetype
	extern/nlohmann
)

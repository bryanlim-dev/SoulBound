# ------------------------------------------------------------ #
# Third-Party Dependencies
# ------------------------------------------------------------ #

# GLFW
add_subdirectory(glfw EXCLUDE_FROM_ALL)
set_target_properties(glfw PROPERTIES FOLDER "ThirdParty")

# Glad
add_library(glad glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)
set_target_properties(glad PROPERTIES FOLDER "ThirdParty")

# stb_image (header-only)
add_library(stb_image INTERFACE)
target_include_directories(stb_image SYSTEM INTERFACE stb_image/include)

# GLM (header-only)
add_library(glm INTERFACE)
target_include_directories(glm SYSTEM INTERFACE glm)

# fmod
add_library(fmod INTERFACE)
target_include_directories(fmod SYSTEM INTERFACE fmod/inc)
# Different fmod links based on config
target_link_libraries(fmod INTERFACE
    debug "${CMAKE_CURRENT_SOURCE_DIR}/fmod/lib/fmodL_vc.lib"
    optimized "${CMAKE_CURRENT_SOURCE_DIR}/fmod/lib/fmod_vc.lib"
)

# nlohmann_json (header-only)
add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json SYSTEM INTERFACE nlohmann)

# IMGui
set(IMGUI_SRC
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SRC})
target_include_directories(imgui PUBLIC
    imgui
    imgui/backends
    glfw/include
)
set_target_properties(imgui PROPERTIES FOLDER "ThirdParty")

# freetype
add_library(freetype INTERFACE)
target_include_directories(freetype SYSTEM INTERFACE freetype)
target_link_libraries(freetype INTERFACE 
    "${CMAKE_CURRENT_SOURCE_DIR}/freetype/freetype.lib"
)

# Create an interface library that bundles all common dependencies
add_library(EngineThirdParty INTERFACE)
target_link_libraries(EngineThirdParty INTERFACE
    glfw
    glad
    stb_image
    glm
    fmod
    nlohmann_json
    imgui
    freetype
)

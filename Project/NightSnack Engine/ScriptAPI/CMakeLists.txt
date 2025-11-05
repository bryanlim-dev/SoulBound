# Enable C++/CLI support
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /clr")
set(CMAKE_CXX_STANDARD 20)

# Source files
set(SCRIPTAPI_SOURCES
    src/ScriptAPIWrapper.cpp
)

set(SCRIPTAPI_HEADERS
    include/ScriptAPIWrapper.h
)

# Create the C++/CLI DLL
add_library(ScriptAPI SHARED
    ${SCRIPTAPI_SOURCES}
    ${SCRIPTAPI_HEADERS}
)

# Link against Core engine
target_link_libraries(ScriptAPI
    PUBLIC Core
)

# Include directories
target_include_directories(ScriptAPI
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# C++/CLI specific settings
target_compile_options(ScriptAPI PRIVATE
    /clr
    /EHa  # Enable C++ exceptions with SEH
)

# Disable precompiled headers for C++/CLI (they can cause issues)
set_target_properties(ScriptAPI PROPERTIES
    FOLDER "Scripting"
    COMMON_LANGUAGE_RUNTIME "netcore"  # Use .NET Core/5+
    DOTNET_TARGET_FRAMEWORK "net8.0"
    VS_GLOBAL_CLRSupport "NetCore"
)

# Define export macro
target_compile_definitions(ScriptAPI PRIVATE
    SCRIPTAPI_EXPORTS
)

# Remove /RTC1 flag which is incompatible with /clr
string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
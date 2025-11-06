# Enable C++/CLI support
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /clr")
set(CMAKE_CXX_STANDARD 20)

# Source files
file(GLOB_RECURSE SCRIPTAPI_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
file(GLOB_RECURSE SCRIPTAPI_HEADERS CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
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

set_target_properties(ScriptAPI PROPERTIES
    FOLDER "Scripting"
    COMMON_LANGUAGE_RUNTIME ""  # Let VS handle it
    VS_DOTNET_REFERENCES "System;System.Core"
)

# Define export macro
target_compile_definitions(ScriptAPI PRIVATE
    SCRIPTAPI_EXPORTS
)

# Organize files in Visual Studio Solution Explorer
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" 
    FILES ${SCRIPTAPI_SOURCES} ${SCRIPTAPI_HEADERS}
)

# Remove incompatible flags
string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
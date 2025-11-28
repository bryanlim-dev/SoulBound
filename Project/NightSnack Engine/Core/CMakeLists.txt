# ------------------------------------------------------------ #
# Core Engine DLL
# ------------------------------------------------------------ #
project(Core)

# Gather engine source and header files
file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
file(GLOB_RECURSE CORE_HEADERS CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

# Create the DLL
add_library(Core SHARED
    ${CORE_SOURCES}
    ${CORE_HEADERS}
)

# Set warning level
if(MSVC)
    target_compile_options(Core PRIVATE /W4 /permissive-)
else()
    target_compile_options(Core PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(MSVC)
    target_link_options(Core PRIVATE /NODEFAULTLIB:LIBCMT)
endif()

# Define export macro for DLL with relative paths from executable location
target_compile_definitions(Core 
    PRIVATE
        CORE_EXPORTS
    PUBLIC
        RESOURCE_PATH=\"./assets/\"
        SCRIPTS_PATH=\"./assets/Scripts/\"
        PREFABS_PATH=\"./assets/GameData/Prefabs/\"
        LEVELS_PATH=\"./assets/GameData/Levels/\"
        DATA_PATH=\"./assets/Data/\"
        PRIME_RESOURCE_PATH=\"../../../assets/\"
	    PRIME_SCRIPTS_PATH=\"../../../ManagedScripts/\"
        PRIME_PREFABS_PATH=\"../../../assets/GameData/Prefabs/\"
        PRIME_LEVELS_PATH=\"../../../assets/GameData/Levels/\"
)

# Include directories
target_include_directories(Core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link against third-party dependencies
target_link_libraries(Core PUBLIC EngineThirdParty)

# Organize files in Visual Studio Solution Explorer
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" 
    FILES ${CORE_SOURCES} ${CORE_HEADERS}
)

# Optional: Precompiled headers (if planning to have pch.h in future)
# target_precompile_headers(Core PRIVATE src/pch.h)
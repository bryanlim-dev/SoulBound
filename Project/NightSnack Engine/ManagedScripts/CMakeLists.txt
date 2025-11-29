# ------------------------------------------------------------ #
# ManagedScripts - C# Class Library
# ------------------------------------------------------------ #
project(ManagedScripts LANGUAGES CSharp)

enable_language(CSharp)

# Source files
file(GLOB_RECURSE MANAGED_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cs"
)

# Create C# class library
add_library(ManagedScripts SHARED
    ${MANAGED_SOURCES}
)

# Set C# project properties - Target .NET Framework 4.5 for Mono compatibility
set_target_properties(ManagedScripts PROPERTIES
    FOLDER "Scripting"
    DOTNET_TARGET_FRAMEWORK "net45"  # .NET Framework 4.5
    DOTNET_SDK "Microsoft.NET.Sdk"
    VS_GLOBAL_ROOTNAMESPACE "ManagedScripts"
    VS_DOTNET_REFERENCES "System;System.Core"
    DOTNET_BUILD_ALWAYS true
)

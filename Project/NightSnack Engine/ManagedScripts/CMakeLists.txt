# Enable C# language support
enable_language(CSharp)

# Source files
set(MANAGED_SOURCES
    Script.cs
    ScriptManager.cs
)

# Create C# class library
add_library(ManagedScripts SHARED
    ${MANAGED_SOURCES}
)

# Set C# project properties
set_target_properties(ManagedScripts PROPERTIES
    FOLDER "Scripting"
    DOTNET_TARGET_FRAMEWORK "net8.0"
    DOTNET_SDK "Microsoft.NET.Sdk"
    VS_GLOBAL_ROOTNAMESPACE "ManagedScripts"
    VS_DOTNET_REFERENCES "System;System.Core"
)

# Reference ScriptAPI
set_property(TARGET ManagedScripts PROPERTY VS_DOTNET_REFERENCE_ScriptAPI
    "${CMAKE_BINARY_DIR}/bin/ScriptAPI.dll"
)

# Add dependency to ensure ScriptAPI builds first
add_dependencies(ManagedScripts ScriptAPI)

# Copy ScriptAPI DLL to ManagedScripts output for reference resolution
add_custom_command(TARGET ManagedScripts PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:ScriptAPI>"
        "${CMAKE_CURRENT_BINARY_DIR}/"
    COMMENT "Copying ScriptAPI.dll for reference resolution"
)
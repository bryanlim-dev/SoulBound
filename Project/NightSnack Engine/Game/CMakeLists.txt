# ------------------------------------------------------------ #
# Game Executable
# ------------------------------------------------------------ #
project(Game)

# Gather game-specific source files
set(GAME_SOURCES
    src/main.cpp
)

# Create executable (removed WIN32 to use console entry point)
add_executable(Game ${GAME_SOURCES})

# Set warning level and fix runtime library conflicts
if(MSVC)
    target_compile_options(Game PRIVATE /W4 /permissive-)
else()
    target_compile_options(Game PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Link against Core engine
target_link_libraries(Game PRIVATE Core)

# Set output name and Visual Studio debugging directory
set_target_properties(Game PROPERTIES
    OUTPUT_NAME "SoulBound"
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Game>"
)

# ------------------------------------------------------------
# Assets and DLL Handling
# ------------------------------------------------------------
# Gather all asset files (for dependency tracking)
file(GLOB_RECURSE ASSET_FILES "${CMAKE_SOURCE_DIR}/assets/*")

# Define output directory variable for cleaner commands
set(GAME_OUTPUT_DIR "$<TARGET_FILE_DIR:Game>")

# Copy assets and DLLs
add_custom_target(copy_assets ALL
    # Copy assets folder to output directory
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "${GAME_OUTPUT_DIR}/assets"

    # Copy correct FMOD dll depending on build type
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/extern/fmod/lib/fmodL.dll>"
        "$<$<CONFIG:Release>:${CMAKE_SOURCE_DIR}/extern/fmod/lib/fmod.dll>"
        "${GAME_OUTPUT_DIR}"

    # Copy freetype dll
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/extern/freetype/freetype.dll"
        "${GAME_OUTPUT_DIR}"

    # Copy mono dll
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/extern/mono/lib/mono-2.0-sgen.dll"
        "${GAME_OUTPUT_DIR}"

    DEPENDS ${ASSET_FILES}
    COMMENT "Copying assets and DLLs to SoulBound folder..."
)

# Copy Mono lib directory
add_custom_command(TARGET Game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/extern/mono/lib"
        "${GAME_OUTPUT_DIR}/lib/"
    COMMENT "Copying Mono libraries..."
)

# Copy Core.dll to Game output directory
add_custom_command(TARGET Game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:Core>"
        "${GAME_OUTPUT_DIR}"
    COMMENT "Copying Core.dll..."
)

# Copy ManagedScripts.dll to assets/Scripts folder
add_custom_command(TARGET Game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "${GAME_OUTPUT_DIR}/assets/Scripts"
    COMMAND ${CMAKE_COMMAND} -E copy
        "$<TARGET_FILE:ManagedScripts>"
        "${GAME_OUTPUT_DIR}/assets/Scripts/"
    COMMENT "Copying ManagedScripts.dll to assets/Scripts..."
)

# Ensure proper build order
add_dependencies(Game 
    copy_assets 
    Core 
    ManagedScripts
)

# Add install rules (optional, for creating distributable packages)
install(TARGETS Game
    RUNTIME DESTINATION .
)

install(DIRECTORY "${CMAKE_SOURCE_DIR}/assets/"
    DESTINATION assets
)

install(FILES 
    "$<TARGET_FILE:Core>"
    "${CMAKE_SOURCE_DIR}/extern/fmod/lib/fmod.dll"
    "${CMAKE_SOURCE_DIR}/extern/freetype/freetype.dll"
    "${CMAKE_SOURCE_DIR}/extern/mono/lib/mono-2.0-sgen.dll"
    DESTINATION .
)

install(FILES "$<TARGET_FILE:ManagedScripts>"
    DESTINATION assets/Scripts
)

install(DIRECTORY "${CMAKE_SOURCE_DIR}/extern/mono/lib/"
    DESTINATION lib
)
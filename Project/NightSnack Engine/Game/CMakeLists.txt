# ------------------------------------------------------------ #
# Game Executable
# ------------------------------------------------------------ #
project(Game)

# Gather game-specific source files (if separate from engine)
# If your game code is mixed with engine, just create main.cpp
set(GAME_SOURCES
    src/main.cpp
)

# Create executable
add_executable(Game ${GAME_SOURCES})

# Set warning level
if(MSVC)
    target_compile_options(Game PRIVATE /W4 /permissive-)
else()
    target_compile_options(Game PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Link against Core engine
target_link_libraries(Game PRIVATE Core)

# ------------------------------------------------------------
# Assets Handling
# ------------------------------------------------------------
# Gather all asset files (for dependency tracking)
file(GLOB_RECURSE ASSET_FILES "${CMAKE_SOURCE_DIR}/assets/*")

# Copy assets and DLLs
add_custom_target(copy_assets ALL
    # Mirror assets folder to build output (only new files are copied)
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:Game>/assets"

    # Mirror to build/assets (for convenience)
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_SOURCE_DIR}/assets"
        "${CMAKE_BINARY_DIR}/assets"

    # Copy fmod debug dll
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/extern/fmod/lib/fmodL.dll"
        "$<TARGET_FILE_DIR:Game>"

    # Copy fmod release dll
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/extern/fmod/lib/fmod.dll"
        "$<TARGET_FILE_DIR:Game>"

    # Copy freetype dll
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/extern/freetype/freetype.dll"
        "$<TARGET_FILE_DIR:Game>"

    DEPENDS ${ASSET_FILES}
    COMMENT "Copying assets and DLLs..."
)

# Copy Core.dll to Game output directory
add_custom_command(TARGET Game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:Core>"
        "$<TARGET_FILE_DIR:Game>"
    COMMENT "Copying Core.dll to game directory"
)

# # Copy ScriptAPI.dll if scripting is enabled
# add_custom_command(TARGET Game POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         "$<TARGET_FILE:ScriptAPI>"
#         "$<TARGET_FILE_DIR:Game>"
#     COMMENT "Copying ScriptAPI.dll to game directory"
# )

# # Copy ManagedScripts.dll if scripting is enabled
# add_custom_command(TARGET Game POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         "$<TARGET_FILE:ManagedScripts>"
#         "$<TARGET_FILE_DIR:Game>"
#     COMMENT "Copying ManagedScripts.dll to game directory"
# )

# Make sure assets are copied before building/running
add_dependencies(Game copy_assets Core)
#add_dependencies(Game copy_assets Core ScriptAPI ManagedScripts)
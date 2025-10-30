# ------------------------------------------------------------ #
# Readme file for NightSnack Engine
# Team: Midnight Munchies
# Module: GAM_200
# ------------------------------------------------------------ #

## Input Controls
## --------------------------
F: Apply force to Red ball to test collision (Dynamic -> Dynamic | Dynamic -> Static).
T: Activate debug-drawing.

A: Shrink entity with Target texture
D: Enlarge entity with Target texture
R: Rotate entity with Target texture

H: Play sound effect

TAB: Enable/Disable printing System % time consumption



## Player Controls
## --------------------------
Left-Arrow: Move Blue ball left
Right-Arrow: Move Blue ball right
Up-Arrow: "Jump" with Blue ball



## Cloning/Creating from Archetypes
## --------------------------
1: Create Blue square
2: Create Pink oval
3: Create Circle with banana texture



## Stress testing
## --------------------------
Spacebar: Double number of living entities (Creates entities with random transforms)
Backspace: Half number of living entities (Destroys entities except default scene)



## Force Crash
## --------------------------
C: Force a crash by setting GraphicsSystem shader to a nullptr

Crashlogs can be found in "./build/CrashLog/crash_log.txt"



## Step by Step Physics debug
## --------------------------
Ctrl P: Enable/Disable Step by Step Module
P: When enabled, Step to next frame

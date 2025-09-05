## ⚙️NightSnack Engine
## --------------------------
To ensure a clean repository, build artifacts, logs and user settings will be ignored
during git commits.
During first set-up, there will not be a solution file. (Do not panic)


## 📂 Repository Structure
## --------------------------

Project/
├── .git/
├── .gitignore
└── NightSnack Engine/
    │
    ├── src/ 			# Engine source files (.cpp .h)
    ├── extern/		# External libraries (GLFW, GLAD, GLM, IMGUI, etc.)
    ├── assets/ 		# Game assets (textures, sounds, etc.) - not yet populated
    ├── CMakeLists.txt 	# Main CMake build script
    └── README.md 		# This file


## 🚀 Build Instructions
## --------------------------
1. Create a new folder "build" inside "NightSnack Engine/". (same level as this README file)
2. Open CMD inside "build" folder.
3. Type "cmake .." (Do this in plain CMD, not WSL)

You should now be able to find "NightSnack.sln" inside the "build" folder.
Please direct message me if you run into any issues trying to set up the solution.

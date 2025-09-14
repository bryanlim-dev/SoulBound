#include "Core.h"
#include "Systems/AllSystems.h"

// ------------------------------------------------------------------------------------------------
// CORE
// ------------------------------------------------------------------------------------------------

void CoreEngine::Initialize() {

	// TEMP; By right this will be handled by Graphics system and will be removed;
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	// Initialize ECS Coordinator and Managers;
	Coord.Initialize();

	// Register all Components;
	Coord.RegisterComponent<Position>();
	Coord.RegisterComponent<Velocity>();

	// Add more Components here...

}

// Function should be called upon termination of program;
// Return 0 if successful, else return exception code in case of error or exception thrown;
int CoreEngine::Terminate() {

	// Empty for now...

	return 0;
}

void CoreEngine::Update() {

	// Clear the screen; By right this will be handled by Graphics system and will be removed;
	glClear(GL_COLOR_BUFFER_BIT);

	// Obtain dt; TEMP, WILL BE REPLACED WITH TIMER CLASS;
	double deltaTime = 0.2;

	// Iterate through Systems and Update them through Coordinator;
	for (auto& System : Systems) {
		System->Update(deltaTime, Coord);
	}

	// Application Window	Update;
	CoreWindow->PollEvents();
	CoreWindow->SwapBuffers();

}


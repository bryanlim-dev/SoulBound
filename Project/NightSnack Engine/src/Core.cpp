#include "Core.h"
#include "Systems/AllSystems.h"

// ------------------------------------------------------------------------------------------------
// CORE
// ------------------------------------------------------------------------------------------------

void CoreEngine::Initialize() {

	// TEMP; By right this will be handled by Graphics system and will be removed;
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	// Initialize ECS Coordinator and Managers;
	m_coord.Initialize();

	// Register all Components;
	m_coord.RegisterComponent<Position>();
	m_coord.RegisterComponent<Rotation>();
	m_coord.RegisterComponent<Scale>();
	m_coord.RegisterComponent<Color>();
	m_coord.RegisterComponent<Velocity>();
	m_coord.RegisterComponent<Renderable>();
	m_coord.RegisterComponent<Camera>();

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
	double deltaTime = 0.02;

	// Iterate through Systems and Update them through Coordinator;
	for (auto& System : m_systems) {
		System->Update(deltaTime, m_coord);
	}

	// Application Window	Update;
	m_coreWindow->PollEvents();
	m_coreWindow->SwapBuffers();

}


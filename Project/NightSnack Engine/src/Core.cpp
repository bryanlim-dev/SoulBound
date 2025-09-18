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

	m_coord.RegisterComponent<Velocity>();
	m_coord.RegisterComponent<Collider>();

	m_coord.RegisterComponent<Renderable>();
	m_coord.RegisterComponent<Color>();
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
	// Start frame timing;
	timer_.StartFrame();

	// Clear the screen; By right this will be handled by Graphics system and will be removed;
	glClear(GL_COLOR_BUFFER_BIT);

	// Obtain dt from Timer;
	double deltaTime = timer_.GetDT();

	// Iterate through Systems and Update them through Coordinator;
	for (auto& System : m_systems) {
		timer_.StartSystemTimer(System->GetName());
		System->Update(deltaTime, m_coord);
		timer_.EndSystemTimer(System->GetName());
	}

	// Update window title with FPS from Timer;
	std::string title = "NightSnack Engine | FPS: " + std::to_string(static_cast<int>(timer_.GetFPS())) 
		+ " | Game Time: " + std::to_string(static_cast<int>(timer_.GetTotalSystemTime()));
	m_coreWindow->SetTitle(title);

	// Displaying active system timers into Console (temp.);
	//timer_.DisplaySystemTimers();

	// Application Window	Update;
	m_coreWindow->PollEvents();
	m_coreWindow->SwapBuffers();

	// End frame timing;
	timer_.EndFrame();
	//timer_.FrameCap(60.f); // Cap frame rate to 60 FPS (optional);
}


#include "Core.h"
#include "Systems/AllSystems.h"

// ------------------------------------------------------------------------------------------------
// CORE
// ------------------------------------------------------------------------------------------------

void CoreEngine::Initialize(int windowWidth, int windowHeight, std::string&& windowTitle) {

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

	// Add the systems you want to the Core engine, Must be in specific order taught in GIT lessons;
	// i.e: Input-> Movement -> Collision -> Graphics;
	AddSystem<ApplicationSystem>();
	AddSystem<InputSystem>();
	AddSystem<MovementSystem>();
	AddSystem<GraphicsSystem>();

	// Add more Components here...

	// Initialise ApplicationSystem
	m_coord.GetSystem<ApplicationSystem>().get()->Init(windowWidth, windowHeight, std::move(windowTitle));

	// Initialise InputSystem
	m_coord.GetSystem<InputSystem>().get()->Init(
		m_coord.GetSystem<ApplicationSystem>().get()->GetNativeHandle()); // GLFWwindow* handle

	// Call this only after the OpenGL context has been created, which is after ApplicationSystem::Init()
	// TEMP; By right this will be handled by Graphics system and will be removed;
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

// Function should be called upon termination of program;
// Return 0 if successful, else return exception code in case of error or exception thrown;
int CoreEngine::Terminate() {

	// Empty for now...

	return 0;
}

void CoreEngine::Update() {

	// Reset input states at the start of the frame
	m_coord.GetSystem<InputSystem>()->ResetInputStates();

	// Obtain dt; TEMP, WILL BE REPLACED WITH TIMER CLASS;
	double deltaTime = 0.02;

	// glfw callbacks are triggered here
	m_coord.GetSystem<ApplicationSystem>().get()->PollEvents();

	// Iterate through Systems and Update them through Coordinator;
	for (auto& System : m_systems) {
		System->Update(deltaTime, m_coord);
	}

	m_coord.GetSystem<ApplicationSystem>().get()->SwapBuffers();
}


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
	m_coord.RegisterComponent<Velocity>();
	
	m_coord.RegisterComponent<Acceleration>();	// Physics;
	m_coord.RegisterComponent<Force>();
	m_coord.RegisterComponent<Mass>();

	m_coord.RegisterComponent<Collider>();		// Collision;

	m_coord.RegisterComponent<Renderable>();	// Graphics;
	m_coord.RegisterComponent<Color>();
	m_coord.RegisterComponent<Scale>();
	m_coord.RegisterComponent<Camera>();

	// Add the systems you want to the Core engine, Must be in specific order taught in GIT lessons;
	// i.e: Input-> Movement -> Collision -> Graphics;
	AddSystem<ApplicationSystem>();
	AddSystem<InputSystem>();
	AddSystem<MovementSystem>();
	AddSystem<CollisionSystem>();
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
	// Start frame timing;
	m_coord.GetSystem<ApplicationSystem>()->GetTimer().StartFrame();

	// Reset input states at the start of the frame
	m_coord.GetSystem<InputSystem>()->ResetInputStates();

	// Obtain dt from Timer;
	double deltaTime = m_coord.GetSystem<ApplicationSystem>()->GetTimer().GetDT();

	// glfw callbacks are triggered here
	m_coord.GetSystem<ApplicationSystem>().get()->PollEvents();

	// Iterate through Systems and Update them through Coordinator;
	for (auto& System : m_systems) {

		static double dt = 0;
		dt += deltaTime;
		if (dt > 2.0) {
			std::cout << "deltaTime: " << deltaTime << "\n";
			dt = 0;
		}
		m_coord.GetSystem<ApplicationSystem>()->GetTimer().StartSystemTimer(System->GetName());
		System->Update(deltaTime, m_coord);
		m_coord.GetSystem<ApplicationSystem>()->GetTimer().EndSystemTimer(System->GetName());
	}

	m_coord.GetSystem<ApplicationSystem>().get()->SwapBuffers();

	// Update window title with FPS from Timer;
	std::string title = "NightSnack Engine | FPS: " + std::to_string(static_cast<int>(m_coord.GetSystem<ApplicationSystem>()->GetTimer().GetFPS()))
		+ " | Game Time: " + std::to_string(static_cast<int>(m_coord.GetSystem<ApplicationSystem>()->GetTimer().GetTotalSystemTime()));
	
	m_coord.GetSystem<ApplicationSystem>().get()->SetWindowTitle(
		m_coord.GetSystem<ApplicationSystem>().get()->GetNativeHandle(), title.c_str()
	);

	// Displaying active system timers into Console (temp.);
	//m_coord.GetSystem<ApplicationSystem>()->GetTimer().DisplaySystemTimers();

	// End frame timing;
	m_coord.GetSystem<ApplicationSystem>()->GetTimer().EndFrame();
	//timer_.FrameCap(60.f); // Cap frame rate to 60 FPS (optional);
}


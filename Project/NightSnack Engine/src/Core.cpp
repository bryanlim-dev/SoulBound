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

	m_coord.RegisterComponent<Color>();
	m_coord.RegisterComponent<Scale>();
	m_coord.RegisterComponent<Camera>();

	m_coord.RegisterComponent<PlayerController>();
	m_coord.RegisterComponent<Renderable>();	// Graphics;

	// Add the systems you want to the Core engine, Must be in specific order taught in GIT lessons;
	// i.e: Input-> Movement -> Collision -> Graphics;
	AddSystem<ApplicationSystem>();
	AddSystem<InputSystem>();
	AddSystem<EntityFactory>(m_coord, core_dispatcher, "assets/GameData/Archetypes/Archetypes.json");
	AddSystem<PlayerControllerSystem>(m_coord, core_dispatcher);
	AddSystem<PhysicsSystem>(m_coord, core_dispatcher);
	AddSystem<CollisionSystem>();
	AddSystem<MovementSystem>(m_coord, core_dispatcher);
	AddSystem<GraphicsSystem>(m_coord);

	// Add more Components here...

	// Initialise ApplicationSystem
	m_coord.GetSystem<ApplicationSystem>()->Init(windowWidth, windowHeight, std::move(windowTitle));

	// Initialise GraphicsSystem
	m_coord.GetSystem<GraphicsSystem>()->Init(m_coord, core_dispatcher);
	
	// Initialise InputSystem
	m_coord.GetSystem<InputSystem>()->Init(m_coord.GetSystem<ApplicationSystem>()->GetNativeHandle(), core_dispatcher); // GLFWwindow* handle

	// Initialise CollisionSystem
	m_coord.GetSystem<CollisionSystem>()->Init(core_dispatcher);

	

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

	// Obtain dt from Timer;
	float deltaTime = m_coord.GetSystem<ApplicationSystem>()->GetTimer().GetDT();

	// Clamp dt to prevent giant steps when window is dragged / unfocused
	if (deltaTime > 0.02f)
		deltaTime = 0.02f;

	// If paused, skip updating systems that rely on dt
	if (m_paused) {
		// Reset timer so dt doesn't accumulate during pause
		deltaTime = 0.0f;		
		return;
	}

	// Iterate through Systems and Update them through Coordinator;
	for (auto& System : m_systems) {

		m_coord.GetSystem<ApplicationSystem>()->GetTimer().StartSystemTimer(System->GetName());

		System->Update(deltaTime, m_coord);

		m_coord.GetSystem<ApplicationSystem>()->GetTimer().EndSystemTimer(System->GetName());
	}

	for (Entity e : m_coord.GetSystem<EntityFactory>()->m_toBeDestroyed) { 
		m_coord.DestroyEntity(e);
	}
	m_coord.GetSystem<EntityFactory>()->m_toBeDestroyed.clear();

	m_coord.GetSystem<ApplicationSystem>()->GetTimer().FrameCap(102.f); // Cap frame rate to 60 FPS (optional);

	m_coord.GetSystem<ApplicationSystem>()->GetTimer().StartSystemTimer(m_coord.GetSystem<ApplicationSystem>()->GetName());
	m_coord.GetSystem<ApplicationSystem>()->SwapBuffers();
	m_coord.GetSystem<ApplicationSystem>()->GetTimer().EndSystemTimer(m_coord.GetSystem<ApplicationSystem>()->GetName());

	// End frame timing;
	m_coord.GetSystem<ApplicationSystem>()->GetTimer().EndFrame();
}


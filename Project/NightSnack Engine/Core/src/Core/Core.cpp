/******************************************************************************
 * File:        Core.cpp
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan (Primary)
 *				LIM Wei Jie (Secondary, 17%) - Tracking of game loop timer 
 *				and system timers, and capping of frame rate
 *				Bryan Lim Jun Jie (Secondary, 5%) - pause delta time when game paused
 *
 * Description:
 * Defines member functions of CoreEngine class.
 *
 * Copyright � 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "Core.h"
#include "Systems/AllSystems.h"

#include "ResourceManager/ResourceHelper/ResourceSystemHelper.h"
#include "../Systems/StateMachine/FiniteStateMachine.h"

namespace NightSnack {

	static float accumulatedTime = 0.0f;
	static int numberOfSteps = 0;

	// ------------------------------------------------------------------------------------------------
	// CORE
	// ------------------------------------------------------------------------------------------------
	void CoreEngine::StartUp(std::string const& configFilePath) {
		try {
			Initialize(configFilePath);
		}
		catch (const NightSnack::IException ex)
		{
			NightSnack::CrashLogger::LogCrash(ex);
			ex.print();
			exit(EXIT_FAILURE);
		}
		catch (const std::exception& ex)
		{
			NightSnack::CrashLogger::LogCrash(NightSnack::IException(ex.what(), NightSnack::ExceptionType::General, __FILE__));
			std::cerr << "[Exception][std::exception] " << ex.what() << std::endl;
			exit(EXIT_FAILURE);
		}
		catch (...)
		{
			NightSnack::CrashLogger::LogCrash(NightSnack::IException("Unknown exception", NightSnack::ExceptionType::General, __FILE__));
			std::cerr << "[Exception][Unknown]" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	void CoreEngine::Initialize(std::string const& configFilePath) {

		// Initialize ECS Coordinator and Managers;
		m_coord.Initialize();

		// Register all Components;
		m_coord.RegisterComponent<Transform>();
		m_coord.RegisterComponent<RigidBody>();		// Physics;
		m_coord.RegisterComponent<Collider>();		// Collision;

		m_coord.RegisterComponent<Renderable>();	// Graphics;
		m_coord.RegisterComponent<Color>();
		m_coord.RegisterComponent<Camera>();

		m_coord.RegisterComponent<PlayerController>();
		m_coord.RegisterComponent<States>();

		m_coord.RegisterComponent<Animator>();
		m_coord.RegisterComponent<Font>();
		m_coord.RegisterComponent<ButtonInfo>();
		m_coord.RegisterComponent<Script>();

		// Add more Components here...

		// Add the systems you want to the Core engine, Must be in specific order taught in GIT lessons
		// i.e: Input-> Movement -> Collision -> Graphics;
		AddSystem<ResourceManager>(); //RM first, so app can load values from it
		std::string windowTitle = ""; int winWidth = 0, winHeight = 0;
		//Get window data
		ResourceSystemHelper::GetResourceSystemWindowData(m_coord.GetSystem<ResourceManager>(), configFilePath, winHeight, winWidth, windowTitle);

		AddSystem<ApplicationSystem>(std::move(windowTitle), m_coord, core_dispatcher);
		AddSystem<InputSystem>(m_coord, core_dispatcher);

		AddSystem<AudioSystem>(m_coord, core_dispatcher);

		AddSystem<GameUI>(m_coord, core_dispatcher);

		AddSystem<EntityFactory>(m_coord, core_dispatcher, "../assets/GameData/Archetypes/Archetypes.json","../assets/GameData/Archetypes/AIStatesArchetypes.json", "../assets/GameData/DefaultLevel.json");

		AddSystem<PlayerControllerSystem>(m_coord, core_dispatcher);

		AddSystem<CameraSystem>(m_coord, core_dispatcher);
		AddSystem<PhysicsSystem>(m_coord, core_dispatcher);
		AddSystem<MovementSystem>(m_coord, core_dispatcher);

		AddSystem<GraphicsSystem>(m_coord, core_dispatcher);
		AddSystem<AnimationSystem>(m_coord, core_dispatcher);
		AddSystem<TextSystem>(m_coord, core_dispatcher);
		AddSystem<ImguiSystem>(m_coord, core_dispatcher);

		AddSystem<FiniteStateMachine>(m_coord);
		AddSystem<Scripting::ScriptManager>(m_coord, core_dispatcher);


		// Buffer all entities' signatures to properly initialize all systems' set of entities
		for (Entity e : m_coord.GetLivingEntities()) {
			m_coord.BufferEntitySignature(e);
		}
	}

	// Function should be called upon termination of program;
	// Return 0 if successful, else return exception code in case of error or exception thrown;
	int CoreEngine::Terminate() {

		// Empty for now...

		return 0;
	}

	void CoreEngine::Run() {
		try {
			while (!m_coord.GetSystem<ApplicationSystem>()->ShouldClose()) {
				Update();
			}
		}
		catch (const NightSnack::IException ex)
		{
			NightSnack::CrashLogger::LogCrash(ex);
			ex.print();
			exit(EXIT_FAILURE);
		}
		catch (const std::exception& ex)
		{
			NightSnack::CrashLogger::LogCrash(NightSnack::IException(ex.what(), NightSnack::ExceptionType::General, __FILE__));
			std::cerr << "[Exception][std::exception] " << ex.what() << std::endl;
			exit(EXIT_FAILURE);
		}
		catch (...)
		{
			NightSnack::CrashLogger::LogCrash(NightSnack::IException("Unknown exception", NightSnack::ExceptionType::General, __FILE__));
			std::cerr << "[Exception][Unknown]" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	void CoreEngine::Update() {
		// Start frame timing;
		m_coord.GetSystem<ApplicationSystem>()->StartFrame();

		// Fixed DT with Accumulated time implementation;
		numberOfSteps = 0;	// reset step count per frame;
		float dt = m_coord.GetSystem<ApplicationSystem>()->GetDT();	// compute actual game loop time;
		accumulatedTime += dt;

		// Obtain number of steps this loop;
		while (accumulatedTime >= FixedDT) { 
			accumulatedTime -= FixedDT;
			++numberOfSteps;
		}
		if (m_paused) {
			numberOfSteps = 0;
			return;
		}
		if (numberOfSteps > 15) {
			numberOfSteps = 15;
		}

		// Iterate through Systems and Update them through Coordinator;
		for (auto& System : m_systems) {

			m_coord.GetSystem<ApplicationSystem>()->StartSystemTimer(System->GetName());
			System->Update(numberOfSteps);
			m_coord.GetSystem<ApplicationSystem>()->EndSystemTimer(System->GetName());
		}

		// Destroy all entities scheduled to be removed;
		m_coord.GetSystem<ApplicationSystem>()->StartSystemTimer("EntityFactory");
		m_coord.GetSystem<EntityFactory>()->PostUpdate();
		m_coord.GetSystem<ApplicationSystem>()->EndSystemTimer("EntityFactory");

		m_coord.GetSystem<ApplicationSystem>()->StartSystemTimer("ApplicationSystem");
		m_coord.GetSystem<ApplicationSystem>()->SwapBuffers();
		//m_coord.GetSystem<ApplicationSystem>()->GetTimer().FrameCap(62.0f);
		m_coord.GetSystem<ApplicationSystem>()->EndSystemTimer("ApplicationSystem");

		// End frame timing;
		m_coord.GetSystem<ApplicationSystem>()->EndFrame();
	}

}	// NightSnack

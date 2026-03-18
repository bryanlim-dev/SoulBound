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
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "Core.h"
#include "Systems/AllSystems.h"

#include "ResourceManager/ResourceHelper/ResourceSystemHelper.h"
#include "../Systems/StateMachine/FiniteStateMachine.h"

#include <chrono>

namespace NightSnack {

	constexpr float MAX_FRAME_TIME = 0.25f;
	using Clock = std::chrono::steady_clock;
	using Duration = std::chrono::duration<float>;

	static Clock::time_point lastTime = Clock::now();
	static float accumulatedTime = 0.0f;

	// ------------------------------------------------------------------------------------------------
	// CORE
	// ------------------------------------------------------------------------------------------------
	void CoreEngine::StartUp(std::string const& configFilePath) {
		try {
			Initialize(configFilePath);
		}
		catch (const NightSnack::IException ex)
		{
			NightSnack::CrashLogger::LogCrash(ex, "Core StartUp Failed");
			ex.print();
			exit(EXIT_FAILURE);
		}
		catch (const std::exception& ex)
		{
			NightSnack::CrashLogger::LogCrash(NightSnack::IException(ex.what(), NightSnack::ExceptionType::General, __FILE__), "Core StartUp Failed");
			std::cerr << "[Exception][std::exception] " << ex.what() << std::endl;
			exit(EXIT_FAILURE);
		}
		catch (...)
		{
			NightSnack::CrashLogger::LogCrash(NightSnack::IException("Unknown exception", NightSnack::ExceptionType::General, __FILE__), "Core StartUp Failed");
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
		m_coord.RegisterComponent<Audio>();
		m_coord.RegisterComponent<Camera>();

		m_coord.RegisterComponent<PlayerController>();
		//m_coord.RegisterComponent<States>();
		m_coord.RegisterComponent<Hierarchy>();

		m_coord.RegisterComponent<Animator>();
		m_coord.RegisterComponent<Particle>();
		m_coord.RegisterComponent<Collectible>();
		m_coord.RegisterComponent<Font>();
		m_coord.RegisterComponent<ButtonInfo>();
		m_coord.RegisterComponent<Script>();
		m_coord.RegisterComponent<Character>();

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


		AddSystem<PhysicsSystem>(m_coord, core_dispatcher);
		AddSystem<MovementSystem>(m_coord, core_dispatcher);
		AddSystem<GraphicsSystem>(m_coord, core_dispatcher);
		AddSystem<AnimationSystem>(m_coord, core_dispatcher);
		AddSystem<ParticleSystem>(m_coord, core_dispatcher);
		AddSystem<CollectibleSystem>(m_coord, core_dispatcher);

		AddSystem<CameraSystem>(m_coord, core_dispatcher);
		AddSystem<EntityFactory>(m_coord, core_dispatcher);
		AddSystem<PlayerControllerSystem>(m_coord, core_dispatcher);
		AddSystem<Scene::SceneManager>(m_coord, core_dispatcher);
		AddSystem<Scripting::ScriptManager>(m_coord, core_dispatcher);
		//AddSystem<FiniteStateMachine>(m_coord);
		AddSystem<CharacterSystem>(m_coord, core_dispatcher);
		AddSystem<DialogueSystem>(m_coord, core_dispatcher);
		AddSystem<TextSystem>(m_coord, core_dispatcher);

#ifdef EDITOR
		AddSystem<ImguiSystem>(m_coord, core_dispatcher);
#endif

		// Buffer all entities' signatures to properly initialize all systems' set of entities
		for (Entity e : m_coord.GetLivingEntities()) {
			m_coord.BufferEntitySignature(e);
		}

		// Set to 60 to disable frame cap
		m_coord.GetSystem<ApplicationSystem>()->SetFrameCap(60.f);
	}

	// Function should be called upon termination of program;
	// Return 0 if successful, else return exception code in case of error or exception thrown;
	int CoreEngine::Terminate() {

		// Empty for now...

		return 0;
	}

	void CoreEngine::Run() {
		try {

#ifndef EDITOR
			m_coord.Unpause();
#endif
			lastTime = Clock::now();
			while (!m_coord.GetSystem<ApplicationSystem>()->ShouldClose()) {
				Update();
			}
		}
		catch (const NightSnack::IException ex)
		{
			NightSnack::CrashLogger::LogCrash(ex, lastSystem);
			ex.print();
			exit(EXIT_FAILURE);
		}
		catch (const std::exception& ex)
		{
			NightSnack::CrashLogger::LogCrash(NightSnack::IException(ex.what(), NightSnack::ExceptionType::General, __FILE__), lastSystem);
			std::cerr << "[Exception][std::exception] " << ex.what() << std::endl;
			exit(EXIT_FAILURE);
		}
		catch (...)
		{
			NightSnack::CrashLogger::LogCrash(NightSnack::IException("Unknown exception", NightSnack::ExceptionType::General, __FILE__), lastSystem);
			std::cerr << "[Exception][Unknown]" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	void CoreEngine::Update() {
		// Start frame timing;
		m_coord.GetSystem<ApplicationSystem>()->StartFrame();

		// Accumulated time implementation;
		Clock::time_point currentTime = Clock::now();
		Duration frameTime = currentTime - lastTime;
		lastTime = currentTime;

		float dt = frameTime.count();

		// Clamp very large frame times (debugger pauses, alt-tab, etc.)
		if (dt > MAX_FRAME_TIME) {
			dt = MAX_FRAME_TIME;
		}
		accumulatedTime += dt;
		// Compute number of steps
		m_coord.steps = 0;
		while (accumulatedTime >= FixedDT) {
			m_coord.steps += 1;
			accumulatedTime -= FixedDT;
		}
		if (m_coord.IsPaused()) { m_coord.steps = 0; }

		// Iterate through Systems and Update them through Coordinator;
		for (auto& System : m_systems) {

			m_coord.GetSystem<ApplicationSystem>()->StartSystemTimer(System->GetName());
			System->Update();
			lastSystem = System->GetName();
			m_coord.GetSystem<ApplicationSystem>()->EndSystemTimer(System->GetName());
		}

		// Destroy all entities scheduled to be removed;
		m_coord.GetSystem<ApplicationSystem>()->StartSystemTimer("EntityFactory");
		m_coord.GetSystem<EntityFactory>()->PostUpdate();
		lastSystem = "EntityFactory PostUpdate";
		m_coord.GetSystem<ApplicationSystem>()->EndSystemTimer("EntityFactory");

		m_coord.GetSystem<ApplicationSystem>()->StartSystemTimer("ApplicationSystem");
		m_coord.GetSystem<ApplicationSystem>()->SwapBuffers();
		m_coord.GetSystem<ApplicationSystem>()->FrameCapUpdate();
		lastSystem = "Application Buffer-Swap";
		m_coord.GetSystem<ApplicationSystem>()->EndSystemTimer("ApplicationSystem");

		// End frame timing;
		lastSystem = "Frame Complete";
		m_coord.GetSystem<ApplicationSystem>()->EndFrame();
	}

}	// NightSnack

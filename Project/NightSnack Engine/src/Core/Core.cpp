/******************************************************************************
 * File:        Core.cpp
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan (Primary)
 *				<NAME> (Secondary, xx%) - <Comments>
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

namespace NightSnack {

	constexpr float fixedDeltaTime = 0.016f;
	static float accumulatedTime = 0.0f;
	static int numberOfSteps = 0;

	// ------------------------------------------------------------------------------------------------
	// CORE
	// ------------------------------------------------------------------------------------------------

	void CoreEngine::Initialize(int windowWidth, int windowHeight, std::string&& windowTitle) {

		// Initialize ECS Coordinator and Managers;
		m_coord.Initialize();

		// Register all Components;
		m_coord.RegisterComponent<Transform>();
		m_coord.RegisterComponent<RigidBody>();		// Physics;
		m_coord.RegisterComponent<Force>();
		m_coord.RegisterComponent<Collider>();		// Collision;

		m_coord.RegisterComponent<Color>();
		m_coord.RegisterComponent<Camera>();

		m_coord.RegisterComponent<PlayerController>();
		m_coord.RegisterComponent<Renderable>();	// Graphics;

		// Add more Components here...

		// Add the systems you want to the Core engine, Must be in specific order taught in GIT lessons;
		// i.e: Input-> Movement -> Collision -> Graphics;
		AddSystem<ApplicationSystem>(windowWidth, windowHeight, std::move(windowTitle), m_coord, core_dispatcher);
		AddSystem<InputSystem>(m_coord, core_dispatcher);
		AddSystem<EntityFactory>(m_coord, core_dispatcher, "assets/GameData/Archetypes/Archetypes.json");
		AddSystem<PlayerControllerSystem>(m_coord, core_dispatcher);
		AddSystem<PhysicsSystem>(m_coord, core_dispatcher);
		AddSystem<CollisionSystem>(m_coord, core_dispatcher);
		AddSystem<MovementSystem>(m_coord, core_dispatcher);
		AddSystem<GraphicsSystem>(m_coord, core_dispatcher);
		AddSystem<ImguiSystem>(m_coord);
	}

	// Function should be called upon termination of program;
	// Return 0 if successful, else return exception code in case of error or exception thrown;
	int CoreEngine::Terminate() {

		// Empty for now...

		return 0;
	}

	void CoreEngine::Run() {

		while (m_coord.GetSystem<ApplicationSystem>()->ShouldClose()) {
			Update();
		}
	}

	void CoreEngine::Update() {
		// Start frame timing;
		m_coord.GetSystem<ApplicationSystem>()->GetTimer().StartFrame();

		// Fixed DT with Accumulated time implementation;
		numberOfSteps = 0;	// reset step count per frame;
		float dt = m_coord.GetSystem<ApplicationSystem>()->GetTimer().GetDT();	// compute actual game loop time;
		accumulatedTime += dt;

		// Obtain number of steps this loop;
		while (accumulatedTime >= fixedDeltaTime) { 
			accumulatedTime -= fixedDeltaTime;
			++numberOfSteps;
		}

		if (m_paused) {
			numberOfSteps = 0;
			return;
		}

		// Iterate through Systems and Update them through Coordinator;
		for (auto& System : m_systems) {

			m_coord.GetSystem<ApplicationSystem>()->GetTimer().StartSystemTimer(System->GetName());

			System->Update(numberOfSteps);

			m_coord.GetSystem<ApplicationSystem>()->GetTimer().EndSystemTimer(System->GetName());
		}
		//m_coord.GetSystem<ApplicationSystem>()->GetTimer().StartSystemTimer(m_coord.GetSystem<EntityFactory>()->GetName());
		// Destroy all entities scheduled to be removed;
		for (Entity e : m_coord.GetSystem<EntityFactory>()->m_toBeDestroyed) { m_coord.DestroyEntity(e); }
		m_coord.GetSystem<EntityFactory>()->m_toBeDestroyed.clear();
		//m_coord.GetSystem<ApplicationSystem>()->GetTimer().EndSystemTimer(m_coord.GetSystem<EntityFactory>()->GetName());

		m_coord.GetSystem<ApplicationSystem>()->GetTimer().StartSystemTimer(m_coord.GetSystem<ApplicationSystem>()->GetName());
		m_coord.GetSystem<ApplicationSystem>()->GetTimer().FrameCap(102.f); // Cap frame rate to 102 FPS;
		m_coord.GetSystem<ApplicationSystem>()->SwapBuffers();
		m_coord.GetSystem<ApplicationSystem>()->GetTimer().EndSystemTimer(m_coord.GetSystem<ApplicationSystem>()->GetName());

		// End frame timing;
		m_coord.GetSystem<ApplicationSystem>()->GetTimer().EndFrame();
	}

}	// NightSnack

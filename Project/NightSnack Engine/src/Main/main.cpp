/******************************************************************************
 * File:        main.cpp
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Secondary, 5%) - Initialized Audio system and play background audio/ click sound using H key
 *				Clement Ang Yang Ying (Secondary, 5%) - Created/Inited Resource Manager, loaded & assigned window high/width/title into application
 *
 * Description:
 * This file contains the main function.
 * This file is the main entry point of the NightSnack Engine
 *
 * Copyright � 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#include <iostream>

#include "MathLibrary/Math.h"
#include "Systems/Physics/Collision.h"

#include "Core/Core.h"
#include "Core/ECS/ISystem.h"
#include "Factory/EntityFactory.h"
#include "Systems/AllSystems.h"

#include "ResourceManager/ResourceManager.h"
#include "Systems/AudioSystem.h"

#include <fmod.hpp>

int main()
{
	// Enable memory leak checking at program exit
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	try
	{
		// Enable memory leak checking at program exit
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

		// Create Core engine
		CoreEngine core;

#ifdef _DEBUG
		//(void)FMOD_ErrorString(FMOD_OK);
#endif

		// Will register all components;
		// If you create new components, please register them in this function defined in Core.cpp;
		core.Initialize("assets/Data/WindowConfig.json");
		Coordinator& coord = core.GetCoord();

		auto audioSystem = coord.GetSystem<AudioSystem>();

		//audioSystem->PlaySound("click");
		//audioSystem->PlaySound("bgm");
		/*
		// Get singletons
		NightSnack::AudioSystem* audioSystem = NightSnack::AudioSystem::GetInstance();

		// Link ResourceManager
		audioSystem->SetResourceManager(core.GetCoord().GetSystem<ResourceManager>());

		// Init AudioSystem
		if (!audioSystem->Initialize())
		{
			std::cout << "Failed to initialize AudioSystem!" << std::endl;
			return -1;
		}


		// Load sounds
		std::size_t sfxKey = audioSystem->LoadSFX("assets/Sound/SFX/sound_button_temp.wav");
		std::size_t musicKey = audioSystem->LoadMusic("assets/Sound/Music/LevelCompleteMusic.wav", true);

		if (sfxKey == 0 || musicKey == 0)
		{
			std::cout << "Failed to load sounds!" << std::endl;
			return -1;
		}

		// Play sounds
		audioSystem->PlaySound(sfxKey);   // play button click once
		audioSystem->PlaySound(musicKey); // play looping background music
		*/

		// ECS test --------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------------
		// Camera object
		Entity camera = coord.CreateEntity();
		Transform t_cam{ NightSnack::Vec3{}, 0.f, NightSnack::Vec2{} };
		coord.AddComponent<Transform>(camera, t_cam);
		Camera isCamera{
			true,
			1.f,
			1600.f,
			900.f
		};
		coord.AddComponent<Camera>(camera, isCamera);

		// Higher values will render on top of lower values
		int posIndexBackgroundLayer = 1;
		int posIndexDefaultLayer = 1;

		// Background image --------------
		// -------------------------------
		Entity background = coord.CreateEntity();
		Transform tbackground = { NightSnack::Vec3{ 0.f, 0.f, posIndexBackgroundLayer++ * NightSnack::Z_EPSILON }, {},  NightSnack::Vec2{ 1920.f, 1080.f } };
		coord.AddComponent<Transform>(background, tbackground);
		Renderable r6{ "square", "backgroundMat", RenderLayer::BACKGROUND };
		coord.AddComponent<Renderable>(background, r6);

		// Dynamic Red ball --------------
		// -------------------------------
		Entity redBallDynamic = coord.CreateEntity();
		Transform tredball = { NightSnack::Vec3{ 0.f, 0.f, posIndexDefaultLayer++ * NightSnack::Z_EPSILON }, {},  NightSnack::Vec2{ 100.f, 100.f } };
		coord.AddComponent<Transform>(redBallDynamic, tredball);
		RigidBody rredball = { 1.f, NightSnack::Vec2{}, NightSnack::Vec2{} };
		coord.AddComponent<RigidBody>(redBallDynamic, rredball);
		coord.AddComponent<Force>(redBallDynamic, Force{ 0.f,0.f });
		Collider c0{ ShapeType::AABB, ColliderType::Solid, false, 200.f, 200.f, 0.f, true };
		coord.AddComponent<Collider>(redBallDynamic, c0);
		Renderable r1{ "circle", "ColorMat", RenderLayer::DEFAULT };
		coord.AddComponent<Renderable>(redBallDynamic, r1);
		coord.AddComponent<Color>(redBallDynamic, Color{ 1.f, 0.f, 0.f, 1.f });
		// -------------------------------

		// Dynamic ball 2 ----------------
		// -------------------------------
		Entity bananaBallDynamic = coord.CreateEntity();
		Transform tbananaball = { NightSnack::Vec3{ 500.f, 0.f, posIndexDefaultLayer++ * NightSnack::Z_EPSILON }, {},  NightSnack::Vec2{ 100.f, 100.f } };
		coord.AddComponent<Transform>(bananaBallDynamic, tbananaball);
		RigidBody rbananaball = { 1.f, NightSnack::Vec2{}, NightSnack::Vec2{} };
		coord.AddComponent<RigidBody>(bananaBallDynamic, rbananaball);
		coord.AddComponent<Force>(bananaBallDynamic, Force{ 0.f,0.f });
		Collider c1{ ShapeType::Circle, ColliderType::Solid, false, 0.f, 0.f, 100.f, true };
		coord.AddComponent<Collider>(bananaBallDynamic, c1);
		Renderable r2{ "circle", "bananaMat", RenderLayer::DEFAULT };
		coord.AddComponent<Renderable>(bananaBallDynamic, r2);
		// -------------------------------

		// Control - Only draw, no collision or physics;
		// -------------------------------
		Entity control = coord.CreateEntity();
		Transform tcontrol = { NightSnack::Vec3{ 300.f, 0.f, posIndexDefaultLayer++ * NightSnack::Z_EPSILON }, {},  NightSnack::Vec2{ 50.f, 50.f } };
		coord.AddComponent<Transform>(control, tcontrol);
		Renderable r0{ "square", "targetMat", RenderLayer::DEFAULT };
		coord.AddComponent<Renderable>(control, r0);
		// -------------------------------

		// Wall wireframe ----------------
		// -------------------------------
		Entity wWall = coord.CreateEntity();
		coord.AddComponent<Transform>(wWall, Transform{ NightSnack::Vec3{ -300.f, 0.f, posIndexDefaultLayer++ * NightSnack::Z_EPSILON }, {}, NightSnack::Vec2{ 100.f, 400.f } });
		coord.AddComponent<RigidBody>(wWall, RigidBody{ 1.f, {}, {} });
		coord.AddComponent<Force>(wWall, Force{ 0.f,0.f });
		Collider c2{ ShapeType::AABB, ColliderType::Solid, true, 100.f, 400.f, 0.f, true };
		coord.AddComponent<Collider>(wWall, c2);
		Renderable r3{ "square", "ColorMat", RenderLayer::DEFAULT, true };
		coord.AddComponent<Renderable>(wWall, r3);
		coord.AddComponent<Color>(wWall, Color{ 0.f, 1.f, 0.7f, 1.f });
		// -------------------------------

		// Falling ball - Test gravity ---
		// -------------------------------
		Entity gravityBall = coord.CreateEntity();
		coord.AddComponent<Transform>(gravityBall, Transform{ NightSnack::Vec3{ -500.f, 400.f, posIndexDefaultLayer++ * NightSnack::Z_EPSILON }, {}, NightSnack::Vec2{ 30.f, 30.f } });
		coord.AddComponent<RigidBody>(gravityBall, RigidBody{ 1.f, {}, {}, true });
		coord.AddComponent<Force>(gravityBall, Force{ 0.f,0.f });
		Collider c3{ ShapeType::Circle, ColliderType::Solid, false, 0.f, 0.f, 30.f, false };
		coord.AddComponent<Collider>(gravityBall, c3);
		Renderable r4{ "circle", "ColorMat", RenderLayer::DEFAULT };
		coord.AddComponent<Renderable>(gravityBall, r4);
		coord.AddComponent<Color>(gravityBall, Color{ 0.2f, 0.2f, 0.6f, 1.f });
		coord.AddComponent<PlayerController>(gravityBall, PlayerController{ 300.f, 600.f });
		// -------------------------------

		// Right side wall ---------------
		// -------------------------------
		Entity RightWall = coord.CreateEntity();
		coord.AddComponent<Transform>(RightWall, Transform{ NightSnack::Vec3{ 800.f, 0.f, posIndexBackgroundLayer++ * NightSnack::Z_EPSILON }, {}, NightSnack::Vec2{ 100.f, 800.f } });
		coord.AddComponent<RigidBody>(RightWall, RigidBody{ 1.f, {}, {} });
		coord.AddComponent<Force>(RightWall, Force{ 0.f,0.f });
		Collider c4{ ShapeType::AABB, ColliderType::Solid, true, 100.f, 800.f, 0.f, true };
		coord.AddComponent<Collider>(RightWall, c4);
		Renderable r7{ "square", "ColorMat", RenderLayer::DEFAULT };
		coord.AddComponent<Renderable>(RightWall, r7);
		coord.AddComponent<Color>(RightWall, Color{ 1.f, 1.f, 0.f, 1.f });
		// -------------------------------

		// Left side wall ----------------
		// -------------------------------
		Entity LeftWall = coord.CreateEntity();
		coord.AddComponent<Transform>(LeftWall, Transform{ NightSnack::Vec3{ -800.f, 0.f, posIndexBackgroundLayer++ * NightSnack::Z_EPSILON }, {}, NightSnack::Vec2{ 100.f, 800.f } });
		coord.AddComponent<RigidBody>(LeftWall, RigidBody{ 1.f, {}, {} });
		coord.AddComponent<Force>(LeftWall, Force{ 0.f,0.f });
		coord.AddComponent<Collider>(LeftWall, c4);
		coord.AddComponent<Renderable>(LeftWall, r7);
		coord.AddComponent<Color>(LeftWall, Color{ 1.f, 1.f, 0.f, 1.f });
		// -------------------------------

		// Ground ------------------------
		// -------------------------------
		Entity ground = coord.CreateEntity();
		coord.AddComponent<Transform>(ground, Transform{ NightSnack::Vec3{ 0.f, -350.f, posIndexBackgroundLayer++ * NightSnack::Z_EPSILON }, {}, NightSnack::Vec2{ 1600.f, 100.f } });
		coord.AddComponent<RigidBody>(ground, RigidBody{ 1.f, {}, {} });
		Collider ground_c{ ShapeType::AABB, ColliderType::Ground, true, 1600.f, 100.f, 0.f, true };
		coord.AddComponent<Collider>(ground, ground_c);
		Renderable r5{ "square", "ColorMat", RenderLayer::DEFAULT };
		coord.AddComponent<Renderable>(ground, r5);
		coord.AddComponent<Color>(ground, Color{ 0.2f, 0.1f, 0.3f, 1.f });

		// Core Application Loop;
		while (!core.ShouldClose()) {
			core.Update();
			// Update AudioSystem every frame
			//audioSystem->Update();

			// Temporary test code for music sound
			// To be moved once audio system has been fully integrated into the ECS
			/*if (coord.GetSystem<InputSystem>()->IsKeyPressed(InputSystem::H))
			{
				audioSystem->PlaySound(sfxKey);
				std::cout << "Playing SFX!\n";
			}*/

			// Reset input states is temporarily reset here! 
			// Once audio system has been fully integrated into the ECS, 
			// move this function call to Core.cpp Update()
			//coord.GetSystem<InputSystem>()->ResetInputStates();
		}

		// Clean termination;
		//rm->DeleteRM();
		//fmodSystem->close();
		// Clean up when game exits
		//NightSnack::AudioSystem::DestroyInstance();

		return 0;
	}
	catch (const NightSnack::IException ex)
	{
		NightSnack::CrashLogger::LogCrash(ex);
		ex.print();
		return EXIT_FAILURE;
	}
	catch (const std::exception& ex)
	{
		NightSnack::CrashLogger::LogCrash(NightSnack::IException(ex.what(), NightSnack::ExceptionType::General, __FILE__));
		std::cerr << "[Exception][std::exception] " << ex.what() << std::endl;
		return EXIT_FAILURE;
	}
	catch (...)
	{
		NightSnack::CrashLogger::LogCrash(NightSnack::IException("Unknown exception", NightSnack::ExceptionType::General, __FILE__));
		std::cerr << "[Exception][Unknown]" << std::endl;
		return EXIT_FAILURE;
	}

}
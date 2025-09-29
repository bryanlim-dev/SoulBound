#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#include <iostream>
//#include <thread>
//#include <chrono>

#include "MathLibrary/Math.h"
#include "Systems/Collision.h"

#include "Core.h"
#include "ECS/ISystem.h"
#include "Factory/EntityFactory.h"
#include "Systems/AllSystems.h"

#include "ResourceManager/ResourceManager.h"
#include "Systems/AudioSystem.h"

#include <fmod.hpp>


int main()
{
	try
	{
		// Enable memory leak checking at program exit
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

		////Resource Manager test
		//{
		//	//rm->DebugTestEverything();
		//}

		////Replace this with actual sound manager later
		//FMOD::System* fmodSystem = nullptr;
		//FMOD::System_Create(&fmodSystem);

		// Create Core engine
		CoreEngine core;

		//Creatr resource manager
		ResourceManager* rm = ResourceManager::GetInstance();
		//rm->DebugTestEverything();
		size_t configKey = 0;
		rm->LoadJson("assets/Data/WindowConfig.json", configKey);
		std::string windowTitle = "";
		int winWidth = 0;
		int winHeight = 0;

		if (rm->ResourceLoaded(RESOURCE_JSON, configKey))
		{
			windowTitle = (rm->GetJson(configKey)["WindowConfig"].componentMap["WindowTitle"]);
			winWidth = std::stoi(rm->GetJson(configKey)["WindowConfig"].componentMap["Width"]);
			winHeight = std::stoi(rm->GetJson(configKey)["WindowConfig"].componentMap["Height"]);
			rm->UnloadJson(configKey);
		}
		else
		{
			windowTitle = "Load Failed";
			winWidth = 100;
			winHeight = 100;
		}

		// Will register all components;
		// If you create new components, please register them in this function defined in Core.cpp;
		core.Initialize(winWidth, winHeight, windowTitle.c_str());
		Coordinator& coord = core.GetCoord();

		// Get singletons
		AudioSystem* audioSystem = AudioSystem::GetInstance();

		// Init AudioSystem
		if (!audioSystem->Initialize())
		{
			std::cout << "Failed to initialize AudioSystem!" << std::endl;
			return -1;
		}


		//Crash Logger test message
		//throw THROW_IEXCEPTION("This is a real crash test from main!", ExceptionType::General);

		////Replace this with actual sound manager later
		//FMOD::System* fmodSystem = nullptr;
		//FMOD::System_Create(&fmodSystem);


		// Link ResourceManager
		audioSystem->SetResourceManager(rm);

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

		// ECS test --------------------------------------------------------------------------------------
		// -----------------------------------------------------------------------------------------------
		// Camera object
		Entity camera = coord.CreateEntity();
		Position posCam{ 0.f, 0.f, 0.f };
		Rotation rotCam{ 0.f };
		Scale scaleCam{ 200.f, 200.f };
		coord.AddComponent<Position>(camera, posCam);
		coord.AddComponent<Rotation>(camera, rotCam);
		coord.AddComponent<Scale>(camera, scaleCam);
		Camera isCamera{
			true,
			1.f,
			1600.f,
			900.f
		};
		coord.AddComponent<Camera>(camera, isCamera);

		Entity background = coord.CreateEntity();
		Entity p0 = coord.CreateEntity();
		Entity p1 = coord.CreateEntity();
		Entity p2 = coord.CreateEntity();
		Entity p3 = coord.CreateEntity();
		Entity ground = coord.CreateEntity();
		// To test that systems don't affect entities that don't have all required components;
		Entity control = coord.CreateEntity();

		Collider c0{ ShapeType::Circle, ColliderType::Solid, false, 0.f, 0.f, 100.f, true };
		coord.AddComponent<Collider>(p0, c0);
		Collider c1{ ShapeType::Circle, ColliderType::Solid, false, 0.f, 0.f, 100.f, true };
		coord.AddComponent<Collider>(p1, c1);
		Collider c2{ ShapeType::AABB, ColliderType::Solid, true, 100.f, 400.f, 0.f, true };
		coord.AddComponent<Collider>(p2, c2);
		Collider ground_c{ ShapeType::AABB, ColliderType::Ground, true, 1600.f, 100.f, 0.f, true };
		coord.AddComponent<Collider>(ground, ground_c);
		// isGrounded->false to test gravity;
		Collider c3{ ShapeType::Circle, ColliderType::Solid, false, 0.f, 0.f, 50.f, false };
		coord.AddComponent<Collider>(p3, c3);

		// Higher values will render on top of lower values
		int posIndexBackgroundLayer = 1;
		int posIndexDefaultLayer = 1;
		coord.AddComponent<Position>(control, Position{ 300.f, 0.f, posIndexDefaultLayer++ * Z_EPSILON });
		coord.AddComponent<Position>(p0, Position{ 0.f, 0.f, posIndexDefaultLayer++ * Z_EPSILON });
		coord.AddComponent<Position>(p1, Position{ 500.f, 0.f, posIndexDefaultLayer++ * Z_EPSILON });
		coord.AddComponent<Position>(p2, Position{ -300.f, 0.f, posIndexDefaultLayer++ * Z_EPSILON });
		coord.AddComponent<Position>(background, Position{ 0.f, 0.f, posIndexBackgroundLayer++ * Z_EPSILON });
		coord.AddComponent<Position>(p3, Position{ -500.f, 400.f, posIndexBackgroundLayer++ * Z_EPSILON });
		coord.AddComponent<Position>(ground, Position{ 0.f, -350.f, posIndexBackgroundLayer++ * Z_EPSILON });

		coord.AddComponent<Mass>(p0, Mass{ 1.f });
		coord.AddComponent<Mass>(p1, Mass{ 1.f });
		coord.AddComponent<Mass>(p2, Mass{ 1.f });
		coord.AddComponent<Mass>(p3, Mass{ 1.f });
		coord.AddComponent<Mass>(ground, Mass{ 1.f });

		coord.AddComponent<Acceleration>(p0, Acceleration{ 0.f,0.f });
		coord.AddComponent<Acceleration>(p1, Acceleration{ 0.f,0.f });
		coord.AddComponent<Acceleration>(p2, Acceleration{ 0.f,0.f });
		coord.AddComponent<Acceleration>(p3, Acceleration{ 0.f,0.f });

		coord.AddComponent<Force>(p0, Force{ 0.f,0.f });
		coord.AddComponent<Force>(p1, Force{ 0.f,0.f });
		coord.AddComponent<Force>(p2, Force{ 0.f,0.f });
		coord.AddComponent<Force>(p3, Force{ 0.f,0.f });

		// Using Physics System interface to make entity move instead;
		coord.AddComponent<Velocity>(p0, Velocity{ 0.f, 0.f });
		coord.AddComponent<Velocity>(p1, Velocity{ 0.f, 0.f });
		coord.AddComponent<Velocity>(p2, Velocity{ 0.f, 0.f });
		coord.AddComponent<Velocity>(p3, Velocity{ 0.f, 0.f });
		coord.AddComponent<Velocity>(ground, Velocity{ 0.f, 0.f });

		coord.AddComponent<Scale>(control, Scale{ 20.f, 20.f });
		coord.AddComponent<Scale>(p0, Scale{ 100.f, 100.f });
		coord.AddComponent<Scale>(p1, Scale{ 100.f, 100.f });
		coord.AddComponent<Scale>(p2, Scale{ 100.f, 400.f });
		coord.AddComponent<Scale>(p3, Scale{ 50.f, 50.f });
		coord.AddComponent<Scale>(ground, Scale{ 1600.f, 100.f });
		coord.AddComponent<Scale>(background, Scale{ 1920.f, 1080.f });

		coord.AddComponent<Rotation>(control, Rotation{});
		coord.AddComponent<Rotation>(p0, Rotation{});
		coord.AddComponent<Rotation>(p1, Rotation{});
		coord.AddComponent<Rotation>(p2, Rotation{});
		coord.AddComponent<Rotation>(p3, Rotation{});
		coord.AddComponent<Rotation>(ground, Rotation{});
		coord.AddComponent<Rotation>(background, Rotation{});

		// TESTING GRAPHICS;
		Renderable r0{ "square", "bananaMat", RenderLayer::DEFAULT };
		coord.AddComponent<Renderable>(control, r0);
		Renderable r1{ "circle", "ColorMat", RenderLayer::DEFAULT };
		coord.AddComponent<Renderable>(p0, r1);
		Renderable r2{ "circle", "bananaMat", RenderLayer::DEFAULT };
		coord.AddComponent<Renderable>(p1, r2);
		Renderable r3{ "square", "bananaMat", RenderLayer::DEFAULT, true };
		coord.AddComponent<Renderable>(p2, r3);
		Renderable r4{ "circle", "ColorMat", RenderLayer::DEFAULT };
		coord.AddComponent<Renderable>(p3, r4);
		Renderable r5{ "square", "ColorMat", RenderLayer::DEFAULT };
		coord.AddComponent<Renderable>(ground, r5);
		Renderable r6{ "square", "backgroundMat", RenderLayer::BACKGROUND };
		coord.AddComponent<Renderable>(background, r6);

<<<<<<< Updated upstream
		coord.AddComponent<Color>(p0, Color{ 1.f, 0.f, 0.f, 1.f });
		coord.AddComponent<Color>(p3, Color{ 0.f, 1.f, 0.f, 1.f });
		coord.AddComponent<Color>(ground, Color{ 0.8f, 0.f, 0.8f, 1.f });
=======
		// Dynamic Red ball --------------
		// -------------------------------
		Entity redBallDynamic = coord.CreateEntity();
		coord.AddComponent<Position>(redBallDynamic, Position{ 0.f, 0.f, posIndexDefaultLayer++ * Z_EPSILON });
		coord.AddComponent<Rotation>(redBallDynamic, Rotation{});
		coord.AddComponent<Scale>(redBallDynamic, Scale{ 100.f, 100.f });
		coord.AddComponent<Acceleration>(redBallDynamic, Acceleration{ 0.f,0.f });
		coord.AddComponent<Velocity>(redBallDynamic, Velocity{ 0.f, 0.f });
		coord.AddComponent<Force>(redBallDynamic, Force{ 0.f,0.f });
		coord.AddComponent<Mass>(redBallDynamic, Mass{ 1.f });
		Collider c0{ ShapeType::Circle, ColliderType::Solid, false, 0.f, 0.f, 100.f, true };
		coord.AddComponent<Collider>(redBallDynamic, c0);
		Renderable r1{ "circle", "ColorMat", RenderLayer::DEFAULT };
		coord.AddComponent<Renderable>(redBallDynamic, r1);
		coord.AddComponent<Color>(redBallDynamic, Color{ 1.f, 0.f, 0.f, 1.f });
		// -------------------------------

		// Dynamic ball 2 ----------------
		// -------------------------------
		Entity bananaBallDynamic = coord.CreateEntity();
		coord.AddComponent<Position>(bananaBallDynamic, Position{ 500.f, 0.f, posIndexDefaultLayer++ * Z_EPSILON });
		coord.AddComponent<Rotation>(bananaBallDynamic, Rotation{});
		coord.AddComponent<Scale>(bananaBallDynamic, Scale{ 100.f, 100.f });
		coord.AddComponent<Velocity>(bananaBallDynamic, Velocity{ 0.f, 0.f });
		coord.AddComponent<Acceleration>(bananaBallDynamic, Acceleration{ 0.f,0.f });
		coord.AddComponent<Force>(bananaBallDynamic, Force{ 0.f,0.f });
		coord.AddComponent<Mass>(bananaBallDynamic, Mass{ 1.f });
		Collider c1{ ShapeType::Circle, ColliderType::Solid, false, 0.f, 0.f, 100.f, true };
		coord.AddComponent<Collider>(bananaBallDynamic, c1);
		Renderable r2{ "circle", "bananaMat", RenderLayer::DEFAULT };
		coord.AddComponent<Renderable>(bananaBallDynamic, r2);
		// -------------------------------

		// Control - Only draw, no collision or physics;
		// -------------------------------
		Entity control = coord.CreateEntity();
		coord.AddComponent<Position>(control, Position{ 300.f, 0.f, posIndexDefaultLayer++ * Z_EPSILON });
		coord.AddComponent<Rotation>(control, Rotation{});
		coord.AddComponent<Scale>(control, Scale{ 50.f, 50.f });
		Renderable r0{ "square", "targetMat", RenderLayer::DEFAULT };
		coord.AddComponent<Renderable>(control, r0);
		// -------------------------------

		// Wall wireframe ----------------
		// -------------------------------
		Entity wWall = coord.CreateEntity();
		coord.AddComponent<Position>(wWall, Position{ -300.f, 0.f, posIndexDefaultLayer++ * Z_EPSILON });
		coord.AddComponent<Rotation>(wWall, Rotation{});
		coord.AddComponent<Scale>(wWall, Scale{ 100.f, 400.f });
		coord.AddComponent<Acceleration>(wWall, Acceleration{ 0.f,0.f });
		coord.AddComponent<Velocity>(wWall, Velocity{ 0.f, 0.f });
		coord.AddComponent<Force>(wWall, Force{ 0.f,0.f });
		coord.AddComponent<Mass>(wWall, Mass{ 1.f });
		Collider c2{ ShapeType::AABB, ColliderType::Solid, true, 100.f, 400.f, 0.f, true };
		coord.AddComponent<Collider>(wWall, c2);
		Renderable r3{ "square", "ColorMat", RenderLayer::DEFAULT, true };
		coord.AddComponent<Renderable>(wWall, r3);
		coord.AddComponent<Color>(wWall, Color{ 0.f, 1.f, 0.7f, 1.f });
		// -------------------------------

		// Falling ball - Test gravity ---
		// -------------------------------
		Entity gravityBall = coord.CreateEntity();
		coord.AddComponent<Position>(gravityBall, Position{ -500.f, 400.f, posIndexDefaultLayer++ * Z_EPSILON });
		coord.AddComponent<Rotation>(gravityBall, Rotation{});
		coord.AddComponent<Scale>(gravityBall, Scale{ 30.f, 30.f });
		coord.AddComponent<Acceleration>(gravityBall, Acceleration{ 0.f,0.f });
		coord.AddComponent<Velocity>(gravityBall, Velocity{ 0.f, 0.f });
		coord.AddComponent<Force>(gravityBall, Force{ 0.f,0.f });
		coord.AddComponent<Mass>(gravityBall, Mass{ 1.f });
		Collider c3{ ShapeType::Circle, ColliderType::Solid, false, 0.f, 0.f, 30.f, false };
		coord.AddComponent<Collider>(gravityBall, c3);
		Renderable r4{ "circle", "ColorMat", RenderLayer::DEFAULT };
		coord.AddComponent<Renderable>(gravityBall, r4);
		coord.AddComponent<Color>(gravityBall, Color{ 0.f, 1.f, 0.f, 1.f });
		coord.AddComponent<PlayerController>(gravityBall, PlayerController{25.f, 50.f});
		// -------------------------------

		// Right side wall ---------------
		// -------------------------------
		Entity RightWall = coord.CreateEntity();
		coord.AddComponent<Position>(RightWall, Position{ 800.f, 0.f, posIndexBackgroundLayer++ * Z_EPSILON });
		coord.AddComponent<Rotation>(RightWall, Rotation{});
		coord.AddComponent<Scale>(RightWall, Scale{ 100.f, 800.f });
		coord.AddComponent<Acceleration>(RightWall, Acceleration{ 0.f,0.f });
		coord.AddComponent<Velocity>(RightWall, Velocity{ 0.f, 0.f });
		coord.AddComponent<Force>(RightWall, Force{ 0.f,0.f });
		coord.AddComponent<Mass>(RightWall, Mass{ 1.f });
		Collider c4{ ShapeType::AABB, ColliderType::Solid, true, 100.f, 800.f, 0.f, true };
		coord.AddComponent<Collider>(RightWall, c4);
		Renderable r7{ "square", "ColorMat", RenderLayer::DEFAULT};
		coord.AddComponent<Renderable>(RightWall, r7);
		coord.AddComponent<Color>(RightWall, Color{ 1.f, 1.f, 0.f, 1.f });
		// -------------------------------

		// Left side wall ----------------
		// -------------------------------
		Entity LeftWall = coord.CreateEntity();
		coord.AddComponent<Position>(LeftWall, Position{ -800.f, 0.f, posIndexBackgroundLayer++ * Z_EPSILON });
		coord.AddComponent<Rotation>(LeftWall, Rotation{});
		coord.AddComponent<Scale>(LeftWall, Scale{ 100.f, 800.f });
		coord.AddComponent<Acceleration>(LeftWall, Acceleration{ 0.f,0.f });
		coord.AddComponent<Velocity>(LeftWall, Velocity{ 0.f, 0.f });
		coord.AddComponent<Force>(LeftWall, Force{ 0.f,0.f });
		coord.AddComponent<Mass>(LeftWall, Mass{ 1.f });
		coord.AddComponent<Collider>(LeftWall, c4);
		coord.AddComponent<Renderable>(LeftWall, r7);
		coord.AddComponent<Color>(LeftWall, Color{ 1.f, 1.f, 0.f, 1.f });
		// -------------------------------

		// Ground ------------------------
		// -------------------------------
		Entity ground = coord.CreateEntity();
		coord.AddComponent<Position>(ground, Position{ 0.f, -350.f, posIndexBackgroundLayer++ * Z_EPSILON });
		coord.AddComponent<Rotation>(ground, Rotation{});
		coord.AddComponent<Scale>(ground, Scale{ 1600.f, 100.f });
		coord.AddComponent<Velocity>(ground, Velocity{ 0.f, 0.f });
		coord.AddComponent<Mass>(ground, Mass{ 1.f });
		Collider ground_c{ ShapeType::AABB, ColliderType::Ground, true, 1600.f, 100.f, 0.f, true };
		coord.AddComponent<Collider>(ground, ground_c);
		Renderable r5{ "square", "ColorMat", RenderLayer::DEFAULT };
		coord.AddComponent<Renderable>(ground, r5);
		coord.AddComponent<Color>(ground, Color{ 0.2f, 0.1f, 0.3f, 1.f });
>>>>>>> Stashed changes

		// Core Application Loop;
		while (!core.ShouldClose()) {
			core.Update();
			// Update AudioSystem every frame
			audioSystem->Update();

			/*if (coord.GetSystem<InputSystem>()->IsKeyPressed(InputSystem::SPACE))
			{
				audioSystem->PlaySound(sfxKey);
				std::cout << "Playing SFX!\n";
			}*/
		}

		// Clean termination;
		rm->DeleteRM();
		//fmodSystem->close();
		// Clean up when game exits
		AudioSystem::DestroyInstance();

		return 0;
	}
	catch (const IException ex)
	{
		CrashLogger::LogCrash(ex);
		ex.print();
		return EXIT_FAILURE;
	}
	catch (const std::exception& ex)
	{
		CrashLogger::LogCrash(IException(ex.what(), ExceptionType::General, __FILE__));
		std::cerr << "[Exception][std::exception] " << ex.what() << std::endl;
		return EXIT_FAILURE;
	}
	catch (...)
	{
		CrashLogger::LogCrash(IException("Unknown exception", ExceptionType::General, __FILE__));
		std::cerr << "[Exception][Unknown]" << std::endl;
		return EXIT_FAILURE;
	}
}
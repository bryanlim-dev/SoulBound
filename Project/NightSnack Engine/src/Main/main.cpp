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

		coord.AddComponent<Color>(p0, Color{ 1.f, 0.f, 0.f, 1.f });
		coord.AddComponent<Color>(p3, Color{ 0.f, 1.f, 0.f, 1.f });
		coord.AddComponent<Color>(ground, Color{ 0.8f, 0.f, 0.8f, 1.f });

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
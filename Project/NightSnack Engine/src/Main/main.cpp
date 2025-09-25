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
#include "Systems/AllSystems.h"

#include "ResourceManager/ResourceManager.h"
#include "Systems/AudioSystem.h"

#include <fmod.hpp>


int main()
{
	// Enable memory leak checking at program exit
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//Resource Manager test
	{
		//rm->DebugTestEverything();
	}

	//Replace this with actual sound manager later
	FMOD::System* fmodSystem = nullptr;
	FMOD::System_Create(&fmodSystem);

  	// Create Core engine
  	CoreEngine core;

  	// Will register all components;
  	// If you create new components, please register them in this function defined in Core.cpp;
  	core.Initialize(1600, 900, "NightSnack Engine");

	// Get singletons
	ResourceManager* rm = ResourceManager::GetInstance();
	AudioSystem* audioSystem = AudioSystem::GetInstance();

	// Init AudioSystem
	if (!audioSystem->Initialize())
	{
		std::cout << "Failed to initialize AudioSystem!" << std::endl;
		return -1;
	}

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
	Coordinator& coord = core.GetCoord();
	Entity p0 = coord.CreateEntity();
	Entity p1 = coord.CreateEntity();
	Entity p2 = coord.CreateEntity();
	Entity p3 = coord.CreateEntity();
	Entity p4ground = coord.CreateEntity();
	// To test that systems don't affect entities that don't have all required components;
	Entity control = coord.CreateEntity();

	Collider c1{ ShapeType::Circle, ColliderType::Solid, false, 0.f, 0.f, 100.f, true };
	coord.AddComponent<Collider>(p0, c1);
	Collider c2{ ShapeType::Circle, ColliderType::Solid, false, 0.f, 0.f, 100.f, true };
	coord.AddComponent<Collider>(p1, c2);
	Collider c3{ ShapeType::AABB, ColliderType::Solid, true, 100.f, 400.f, 0.f, true };
	coord.AddComponent<Collider>(p2, c3);
	Collider ground_c{ ShapeType::AABB, ColliderType::Ground, true, 1600.f, 100.f, 0.f, true };
	coord.AddComponent<Collider>(p4ground, ground_c);
	// isGrounded->false to test gravity;
	Collider c4{ ShapeType::Circle, ColliderType::Solid, false, 0.f, 0.f, 50.f, false };
	coord.AddComponent<Collider>(p3, c4);

	coord.AddComponent<Position>(control, Position{ 300.f, 0.f });
	coord.AddComponent<Position>(p0, Position{ 0.f, 0.f });
	coord.AddComponent<Position>(p1, Position{ 500.f, 0.f });
	coord.AddComponent<Position>(p2, Position{ -300.f, 0.f });
	coord.AddComponent<Position>(p3, Position{ -500.f, 400.f });
	coord.AddComponent<Position>(p4ground, Position{ 0.f, -350.f });

	coord.AddComponent<Mass>(p0, Mass{ 1.f });
	coord.AddComponent<Mass>(p1, Mass{ 1.f });
	coord.AddComponent<Mass>(p2, Mass{ 1.f });
	coord.AddComponent<Mass>(p3, Mass{ 1.f });
	coord.AddComponent<Mass>(p4ground, Mass{ 1.f });

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
	coord.AddComponent<Velocity>(p4ground, Velocity{ 0.f, 0.f });

	coord.AddComponent<Scale>(control, Scale{ 20.f, 20.f });
	coord.AddComponent<Scale>(p0, Scale{ 100.f, 100.f });
	coord.AddComponent<Scale>(p1, Scale{ 100.f, 100.f });
	coord.AddComponent<Scale>(p2, Scale{ 100.f, 400.f });
	coord.AddComponent<Scale>(p3, Scale{ 50.f, 50.f });
	coord.AddComponent<Scale>(p4ground, Scale{ 1600.f, 100.f });

	coord.AddComponent<Rotation>(control, Rotation{});
	coord.AddComponent<Rotation>(p0, Rotation{});
	coord.AddComponent<Rotation>(p1, Rotation{});
	coord.AddComponent<Rotation>(p2, Rotation{});
	coord.AddComponent<Rotation>(p3, Rotation{});
	coord.AddComponent<Rotation>(p4ground, Rotation{});


	// Load shader
	Shader shader("../shaders/vertex_shader.vert", "../shaders/fragment_shader.frag");

	// Load textures
	std::size_t bananaKey = 0;
	std::size_t targetKey = 0;
	rm->LoadImage("assets/Banana.jpg", bananaKey);
	rm->LoadImage("assets/image_UI_target.png", targetKey);
	ResourceImage* imgBanana = rm->GetImageResource(bananaKey);
	ResourceImage* imgTarget = rm->GetImageResource(targetKey);
	unsigned char* pixelsBanana = rm->GetImage(bananaKey);
	unsigned char* pixelsTarget = rm->GetImage(targetKey);
	Texture bananaTex(pixelsBanana, imgBanana->Width(), imgBanana->Height(), imgBanana->Channels());
	Texture targetTex(pixelsTarget, imgTarget->Width(), imgTarget->Height(), imgTarget->Channels());

	// Create materials with texture
	Material bananaMat(&shader, &bananaTex);
	Material targetMat(&shader, &targetTex);

	// Create materials with color
	Material red(&shader, glm::vec3(1.f, 0.f, 0.f));
	Material blue(&shader, glm::vec3(0.f, 0.f, 1.f));
	Material green(&shader, glm::vec3(0.f, 1.f, 0.f));
	Mesh rectangle = Mesh::CreateSquare();

	// Create different mesh shapes
	Mesh point = Mesh::CreatePoint();
	Mesh line = Mesh::CreateLine({ 0.f, 0.f }, { 100.f, 0.f });
	Mesh square = Mesh::CreateSquare();
	Mesh circle = Mesh::CreateCircle();

	// Create renderables
	Renderable r1{ &circle, &targetMat, false };
	Renderable r2{ &circle, &bananaMat, false };
	Renderable r3{ &rectangle, &red, true };
	Renderable r4{ &circle, &red, true };
	Renderable r5{ &circle, &red, false };
	Renderable ground_r{ &rectangle, &blue , false };

	coord.AddComponent<Renderable>(control, r4);
	coord.AddComponent<Renderable>(p0, r1); 
	coord.AddComponent<Renderable>(p1, r2);
	coord.AddComponent<Renderable>(p2, r3);
	coord.AddComponent<Renderable>(p3, r5);
	coord.AddComponent<Renderable>(p4ground, ground_r);

	// Camera object
	Entity camera = coord.CreateEntity();
	Position posCam{ 0.0, 0.0 };
	Rotation rotCam{ 0.f };
	Scale scaleCam{ 200.0,200.0 };
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

	if (coord.GetSystem<InputSystem>()->IsKeyPressed(InputSystem::SPACE)) 
	{
		audioSystem->PlaySound(sfxKey);
		std::cout << "Playing SFX!\n";
	}
  }

	// Clean termination;
	rm->DeleteRM();
	fmodSystem->close();
	// Clean up when game exits
	AudioSystem::DestroyInstance();
  return 0;

}
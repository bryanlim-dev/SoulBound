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
#include <fmod.hpp>


int main()
{
	// Enable memory leak checking at program exit
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	ResourceManager* rm = ResourceManager::GetInstance();

	//Resource Manager test
	{
		rm->DebugTestEverything();
	}

	//Replace this with actual sound manager later
	FMOD::System* fmodSystem = nullptr;
	FMOD::System_Create(&fmodSystem);

  	// Create Core engine
  	CoreEngine core;

  	// Will register all components;
  	// If you create new components, please register them in this function defined in Core.cpp;
  	core.Initialize(1600, 900, "NightSnack Engine");

	// ECS test;
	Coordinator& coord = core.GetCoord();
	Entity p1 = coord.CreateEntity();
	Entity p2 = coord.CreateEntity();
	Entity p3 = coord.CreateEntity();
	Entity control = coord.CreateEntity();

	Collider c1{ ShapeType::Circle, ColliderType::Solid, false, 0.f, 0.f, 100.f };
	coord.AddComponent<Collider>(p1, c1);
	Collider c2{ ShapeType::Circle, ColliderType::Solid, false, 0.f, 0.f, 100.f };
	coord.AddComponent<Collider>(p2, c2);
	Collider c3{ ShapeType::AABB, ColliderType::Solid, true, 100.f, 400.f, 0.f };
	coord.AddComponent<Collider>(p3, c3);

	coord.AddComponent<Position>(p1, Position{ 0., 0. });
	coord.AddComponent<Position>(p2, Position{ 500, 0. });
	coord.AddComponent<Position>(p3, Position{ -300., 0. });

	coord.AddComponent<Velocity>(p1, Velocity{ 1500.0, 0. });
	coord.AddComponent<Velocity>(p2, Velocity{ 0.0, 0. });
	coord.AddComponent<Velocity>(p3, Velocity{ 0.0, 0. });

	coord.AddComponent<Scale>(p1, Scale{ 100.0, 100.0 });
	coord.AddComponent<Scale>(p2, Scale{ 100.0, 100.0 });
	coord.AddComponent<Scale>(p3, Scale{ 100.0, 400.0 });

	coord.AddComponent<Rotation>(p1, Rotation{});
	coord.AddComponent<Rotation>(p2, Rotation{});
	coord.AddComponent<Rotation>(p3, Rotation{});


	Mesh rectangle = Mesh::CreateSquare();
	Mesh circle = Mesh::CreateCircle();

	// Load shader
	Shader shader("../shaders/vertex_shader.vert", "../shaders/fragment_shader.frag");

	// Create the different materials
	Material red(&shader, glm::vec3(1, 0, 0));
	Material blue(&shader, glm::vec3(0, 0, 1));
	Material green(&shader, glm::vec3(0, 1, 0));

	Renderable r1{ &circle, &green, false };
	Renderable r2{ &circle, &blue, false };
	Renderable r3{ &rectangle, &red, false };

	coord.AddComponent<Renderable>(p1, r1);
	coord.AddComponent<Renderable>(p2, r2);
	coord.AddComponent<Renderable>(p3, r3);

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

  }

	// Clean termination;
	rm->DeleteRM();
	fmodSystem->close();
  return 0;

}
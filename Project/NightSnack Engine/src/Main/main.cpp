#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#include "AppWindow/Window.h"
#include <iostream>

#include "Core.h"
#include "Systems/AllSystems.h"		// Include your system header in this file;

int main()
{
  // Enable memory leak checking at program exit
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

  // Create Core engine, passing window parameters;
  CoreEngine core(1600, 900, "NightSnack Engine");

  // Will register all components;
  // If you create new components, please register them in this function defined in Core.cpp;
  core.Initialize();

  // Add the systems you want to the Core engine, Must be in specific order taught in GIT lessons;
  // i.e: Input-> Movement -> Collision -> Graphics;
  core.AddSystem<MovementSystem>();
  core.AddSystem<GraphicsSystem>();

  // Add more systems here...

<<<<<<< HEAD
#pragma region Temporary test code for rendering basic debugging shapes
	// Load shader
	Shader shader("../shaders/vertex_shader.vert", "../shaders/fragment_shader.frag");
	
	// Create the different meshes
	Mesh point = Mesh::CreatePoint();
	Mesh line = Mesh::CreateLine();
	Mesh rectangle = Mesh::CreateSquare();
	Mesh circle = Mesh::CreateCircle();

	// Create the different materials
	Material red(&shader, glm::vec3(1, 0, 0));
	Material blue(&shader, glm::vec3(0, 0, 1));
	Material green(&shader, glm::vec3(0, 1, 0));

	// Test graphics system with ECS
	Coordinator& coord = core.GetCoord();

	// Square object
	Entity square = coord.CreateEntity();
	Position posSquare{ 200.0, -300.0 };
	Rotation rotSquare{ 0.f };
	Scale scaleSquare{ 800.0, 450.0 };
	coord.AddComponent<Position>(square, posSquare);
	coord.AddComponent<Rotation>(square, rotSquare);
	coord.AddComponent<Scale>(square, scaleSquare);
	Renderable renderable{
		&rectangle,
		&red,
		true
	};
	coord.AddComponent<Renderable>(square, renderable);

	// Sphere object
	Entity sphere = coord.CreateEntity();
	Position posSphere{ 0.0,0.0 };
	Rotation rotSphere{ 0.f };
	Scale scaleSphere{ 800.0, 450.0 };
	coord.AddComponent<Position>(sphere, posSphere);
	coord.AddComponent<Rotation>(sphere, rotSphere);
	coord.AddComponent<Scale>(sphere, scaleSphere);
	Renderable renderable3{
		&circle,
		&green,
		true
	};
	coord.AddComponent<Renderable>(sphere, renderable3);

	// Camera object
	Entity camera = coord.CreateEntity();
	Position posCam{ 200.0, -300.0 };
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

#pragma endregion

  // Core Application Loop;
  while (!core.ShouldClose()) {
  	core.Update();
  }


  // Clean termination;
=======
	// Create Core engine, passing window parameters;
	CoreEngine Core(1600, 900, "NightSnack Engine");

	// Will register all components;
	// If you create new components, please register them in this function defined in Core.cpp;
	Core.Initialize();

	// Add the systems you want to the Core engine, Must be in specific order taught in GIT lessons;
	// i.e: Input-> Movement -> Collision -> Graphics;
	Core.AddSystem<MovementSystem>();

	// Add more systems here...

	// Core Application Loop;
	while (!Core.ShouldClose()) {
		Core.Update();
	}

	// Clean termination;
>>>>>>> SystemTimers
  return 0;
}
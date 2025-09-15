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
  CoreEngine Core(1600, 900, "NightSnack Engine");

  // Will register all components;
  // If you create new components, please register them in this function defined in Core.cpp;
  Core.Initialize();

  // Add the systems you want to the Core engine, Must be in specific order taught in GIT lessons;
  // i.e: Input-> Movement -> Collision -> Graphics;
  Core.AddSystem<MovementSystem>();
  Core.AddSystem<GraphicsSystem>();

  // Add more systems here...

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
	Coordinator& Coord = Core.GetCoord();

	// Square object
	Entity square = Coord.CreateEntity();
	Transform xform{
		glm::vec2(200.f, -300.f),
		0.f,
		glm::vec2(800.f, 450.f)
	};
	Coord.AddComponent<Transform>(square, xform);
	Renderable renderable{
		&rectangle,
		&red,
		true
	};
	Coord.AddComponent<Renderable>(square, renderable);

	// Sphere object
	Entity sphere = Coord.CreateEntity();
	Transform xform3{
		glm::vec2(0.f, 0.f),
		0.f,
		glm::vec2(800.f, 450.f)
	};
	Coord.AddComponent<Transform>(sphere, xform3);
	Renderable renderable3{
		&circle,
		&green,
		true
	};
	Coord.AddComponent<Renderable>(sphere, renderable3);

	// Camera object
	Entity camera = Coord.CreateEntity();
	Transform xform2{
		glm::vec2(200.f, -300.f),
		90.f,
		glm::vec2(200.f, 200.f)
	};
	Coord.AddComponent<Transform>(camera, xform2);
	Camera isCamera{
		true,
		1.f,
		1600.f,
		900.f
	};
	Coord.AddComponent<Camera>(camera, isCamera);

#pragma endregion

  // Core Application Loop;
  while (!Core.ShouldClose()) {
  	Core.Update();
  }


  // Clean termination;
  return 0;
}
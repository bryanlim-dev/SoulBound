#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#include "AppWindow/Window.h"
#include <iostream>

#include "Core.h"
#include "Systems/AllSystems.h"		// Include your system header in this file;

int main_testECS()
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

	// Add more systems here...


	// ECS test;
	Coordinator& Coord = Core.GetCoord();
	Entity P1 = Coord.CreateEntity();
	std::cout << "Created entity with ID: " << P1 << "\n";
	Entity P2 = Coord.CreateEntity();
	std::cout << "Created entity with ID: " << P2 << "\n";

	Position pos1{ 0.0, 0.0 };
	Coord.AddComponent<Position>(P1, pos1);
	std::cout << "Added Position: " << Coord.GetComponent<Position>(P1).x << ", " << Coord.GetComponent<Position>(P1).y << " to Entity : " << P1 << "\n";
	Velocity vel1{ 1.0, 1.0 };
	Coord.AddComponent<Velocity>(P1, vel1);
	std::cout << "Added Velocity:" << Coord.GetComponent<Velocity>(P1).dx << ", " << Coord.GetComponent<Velocity>(P1).dy << " to Entity : " << P1 << "\n";

	std::cout << "Entity " << P1 << "'s Signature: " << Coord.GetEntitySignature(P1) << "\n";

	Position pos2{ 1.5, 3.2 };
	Coord.AddComponent<Position>(P2, pos2);
	std::cout << "Added Position: " << Coord.GetComponent<Position>(P2).x << ", " << Coord.GetComponent<Position>(P2).y << " to Entity : " << P2 << "\n";

	std::cout << "Entity " << P2 << "'s Signature: " << Coord.GetEntitySignature(P2) << "\n";

	// Core Application Loop;
	while (!Core.ShouldClose()) {
		Core.Update();

		static double currTime = glfwGetTime();
		static double prevTime = currTime;
		static double deltaTime = 0.0;

		currTime = glfwGetTime();
		deltaTime += currTime - prevTime;

		if (deltaTime > 2.5) {
			std::cout << "Entity: " << P1 << " Position = {" << Coord.GetComponent<Position>(P1).x << ", " << Coord.GetComponent<Position>(P1).y << "}\n";
			std::cout << "Entity: " << P2 << " Position = {" << Coord.GetComponent<Position>(P2).x << ", " << Coord.GetComponent<Position>(P2).y << "}\n";

			deltaTime = 0;
		}

		prevTime = currTime;
	}

	// Clean termination;
	return 0;
}
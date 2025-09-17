#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#include "AppWindow/Window.h"
#include <iostream>

#include "Core.h"
#include "Systems/AllSystems.h"		// Include your system header in this file;

#if 0
#define TESTING_FRICTION 1
#define TESTING_GRAVITY 0
#endif

int main_testPhysics() {
	// Enable memory leak checking at program exit
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Create Core engine, passing window parameters;
	CoreEngine core(1600, 900, "NightSnack Engine");

	// Will register all components;
	// If you create new components, please register them in this function defined in Core.cpp;
	core.Initialize();

	// Add the systems you want to the Core engine, Must be in specific order taught in GIT lessons;
	// i.e: Input-> Movement -> Collision -> Graphics;
	core.AddSystem<PhysicsSystem>();
	core.AddSystem<MovementSystem>();
	
	Coordinator& coord = core.GetCoord();

	Entity p1 = coord.CreateEntity();
	Entity p2 = coord.CreateEntity();

	coord.AddComponent<Position>(p1, Position{ 10.0, 1000.0 });
	coord.AddComponent<Velocity>(p1, Velocity{ 0.0, 0.0 });
	coord.AddComponent<Acceleration>(p1, Acceleration{ 0.0, 0.0 });
	coord.AddComponent<Mass>(p1, Mass{ 1.0 });
	coord.AddComponent<Force>(p1, Force{ 0.0, 0.0 });
	coord.AddComponent<Collider>(p1, Collider{ 10.0, 10.0, false, true });

	coord.AddComponent<Position>(p2, Position{ 10.0, 1000.0 });
	coord.AddComponent<Velocity>(p2, Velocity{ 0.0, 0.0 });

	while (!core.ShouldClose()) {
		core.Update();

		auto& pos = coord.GetComponent<Position>(p1);
		auto& vel = coord.GetComponent<Velocity>(p1);

		auto& pos2 = coord.GetComponent<Position>(p2);
		auto& vel2 = coord.GetComponent<Velocity>(p2);

		static double currTime = glfwGetTime();
		static double prevTime = currTime;
		static double deltaTime = 0.0;
		static double dt2 = 0.0;

		currTime = glfwGetTime();
		deltaTime += currTime - prevTime;
		dt2 += currTime - prevTime;

		if (dt2 > 5.0) {
#if TESTING_FRICTION
			coord.GetSystem<PhysicsSystem>()->ApplyForce(coord, p1, 100.0, 0.0);
			std::cout << "Added Horizontal Force\n";

#elif TESTING_GRAVITY
			//coord.GetSystem<PhysicsSystem>()->ApplyForce(coord, p1, 0.0, 100.0);
			//std::cout << "Added Vertical Force\n";
#endif
			dt2 = 0.0;
		}
		if (deltaTime > 1.0) {
			std::cout << "Entity: " << p1 << " Position = {" << pos.x << ", " << pos.y << "}\n";
			std::cout << "Entity: " << p1 << " Velocity = {" << vel.dx << ", " << vel.dy << "}\n";
			std::cout << "Entity: " << p2 << " Position = {" << pos2.x << ", " << pos2.y << "}\n";
			std::cout << "Entity: " << p2 << " Velocity = {" << vel2.dx << ", " << vel2.dy << "}\n";
			std::cout << "-------------------------------\n";
			deltaTime = 0;
		}
		prevTime = currTime;
	}

	return 0;
}

int main_testECS()
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

	// Add more systems here...


	// ECS test;
	Coordinator& coord = core.GetCoord();
	Entity p1 = coord.CreateEntity();
	std::cout << "Created entity with ID: " << p1 << "\n";
	Entity p2 = coord.CreateEntity();
	std::cout << "Created entity with ID: " << p2 << "\n";

	std::cout << "Entities currently alive: ";
	for (Entity const& e : coord.GetLivingEntities()) {
		std::cout << e << ", ";
	}
	std::cout << "\n";

	Position pos1{ 0.0, 0.0 };
	coord.AddComponent<Position>(p1, pos1);
	std::cout << "Added Position: " << coord.GetComponent<Position>(p1).x << ", " << coord.GetComponent<Position>(p1).y << " to Entity : " << p1 << "\n";
	Velocity vel1{ 1.0, 1.0 };
	coord.AddComponent<Velocity>(p1, vel1);
	std::cout << "Added Velocity:" << coord.GetComponent<Velocity>(p1).dx << ", " << coord.GetComponent<Velocity>(p1).dy << " to Entity : " << p1 << "\n";

	std::cout << "Entity " << p1 << "'s Signature: " << coord.GetEntitySignature(p1) << "\n";

	Position pos2{ 1.5, 3.2 };
	coord.AddComponent<Position>(p2, pos2);
	std::cout << "Added Position: " << coord.GetComponent<Position>(p2).x << ", " << coord.GetComponent<Position>(p2).y << " to Entity : " << p2 << "\n";

	std::cout << "Entity " << p2 << "'s Signature: " << coord.GetEntitySignature(p2) << "\n";

	// Core Application Loop;
	while (!core.ShouldClose()) {
		core.Update();

		static double currTime = glfwGetTime();
		static double prevTime = currTime;
		static double deltaTime = 0.0;

		currTime = glfwGetTime();
		deltaTime += currTime - prevTime;

		static int counter = 0;

		if (deltaTime > 2.5) {
			std::cout << "Entity: " << p1 << " Position = {" << coord.GetComponent<Position>(p1).x << ", " << coord.GetComponent<Position>(p1).y << "}\n";
			std::cout << "Entity: " << p2 << " Position = {" << coord.GetComponent<Position>(p2).x << ", " << coord.GetComponent<Position>(p2).y << "}\n";

			Entity p4 = coord.CreateEntity();
			coord.CreateEntity();


			std::cout << "Entities currently alive: ";
			for (Entity const& e : coord.GetLivingEntities()) {
				std::cout << e << ", ";
			}
			std::cout << "\n";

			deltaTime = 0;
			coord.DestroyEntity(p4);

		}
		counter++;

		prevTime = currTime;
	}

	// Clean termination;
	return 0;
}
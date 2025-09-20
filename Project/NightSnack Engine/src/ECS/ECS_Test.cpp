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
//#define _CRTDBG_MAP_ALLOC
//#include <cstdlib>
//#include <crtdbg.h>
//
//#include "AppWindow/Window.h"
//#include <iostream>
//
//#include "ECS/Component.h"
//#include "Core.h"
//#include "Systems/AllSystems.h"		// Include your system header in this file;
//
//#include "MathLibrary/Math.h"
//
//int main_testCollisionGraphics() {
//	// Enable memory leak checking at program exit
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//
//	// Create Core engine, passing window parameters;
//	CoreEngine core(1600, 900, "NightSnack Engine");
//
//	core.Initialize();
//
//	core.AddSystem<CollisionSystem>();
//	core.AddSystem<MovementSystem>();
//	core.AddSystem<GraphicsSystem>();
//
//	// ECS test;
//	Coordinator& coord = core.GetCoord();
//	Entity p1 = coord.CreateEntity();
//	Entity p2 = coord.CreateEntity();
//	Entity p3 = coord.CreateEntity();
//	Entity control = coord.CreateEntity();
//
//
//	Collider c1{ ShapeType::Circle, ColliderType::Solid, false, 0.f, 0.f, 100.f };
//	coord.AddComponent<Collider>(p1, c1);
//	Collider c2{ ShapeType::Circle, ColliderType::Solid, false, 0.f, 0.f, 100.f };
//	coord.AddComponent<Collider>(p2, c2);
//	Collider c3{ ShapeType::AABB, ColliderType::Solid, true, 100.f, 400.f, 0.f };
//	coord.AddComponent<Collider>(p3, c3);
//
//	coord.AddComponent<Position>(p1, Position{ 0., 0. });
//	coord.AddComponent<Position>(p2, Position{ 500, 0. });
//	coord.AddComponent<Position>(p3, Position{ -750., 0. });
//
//	coord.AddComponent<Velocity>(p1, Velocity{ 7.5, 0. });
//	coord.AddComponent<Velocity>(p2, Velocity{ 0.0, 0. });
//	coord.AddComponent<Velocity>(p3, Velocity{ 0.0, 0. });
//
//	coord.AddComponent<Scale>(p1, Scale{ 100.0, 100.0 });
//	coord.AddComponent<Scale>(p2, Scale{ 100.0, 100.0 });
//	coord.AddComponent<Scale>(p3, Scale{ 100.0, 400.0 });
//
//	coord.AddComponent<Rotation>(p1, Rotation{});
//	coord.AddComponent<Rotation>(p2, Rotation{});
//	coord.AddComponent<Rotation>(p3, Rotation{});
//
//
//	Mesh rectangle = Mesh::CreateSquare();
//	Mesh circle = Mesh::CreateCircle();
//
//	// Load shader
//	Shader shader("../shaders/vertex_shader.vert", "../shaders/fragment_shader.frag");
//	
//	// Create the different materials
//	Material red(&shader, glm::vec3(1, 0, 0));
//	Material blue(&shader, glm::vec3(0, 0, 1));
//	Material green(&shader, glm::vec3(0, 1, 0));
//
//	Renderable r1{ &circle, &green, false };
//	Renderable r2{ &circle, &blue, false };
//	Renderable r3{ &rectangle, &red, false };
//
//	coord.AddComponent<Renderable>(p1, r1);
//	coord.AddComponent<Renderable>(p2, r2);
//	coord.AddComponent<Renderable>(p3, r3);
//
//	// Camera object
//	Entity camera = coord.CreateEntity();
//	Position posCam{ 0.0, 0.0 };
//	Rotation rotCam{ 0.f };
//	Scale scaleCam{ 200.0,200.0 };
//	coord.AddComponent<Position>(camera, posCam);
//	coord.AddComponent<Rotation>(camera, rotCam);
//	coord.AddComponent<Scale>(camera, scaleCam);
//	Camera isCamera{
//		true,
//		1.f,
//		1600.f,
//		900.f
//	};
//	coord.AddComponent<Camera>(camera, isCamera);
//
//	while (!core.ShouldClose()) {
//		core.Update();
//
//		//static double currTime = glfwGetTime();
//		//static double prevTime = currTime;
//		//static double deltaTime = 0.0;
//
//		//currTime = glfwGetTime();
//		//deltaTime += currTime - prevTime;
//
//		//if (deltaTime > 2.5) {
//		//	std::cout << "Entity: " << p1 << " Position = {" << coord.GetComponent<Position>(p1).x << ", " << coord.GetComponent<Position>(p1).y << "}\n";
//		//	std::cout << "Entity: " << p2 << " Position = {" << coord.GetComponent<Position>(p2).x << ", " << coord.GetComponent<Position>(p2).y << "}\n";
//
//		//	Entity p4 = coord.CreateEntity();
//		//	coord.CreateEntity();
//
//
//		//	std::cout << "Entities currently alive: ";
//		//	for (Entity const& e : coord.GetLivingEntities()) {
//		//		std::cout << e << ", ";
//		//	}
//		//	std::cout << "\n";
//
//		//	deltaTime = 0;
//		//	coord.DestroyEntity(p4);
//
//		//}
//		//prevTime = currTime;
//	}
//	return 0;
//}
//
//int main_ECStest()
//{
//	// Enable memory leak checking at program exit
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//
//	// Create Core engine, passing window parameters;
//	CoreEngine core(1600, 900, "NightSnack Engine");
//
//	// Will register all components;
//	// If you create new components, please register them in this function defined in Core.cpp;
//	core.Initialize();
//
//	// Add the systems you want to the Core engine, Must be in specific order taught in GIT lessons;
//	// i.e: Input-> Movement -> Collision -> Graphics;
//	core.AddSystem<MovementSystem>();
//
//	// Add more systems here...
//
//
//	// ECS test;
//	Coordinator& coord = core.GetCoord();
//	Entity p1 = coord.CreateEntity();
//	std::cout << "Created entity with ID: " << p1 << "\n";
//	Entity p2 = coord.CreateEntity();
//	std::cout << "Created entity with ID: " << p2 << "\n";
//
//	std::cout << "Entities currently alive: ";
//	for (Entity const& e : coord.GetLivingEntities()) {
//		std::cout << e << ", ";
//	}
//	std::cout << "\n";
//
//	Position pos1{ 0.0, 0.0 };
//	coord.AddComponent<Position>(p1, pos1);
//	std::cout << "Added Position: " << coord.GetComponent<Position>(p1).x << ", " << coord.GetComponent<Position>(p1).y << " to Entity : " << p1 << "\n";
//	Velocity vel1{ 1.0, 1.0 };
//	coord.AddComponent<Velocity>(p1, vel1);
//	std::cout << "Added Velocity:" << coord.GetComponent<Velocity>(p1).dx << ", " << coord.GetComponent<Velocity>(p1).dy << " to Entity : " << p1 << "\n";
//
//	std::cout << "Entity " << p1 << "'s Signature: " << coord.GetEntitySignature(p1) << "\n";
//
//	Position pos2{ 1.5, 3.2 };
//	coord.AddComponent<Position>(p2, pos2);
//	std::cout << "Added Position: " << coord.GetComponent<Position>(p2).x << ", " << coord.GetComponent<Position>(p2).y << " to Entity : " << p2 << "\n";
//
//	std::cout << "Entity " << p2 << "'s Signature: " << coord.GetEntitySignature(p2) << "\n";
//
//	// Core Application Loop;
//	while (!core.ShouldClose()) {
//		core.Update();
//
//		static double currTime = glfwGetTime();
//		static double prevTime = currTime;
//		static double deltaTime = 0.0;
//
//		currTime = glfwGetTime();
//		deltaTime += currTime - prevTime;
//
//		static int counter = 0;
//
//		if (deltaTime > 2.5) {
//			std::cout << "Entity: " << p1 << " Position = {" << coord.GetComponent<Position>(p1).x << ", " << coord.GetComponent<Position>(p1).y << "}\n";
//			std::cout << "Entity: " << p2 << " Position = {" << coord.GetComponent<Position>(p2).x << ", " << coord.GetComponent<Position>(p2).y << "}\n";
//
//			Entity p4 = coord.CreateEntity();
//			coord.CreateEntity();
//
//
//			std::cout << "Entities currently alive: ";
//			for (Entity const& e : coord.GetLivingEntities()) {
//				std::cout << e << ", ";
//			}
//			std::cout << "\n";
//
//			deltaTime = 0;
//			coord.DestroyEntity(p4);
//
//		}
//		counter++;
//
//		prevTime = currTime;
//	}
//
//	// Clean termination;
//	return 0;
//}
//
//int main_Mathtest()
//{
//	// Enable memory leak checking at program exit
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//
//	// Scalar Math
//	std::cout << "=== Scalar Math Tests ===\n";
//	std::cout << "add(3, 4) = " << add(3, 4) << " (expected 7)\n";
//	std::cout << "subtract(10, 3) = " << subtract(10, 3) << " (expected 7)\n";
//	std::cout << "multiply(6, 5) = " << multiply(6, 5) << " (expected 30)\n";
//	std::cout << "divide(20, 4) = " << divide(20, 4) << " (expected 5)\n";
//
//	try {
//		std::cout << "divide(10, 0) = ";
//		std::cout << divide(10, 0) << "\n"; // should throw
//	}
//	catch (const std::exception& e) {
//		std::cout << "caught exception: " << e.what() << "\n";
//	}
//
//	// Vec2
//	std::cout << "\n=== Vec2 Tests ===\n";
//	Vec2 v1(3, 4);
//	Vec2 v2(1, 2);
//
//	std::cout << "v1 = " << v1 << ", v2 = " << v2 << "\n";
//	std::cout << "v1 + v2 = " << (v1 + v2) << " (expected (4, 6))\n";
//	std::cout << "v1 - v2 = " << (v1 - v2) << " (expected (2, 2))\n";
//	std::cout << "v1 * 2 = " << (v1 * 2) << " (expected (6, 8))\n";
//	std::cout << "v1 / 2 = " << (v1 / 2) << " (expected (1.5, 2))\n";
//	std::cout << "v1 dot v2 = " << v1.dot(v2) << " (expected 11)\n";
//	std::cout << "v1 cross v2 = " << v1.cross(v2) << " (expected 2)\n";
//	std::cout << "v1 magnitude = " << v1.magnitude() << " (expected 5)\n";
//
//	// Vec3
//	std::cout << "\n=== Vec3 Tests ===\n";
//	Vec3 a(1, 0, 0);
//	Vec3 b(0, 1, 0);
//
//	std::cout << "a = " << a << ", b = " << b << "\n";
//	std::cout << "a + b = " << (a + b) << " (expected (1, 1, 0))\n";
//	std::cout << "a - b = " << (a - b) << " (expected (1, -1, 0))\n";
//	std::cout << "a dot b = " << a.dot(b) << " (expected 0)\n";
//	std::cout << "a cross b = " << (a ^ b) << " (expected (0, 0, 1))\n";
//	std::cout << "a magnitude = " << a.magnitude() << " (expected 1)\n";
//
//	// Mat3
//	std::cout << "\n=== Mat3 Tests ===\n";
//	Mat3 m1({ { {1,2,3}, {4,5,6}, {7,8,9} } });
//	Mat3 m2({ { {9,8,7}, {6,5,4}, {3,2,1} } });
//
//	std::cout << "m1:\n" << m1;
//	std::cout << "m2:\n" << m2;
//	std::cout << "m1 + m2:\n" << (m1 + m2);
//	std::cout << "m1 - m2:\n" << (m1 - m2);
//	std::cout << "m1 * 2:\n" << (m1 * 2);
//	std::cout << "m1 transpose:\n" << m1.transpose();
//	std::cout << "det(m1) = " << m1.det() << " (expected 0)\n"; // singular
//
//	Mat3 id({ { {1,0,0}, {0,1,0}, {0,0,1} } });
//	std::cout << "m1 * identity =\n" << (m1 * id);
//
//	// Angle
//	std::cout << "\n=== Angle Tests ===\n";
//	Angle angle = Angle::fromDegrees(90);
//	std::cout << "90 degrees in radians = " << angle.radians << "\n";
//	std::cout << "toDegrees() = " << angle.toDegrees() << " (expected 90)\n";
//	std::cout << "sin(90) = " << angle.sin() << " (expected ~1)\n";
//	std::cout << "cos(90) = " << angle.cos() << " (expected ~0)\n";
//
//	Angle rad45(deg2rad(45));
//	std::cout << "tan(45) = " << rad45.tan() << " (expected ~1)\n";
//
//
//	// Create Core engine, passing window parameters;
//	CoreEngine core(1600, 900, "NightSnack Engine");
//
//	// Will register all components;
//	// If you create new components, please register them in this function defined in Core.cpp;
//	core.Initialize();
//
//	// Add the systems you want to the Core engine, Must be in specific order taught in GIT lessons;
//	// i.e: Input-> Movement -> Collision -> Graphics;
//	core.AddSystem<MovementSystem>();
//
//	// Add more systems here...
//
//
//	// Clean termination;
//	return 0;
//}

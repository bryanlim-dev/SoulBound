#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#include "AppWindow/Window.h"
#include "MathLibrary/math.h"
#include <iostream>

int main()
{
  // Enable memory leak checking at program exit
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    /*
    // ========================= Scalar Math =========================
    std::cout << "=== Scalar Math Tests ===\n";
    std::cout << "add(3, 4) = " << add(3, 4) << " (expected 7)\n";
    std::cout << "subtract(10, 3) = " << subtract(10, 3) << " (expected 7)\n";
    std::cout << "multiply(6, 5) = " << multiply(6, 5) << " (expected 30)\n";
    std::cout << "divide(20, 4) = " << divide(20, 4) << " (expected 5)\n";

    try {
        std::cout << "divide(10, 0) = ";
        std::cout << divide(10, 0) << "\n"; // should throw
    }
    catch (const std::exception& e) {
        std::cout << "caught exception: " << e.what() << "\n";
    }

    // ========================= Vec2 =========================
    std::cout << "\n=== Vec2 Tests ===\n";
    Vec2 v1(3, 4);
    Vec2 v2(1, 2);

    std::cout << "v1 = " << v1 << ", v2 = " << v2 << "\n";
    std::cout << "v1 + v2 = " << (v1 + v2) << " (expected (4, 6))\n";
    std::cout << "v1 - v2 = " << (v1 - v2) << " (expected (2, 2))\n";
    std::cout << "v1 * 2 = " << (v1 * 2) << " (expected (6, 8))\n";
    std::cout << "v1 / 2 = " << (v1 / 2) << " (expected (1.5, 2))\n";
    std::cout << "v1 dot v2 = " << v1.dot(v2) << " (expected 11)\n";
    std::cout << "v1 cross v2 = " << v1.cross(v2) << " (expected 2)\n";
    std::cout << "v1 magnitude = " << v1.magnitude() << " (expected 5)\n";

    // ========================= Vec3 =========================
    std::cout << "\n=== Vec3 Tests ===\n";
    Vec3 a(1, 0, 0);
    Vec3 b(0, 1, 0);

    std::cout << "a = " << a << ", b = " << b << "\n";
    std::cout << "a + b = " << (a + b) << " (expected (1, 1, 0))\n";
    std::cout << "a - b = " << (a - b) << " (expected (1, -1, 0))\n";
    std::cout << "a dot b = " << a.dot(b) << " (expected 0)\n";
    std::cout << "a cross b = " << (a ^ b) << " (expected (0, 0, 1))\n";
    std::cout << "a magnitude = " << a.magnitude() << " (expected 1)\n";

    // ========================= Mat3 =========================
    std::cout << "\n=== Mat3 Tests ===\n";
    Mat3 m1({ { {1,2,3}, {4,5,6}, {7,8,9} } });
    Mat3 m2({ { {9,8,7}, {6,5,4}, {3,2,1} } });

    std::cout << "m1:\n" << m1;
    std::cout << "m2:\n" << m2;
    std::cout << "m1 + m2:\n" << (m1 + m2);
    std::cout << "m1 - m2:\n" << (m1 - m2);
    std::cout << "m1 * 2:\n" << (m1 * 2);
    std::cout << "m1 transpose:\n" << m1.transpose();
    std::cout << "det(m1) = " << m1.det() << " (expected 0)\n"; // singular

    Mat3 id({ { {1,0,0}, {0,1,0}, {0,0,1} } });
    std::cout << "m1 * identity =\n" << (m1 * id);

    // ========================= Angle =========================
    std::cout << "\n=== Angle Tests ===\n";
    Angle angle = Angle::fromDegrees(90);
    std::cout << "90 degrees in radians = " << angle.radians << "\n";
    std::cout << "toDegrees() = " << angle.toDegrees() << " (expected 90)\n";
    std::cout << "sin(90) = " << angle.sin() << " (expected ~1)\n";
    std::cout << "cos(90) = " << angle.cos() << " (expected ~0)\n";

    Angle rad45(deg2rad(45));
    std::cout << "tan(45) = " << rad45.tan() << " (expected ~1)\n";
    */

	// Game loop;
	while (!window.ShouldClose()) {
		glClear(GL_COLOR_BUFFER_BIT);

  // Will register all components;
  // If you create new components, please register them in this function defined in Core.cpp;
  core.Initialize();

  // Add the systems you want to the Core engine, Must be in specific order taught in GIT lessons;
  // i.e: Input-> Movement -> Collision -> Graphics;
  core.AddSystem<MovementSystem>();
  core.AddSystem<GraphicsSystem>();

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
  return 0;
}
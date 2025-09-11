#include "AppWindow/Window.h"
#include "MathLibrary/mathLibrary.h"
#include <iostream>

int main()
{
	// Create window (width, height, title);
	Window window(1600, 900, "NightSnack Engine");
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    /*
    // ---- Scalar math ----
    double a = 10, b = 5;
    std::cout << "Scalars: a=" << a << ", b=" << b << "\n";
    std::cout << "a + b = " << add(a, b) << "\n";
    std::cout << "a - b = " << subtract(a, b) << "\n";
    std::cout << "a * b = " << multiply(a, b) << "\n";
    std::cout << "a / b = " << divide(a, b) << "\n\n";

    // ---- Vec2 ----
    Vec2 v2a(1, 2), v2b(3, 4);
    std::cout << "Vec2 a = " << v2a << "\n";
    std::cout << "Vec2 b = " << v2b << "\n";
    std::cout << "a + b = " << (v2a + v2b) << "\n";
    std::cout << "a - b = " << (v2a - v2b) << "\n";
    std::cout << "a * 2 = " << (v2a * 2) << "\n";
    std::cout << "b / 2 = " << (v2b / 2) << "\n";
    std::cout << "a . b = " << (v2a * v2b) << "\n";
    std::cout << "a x b = " << (v2a ^ v2b) << "\n\n";

    // ---- Vec3 ----
    Vec3 v3a(1, 0, 0), v3b(0, 1, 0);
    std::cout << "Vec3 a = " << v3a << "\n";
    std::cout << "Vec3 b = " << v3b << "\n";
    std::cout << "a + b = " << (v3a + v3b) << "\n";
    std::cout << "a - b = " << (v3a - v3b) << "\n";
    std::cout << "a * 2 = " << (v3a * 2) << "\n";
    std::cout << "b / 2 = " << (v3b / 2) << "\n";
    std::cout << "a . b = " << (v3a * v3b) << "\n";
    std::cout << "a x b = " << (v3a ^ v3b) << "\n";
    */

	// Game loop;
	while (!window.ShouldClose()) {
		glClear(GL_COLOR_BUFFER_BIT);

		window.SwapBuffers();
		window.PollEvents();
	}

	// Clean termination;
  return 0;
}
#include "AppWindow/Window.h"

int main()
{
	// Create window (width, height, title);
	Window window(1600, 900, "NightSnack Engine");
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	// Game loop;
	while (!window.ShouldClose()) {
		glClear(GL_COLOR_BUFFER_BIT);
		// CoreEngine.Update();
		window.SwapBuffers();
		window.PollEvents();
	}

	// Clean termination;
  return 0;
}
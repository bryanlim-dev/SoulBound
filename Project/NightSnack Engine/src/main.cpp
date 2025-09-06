#include <Window.h>

#include "ResourceManager/ResourceManager.h"


int main()
{
	// Create window (width, height, title);
	Window window(1600, 900, "NightSnack Engine");
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);


	ResourceManager* rm = ResourceManager::GetInstance();
	rm->LoadImage("assets/Banana.jpg");
	rm->LoadImage("assets/image_UI_target.png");

	rm->DeleteRM();

	// Game loop;
	while (!window.ShouldClose()) {
		glClear(GL_COLOR_BUFFER_BIT);

		window.SwapBuffers();
		window.PollEvents();
	}

	// Clean termination;
  return 0;
}
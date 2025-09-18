#include "AppWindow/Window.h"

#include "ResourceManager/ResourceManager.h"
#include <fmod.hpp>


int main()
{
	// Create window (width, height, title);
	Window window(1600, 900, "NightSnack Engine");
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	ResourceManager* rm = ResourceManager::GetInstance();

	//Resource Manager test
	{
		rm->DebugTestEverything();
	}

	//Replace this with actual sound manager later
	FMOD::System* fmodSystem = nullptr;
	FMOD::System_Create(&fmodSystem);



	//std::cout << "Image loaded: " << rm->GetImage("assets/Banana.jpg") << std::endl;

	// Game loop;
	while (!window.ShouldClose()) {
		glClear(GL_COLOR_BUFFER_BIT);

		window.SwapBuffers();
		window.PollEvents();

		rm->Update(0);
	}

	// Clean termination;

	rm->DeleteRM();
	fmodSystem->close();
  return 0;
}
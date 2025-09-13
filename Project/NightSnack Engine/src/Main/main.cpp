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
		rm->LoadImage("assets/Banana.jpg"); //Testing a loading jpg
		rm->LoadImage("assets/image_UI_target.png"); //Testing loading png
		rm->UnloadImage("assets/image_UI_target.png");

		rm->LoadImage("assets/image_UI_target.png"); //Testing loading a loaded image with deletion flagged
		rm->LoadImage("assets/image_UI_target.png"); //Testing loading same image
		rm->UpdatePostRender(); //Run unload code
		rm->UnloadImage("assets/image_UI_target.png"); //Actual unloading test
		rm->UpdatePostRender(); //Run unload code
		rm->LoadImage("assets/image_UI_target.png"); //Reloading a unloaded image test
	}

	//Replace this with actual sound manager later
	FMOD::System* system = nullptr;
	FMOD::System_Create(&system);



	//std::cout << "Image loaded: " << rm->GetImage("assets/Banana.jpg") << std::endl;

	// Game loop;
	while (!window.ShouldClose()) {
		glClear(GL_COLOR_BUFFER_BIT);

		window.SwapBuffers();
		window.PollEvents();

		rm->Update();

		
		rm->UpdatePostRender();
	}

	// Clean termination;

	rm->DeleteRM();
  return 0;
}
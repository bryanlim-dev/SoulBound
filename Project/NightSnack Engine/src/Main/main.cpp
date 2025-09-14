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
		std::type_index bananaKey = typeid(void);
		std::type_index targetKey = typeid(void);
		std::cout << "Test Load Images" << std::endl;
		rm->LoadImage("assets/Banana.jpg", bananaKey); //Testing a loading jpg
		rm->LoadImage("assets/image_UI_target.png", targetKey); //Testing loading png
		rm->UnloadImage(targetKey);
		rm->UpdatePostRender();
		if (rm->GetImage(targetKey) == nullptr)
		{
			std::cout << "Get Image did not find resource" << std::endl;
		}

		rm->LoadImage("assets/image_UI_target.png", targetKey); //Testing loading a loaded image with deletion flagged
		rm->LoadImage("assets/image_UI_target.png", targetKey); //Testing loading same image
		rm->UpdatePostRender(); //Run unload code
		rm->UnloadImage(targetKey); //Actual unloading test
		rm->UpdatePostRender(); //Run unload code
		rm->LoadImage("assets/image_UI_target.png", targetKey); //Reloading a unloaded image test
		rm->GetImage(targetKey);
		rm->GetImage(bananaKey);
		if (rm->GetImage(targetKey) != nullptr)
		{
			std::cout << "Get Image found target resource" << std::endl;
		}
		if (rm->GetImage(bananaKey) != nullptr)
		{
			std::cout << "Get Image found banana resource" << std::endl;
		}

		std::cout << std::endl;

		std::cout << "Test Load Txt File" << std::endl;

		std::type_index txtKey = typeid(void);
		rm->LoadTxtFile("assets/data/TestTxt.txt", txtKey);
		std::vector<std::string> txtVec = rm->GetTxtFile(txtKey);
		for (std::string strTest : txtVec)
		{
			std::cout << strTest << std::endl;
		}
		std::cout << std::endl;
		rm->UnloadTxtFile(txtKey); //Actual unloading test
		rm->UpdatePostRender(); //Run unload code

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

		rm->Update();

		
		rm->UpdatePostRender();
	}

	// Clean termination;

	rm->DeleteRM();
	fmodSystem->close();
  return 0;
}
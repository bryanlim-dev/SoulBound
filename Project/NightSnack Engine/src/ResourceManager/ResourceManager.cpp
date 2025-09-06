#include "ResourceManager.h"



void ResourceManager::LoadImage(std::string filePath)
{
	ResourceImage* newResource = new ResourceImage(filePath);

	if (newResource->Loaded())
	{
		std::cout << "Failed to load image" << std::endl;
	}
	else
	{
		std::cout << "Loaded image: " << newResource->Path() << std::endl;
	}

	delete newResource;
}


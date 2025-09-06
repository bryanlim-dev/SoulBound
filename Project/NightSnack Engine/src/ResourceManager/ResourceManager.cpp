#include "ResourceManager.h"


ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{

	//Delete all resource


	//Loop through entire map AND DELETE IT MUHAHA
	std::map<std::string, Resource*>::iterator it;
	for (it = resourceMap.begin(); it != resourceMap.end(); it++)
	{
		delete it->second;
	}
	resourceMap.clear();
}



void ResourceManager::LoadImage(std::string filePath)
{
	ResourceImage* newResource = new ResourceImage(filePath);

	std::cout << "RM: ";

	if (!newResource->Loaded())
	{
		std::cout << "Failed to load image" << std::endl;
	}
	else
	{
		std::cout << "Loaded image: " << newResource->Path() << std::endl;
	}

	resourceMap.insert({filePath, newResource});
}

void ResourceManager::DeleteRM()
{
	delete this;
}


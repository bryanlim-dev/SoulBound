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
	std::cout << "RM: ";

	//Check if resource alr exists
	if (resourceMap.find(filePath) != resourceMap.end())
	{
		std::cout << filePath << " already exists" << std::endl;
		return;
	}

	ResourceImage* newResource = new ResourceImage(filePath);


	if (!newResource->Loaded())
	{
		std::cout << "Failed to load image" << std::endl;
	}
	else
	{
		std::cout << "Loaded image " << newResource->Path() << std::endl;
	}

	resourceMap.insert({filePath, newResource});
}

void ResourceManager::DeleteRM()
{
	delete this;
}


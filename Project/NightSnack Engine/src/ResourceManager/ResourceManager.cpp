#include "ResourceManager.h"


ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{

	//Delete all resource


	//Loop through entire image map AND DELETE IT MUHAHA
	std::map<std::string, ResourceImage*>::iterator it;
	for (it = resourceImageMap.begin(); it != resourceImageMap.end(); it++)
	{
		delete it->second;
	}
	resourceImageMap.clear();
}

void ResourceManager::Update()
{
}



bool ResourceManager::LoadImage(std::string filePath)
{
	std::cout << "RM: ";

	//Check if resource alr exists
	if (resourceImageMap.find(filePath) != resourceImageMap.end())
	{
		std::cout << filePath << " already exists" << std::endl;
		return true;
	}

	//This loads the image via the ResourceImage class
	ResourceImage* newResource = new ResourceImage(filePath);

	//Insert the loaded image into the map
	resourceImageMap.insert({filePath, newResource});

	//This checks if it is loaded or now
	if (!newResource->Loaded())
	{
		std::cout << "Failed to load image" << std::endl;
		return false;
	}
	else
	{
		std::cout << "Loaded image " << newResource->Path() << std::endl;
		return true;
	}

}

void ResourceManager::UnloadImage(std::string filePath)
{
	//Check if resource alr exists
	if (resourceImageMap.find(filePath) == resourceImageMap.end())
	{
		std::cout << filePath << " does not exist" << std::endl;
		return;
	}

	//Change this to a queue system
	delete resourceImageMap[filePath];
	resourceImageMap[filePath] = nullptr;
	resourceImageMap.erase(filePath);
}

unsigned char* ResourceManager::GetImage(const std::string filePath)
{
	//Same logic as get image resource so just reuse it
	return GetImageResource(filePath)->Image();
}

ResourceImage* ResourceManager::GetImageResource(std::string filePath)
{
	//Does the resource exist?
	if (resourceImageMap.find(filePath) != resourceImageMap.end())
	{
		return resourceImageMap[filePath];
	}
	//Try to load the image if not loaded yet
	else if (LoadImage(filePath))
	{
		return resourceImageMap[filePath];
	}

	//Return nullptr if failed to load
	return nullptr;
}

void ResourceManager::DeleteRM()
{
	delete this;
}


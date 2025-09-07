#include "ResourceManager.h"


ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
	//Delete all resources

	//Loop through entire image map AND DELETE IT MUHAHA
	std::map<std::string, ResourceImage*>::iterator it;
	for (it = resourceImageMap.begin(); it != resourceImageMap.end(); it++)
	{
		delete it->second;
	}
	resourceImageMap.clear();
	deletionVec.clear();
}

void ResourceManager::Update()
{
}

void ResourceManager::UpdatePostRender()
{
	DeleteFlaggedResources();
}



bool ResourceManager::LoadImage(std::string filePath)
{
	std::cout << "RM: ";

	//Check if resource alr exists
	if (resourceImageMap.find(filePath) != resourceImageMap.end())
	{
		std::cout << filePath << " already exists" << std::endl;

		//Check if it was flagged for deletion
		if (resourceImageMap[filePath]->FlaggedForDeletion())
		{
			//Remove the flag if it was flagged for deletion
			//(This will cause it to not delete in post render)
			resourceImageMap[filePath]->FlaggedForDeletion() = false;
		}

		return true;
	}

	//This loads the image via the ResourceImage class
	ResourceImage* newResource = new ResourceImage(filePath);


	//This checks if it is loaded or now
	if (!newResource->Loaded())
	{
		std::cout << "Failed to load image" << std::endl;

		if (newResource)
			delete newResource;

		return false;
	}
	else
	{
		std::cout << "Loaded image " << newResource->Path() << std::endl;

		//Insert the loaded image into the map only if managed to load
		resourceImageMap.insert({filePath, newResource});
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

	resourceImageMap[filePath]->FlaggedForDeletion() = true;

	//Change this to a queue system
	deletionVec.push_back(resourceImageMap[filePath]);
	//delete resourceImageMap[filePath];
	//resourceImageMap[filePath] = nullptr;
	//resourceImageMap.erase(filePath);
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

void ResourceManager::DeleteFlaggedResources()
{
	for (Resource* resource : deletionVec)
	{
		//If it was unflagged somewhere inbetween
		if (!resource->FlaggedForDeletion())
			continue;

		switch (resource->FileType())
		{
		case RESOURCE_IMAGE:
			resourceImageMap.erase(resource->Path());
			break;
		}
		delete resource;
	}

	deletionVec.clear();
}


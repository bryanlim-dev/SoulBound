#include "ResourceManager.h"


ResourceManager::ResourceManager()
{
	totalResourcesLoaded = 0;
}

ResourceManager::~ResourceManager()
{
	//Delete all resources

	//Loop through entire image map AND DELETE IT MUHAHA
	std::map<std::type_index, ResourceImage*>::iterator it;
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

#pragma region Image Functions
LOAD_RESULT ResourceManager::LoadImage(std::string filePath, std::type_index &imageKey)
{
	std::cout << "RM: ";

	
	imageKey = ConstructMapKey(filePath);

	//Check if resource alr exists
	if (resourceImageMap.find(imageKey) != resourceImageMap.end())
	{
		std::cout << filePath << " already exists" << std::endl;

		//Check if it was flagged for deletion
		if (resourceImageMap[imageKey]->FlaggedForDeletion())
		{
			//Remove the flag if it was flagged for deletion
			//(This will cause it to not delete in post render)
			resourceImageMap[imageKey]->FlaggedForDeletion() = false;
			return LOAD_SUCCESS_STOPPED_UNLOAD;
		}
		else
			return LOAD_FAIL_PREEXIST;
	}

	//This loads the image via the ResourceImage class
	ResourceImage* newResource = new ResourceImage(filePath, imageKey);


	//This checks if it is loaded or now
	if (!newResource->Loaded())
	{
		std::cout << "Failed to load image" << std::endl;

		if (newResource)
			delete newResource;

		return LOAD_FAIL_BADPATH;
	}
	else
	{
		std::cout << "Loaded image " << newResource->Path() << std::endl;

		//Insert the loaded image into the map only if managed to load
		resourceImageMap.insert({imageKey, newResource});
		totalResourcesLoaded++;
		return LOAD_SUCCESS;
	}

}

void ResourceManager::UnloadImage(std::type_index imageKey)
{
	//Check if resource alr exists
	if (resourceImageMap.find(imageKey) == resourceImageMap.end())
	{
		std::cout << "Resource does not exist, cannot unload" << std::endl;
		return;
	}

	resourceImageMap[imageKey]->FlaggedForDeletion() = true;

	//Change this to a queue system
	deletionVec.push_back(resourceImageMap[imageKey]);
	//delete resourceImageMap[filePath];
	//resourceImageMap[filePath] = nullptr;
	//resourceImageMap.erase(filePath);
}


unsigned char* ResourceManager::GetImage(const std::type_index imageKey)
{
	//Same logic as func get image resource so just reuse it
	ResourceImage* image = GetImageResource(imageKey);
	return (image != nullptr) ? GetImageResource(imageKey)->Image(): nullptr;
}

ResourceImage* ResourceManager::GetImageResource(const std::type_index imageKey)
{
	//Does the resource exist?
	if (resourceImageMap.find(imageKey) != resourceImageMap.end())
	{
		return resourceImageMap[imageKey];
	}
	//Try to load the image if not loaded yet
	//else if (LoadImage(filePath))
	//{
	//	return resourceImageMap[filePath];
	//}

	//Return nullptr if failed to load
	return nullptr;
}

#pragma endregion

#pragma region Txt File Functions
#pragma endregion

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
			resourceImageMap.erase(resource->MapKey());
			break;
		}
		delete resource;
	}

	deletionVec.clear();
}


std::type_index ResourceManager::ConstructMapKey(std::string filePath)
{
	size_t lastSlashPos = filePath.find_last_of('/');
	if (lastSlashPos == filePath.npos)
	{
		lastSlashPos = filePath.find_last_of('\\');
		if (lastSlashPos == filePath.npos)
		{
			lastSlashPos = 0;
		}
	}

	filePath = filePath.substr(lastSlashPos, filePath.size() - lastSlashPos);



	return typeid(filePath);
}
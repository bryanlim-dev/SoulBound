#include "ResourceManager.h"


ResourceManager::ResourceManager()
{
	totalResourcesLoaded = 0;
}

ResourceManager::~ResourceManager()
{
	//Delete all resources

	//Loop through entire image map AND DELETE IT MUHAHA
	std::map<std::type_index, ResourceImage*>::iterator itImg;
	for (itImg = resourceImageMap.begin(); itImg != resourceImageMap.end(); itImg++)
	{
		delete itImg->second;
	}
	resourceImageMap.clear();

	//Delete txt map
	std::map<std::type_index, ResourceTxtFile*>::iterator itTxt;
	for (itTxt = resourceTxtFileMap.begin(); itTxt != resourceTxtFileMap.end(); itTxt++)
	{
		delete itTxt->second;
	}
	resourceTxtFileMap.clear();

	deletionVec.clear();
}

void ResourceManager::Update(double dt)
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
			//(This will cause it to not delete in update)
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



LOAD_RESULT ResourceManager::LoadTxtFile(std::string filePath, std::type_index& txtKey)
{
	std::cout << "RM: ";

	txtKey = ConstructMapKey(filePath);

	//Check if resource alr exists
	if (resourceTxtFileMap.find(txtKey) != resourceTxtFileMap.end())
	{
		std::cout << filePath << " already exists" << std::endl;

		//Check if it was flagged for deletion
		if (resourceTxtFileMap[txtKey]->FlaggedForDeletion())
		{
			//Remove the flag if it was flagged for deletion
			//(This will cause it to not delete in update)
			resourceTxtFileMap[txtKey]->FlaggedForDeletion() = false;
			return LOAD_SUCCESS_STOPPED_UNLOAD;
		}
		else
			return LOAD_FAIL_PREEXIST;
	}

	//This loads the image via the ResourceImage class
	ResourceTxtFile* newResource = new ResourceTxtFile(filePath, txtKey);

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
		resourceTxtFileMap.insert({ txtKey, newResource });
		totalResourcesLoaded++;
		return LOAD_SUCCESS;
	}

	return LOAD_RESULT();
}

void ResourceManager::UnloadTxtFile(std::type_index txtKey)
{
	//Check if resource alr exists
	if (resourceTxtFileMap.find(txtKey) == resourceTxtFileMap.end())
	{
		std::cout << "Resource does not exist, cannot unload" << std::endl;
		return;
	}

	resourceTxtFileMap[txtKey]->FlaggedForDeletion() = true;

	//Change this to a queue system
	deletionVec.push_back(resourceTxtFileMap[txtKey]);
}

std::vector<std::string> ResourceManager::GetTxtFile(const std::type_index txtKey)
{
	ResourceTxtFile* txt = GetTxtFileResource(txtKey);
	return (txt != nullptr) ? txt->TextStringVec() : std::vector<std::string>();
}

ResourceTxtFile* ResourceManager::GetTxtFileResource(const std::type_index txtKey)
{
	//Does the resource exist?
	if (resourceImageMap.find(txtKey) != resourceImageMap.end())
	{
		return resourceTxtFileMap[txtKey];
	}

	return nullptr;
}
#pragma endregion

void ResourceManager::DeleteRM()
{
	delete this;
}

void ResourceManager::DebugEverything()
{
	ResourceManager* rm = this;

	std::type_index bananaKey = typeid(void);
	std::type_index targetKey = typeid(void);
	std::cout << "Test Load Images" << std::endl;
	rm->LoadImage("assets/Banana.jpg", bananaKey); //Testing a loading jpg
	rm->LoadImage("assets/image_UI_target.png", targetKey); //Testing loading png
	rm->UnloadImage(targetKey);
	rm->Update(0);
	if (rm->GetImage(targetKey) == nullptr)
	{
		std::cout << "Get Image did not find resource" << std::endl;
	}

	rm->LoadImage("assets/image_UI_target.png", targetKey); //Testing loading a loaded image with deletion flagged
	rm->LoadImage("assets/image_UI_target.png", targetKey); //Testing loading same image
	rm->Update(0); //Run unload code
	rm->UnloadImage(targetKey); //Actual unloading test
	rm->Update(0); //Run unload code
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
	rm->Update(0); //Run unload code

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
		case RESOURCE_TXTFILE:
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
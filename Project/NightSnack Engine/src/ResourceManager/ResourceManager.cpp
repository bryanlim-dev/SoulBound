#include "ResourceManager.h"
#include <stb_image.h>


ResourceManager::ResourceManager()
{
	totalResourcesLoaded = 0;
	stbi_set_flip_vertically_on_load(true); // Flip the y-axis of the images loaded
}

ResourceManager::~ResourceManager()
{
	//Delete all resources

	//Loop through entire image map AND DELETE IT MUHAHA
	std::map<std::size_t, ResourceImage*>::iterator itImg;
	for (itImg = resourceImageMap.begin(); itImg != resourceImageMap.end(); itImg++)
	{
		delete itImg->second;
	}
	resourceImageMap.clear();

	//Delete txt map
	std::map<std::size_t, ResourceTxtFile*>::iterator itTxt;
	for (itTxt = resourceTxtFileMap.begin(); itTxt != resourceTxtFileMap.end(); itTxt++)
	{
		delete itTxt->second;
	}
	resourceTxtFileMap.clear();

	//Delete json map
	std::map<std::size_t, ResourceJson*>::iterator itJson;
	for (itJson = resourceJsonMap.begin(); itJson != resourceJsonMap.end(); itJson++)
	{
		delete itJson->second;
	}
	resourceJsonMap.clear();

	//Delete Sound map
	std::map<std::size_t, ResourceSound*>::iterator itSound;
	for (itSound = resourceSoundMap.begin(); itSound != resourceSoundMap.end(); itSound++)
	{
		delete itSound->second;
	}
	resourceSoundMap.clear();

	deletionVec.clear();
}

void ResourceManager::Update(float /*dt*/)
{
	DeleteFlaggedResources();
}



#pragma region Image Functions
LOAD_RESULT ResourceManager::LoadImage(std::string filePath, std::size_t&imageKey)
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
	newResource->LoadFile();

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

void ResourceManager::UnloadImage(std::size_t imageKey)
{
	//Check if resource alr exists
	if (resourceImageMap.find(imageKey) == resourceImageMap.end())
	{
		std::cout << "Resource does not exist, cannot unload" << std::endl;
		return;
	}

	resourceImageMap[imageKey]->FlaggedForDeletion() = true;

	//Change this to a queue system
	//Check if it exists witin the vec alr tho
	auto it = std::find(deletionVec.begin(), deletionVec.end(), resourceImageMap[imageKey]);
	if (it == deletionVec.end())
		deletionVec.push_back(resourceImageMap[imageKey]);

	//delete resourceImageMap[filePath];
	//resourceImageMap[filePath] = nullptr;
	//resourceImageMap.erase(filePath);
}


unsigned char* ResourceManager::GetImage(const std::size_t imageKey)
{
	//Same logic as func get image resource so just reuse it
	ResourceImage* image = GetImageResource(imageKey);
	return (image != nullptr) ? GetImageResource(imageKey)->Image(): nullptr;
}

unsigned char* ResourceManager::GetImage(const std::size_t imageKey, int& width, int& height, int& channels)
{
	//Same logic as func get image resource so just reuse it
	ResourceImage* image = GetImageResource(imageKey);
	if (image != nullptr)
	{
		width = image->Width();
		height = image->Height();
		channels = image->Channels();
		return GetImageResource(imageKey)->Image();
	}
	return nullptr;
}

ResourceImage* ResourceManager::GetImageResource(const std::size_t imageKey)
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



LOAD_RESULT ResourceManager::LoadTxtFile(std::string filePath, std::size_t& txtKey)
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
	newResource->LoadFile();

	//This checks if it is loaded or now
	if (!newResource->Loaded())
	{
		std::cout << "Failed to load txt" << std::endl;

		if (newResource)
			delete newResource;

		return LOAD_FAIL_BADPATH;
	}
	else
	{
		std::cout << "Loaded txt " << newResource->Path() << std::endl;

		//Insert the loaded image into the map only if managed to load
		resourceTxtFileMap.insert({ txtKey, newResource });
		totalResourcesLoaded++;
		return LOAD_SUCCESS;
	}

	return LOAD_RESULT();
}

void ResourceManager::UnloadTxtFile(std::size_t txtKey)
{
	//Check if resource alr exists
	if (resourceTxtFileMap.find(txtKey) == resourceTxtFileMap.end())
	{
		std::cout << "Resource does not exist, cannot unload" << std::endl;
		return;
	}

	resourceTxtFileMap[txtKey]->FlaggedForDeletion() = true;

	//Change this to a queue system
	//Check if it exists witin the vec alr tho
	auto it = std::find(deletionVec.begin(), deletionVec.end(), resourceTxtFileMap[txtKey]);
	if (it == deletionVec.end())
		deletionVec.push_back(resourceTxtFileMap[txtKey]);
	//deletionVec.push_back(resourceTxtFileMap[txtKey]);
}

std::vector<std::string> ResourceManager::GetTxtFile(const std::size_t txtKey)
{
	ResourceTxtFile* txt = GetTxtFileResource(txtKey);
	return (txt != nullptr) ? txt->TextStringVec() : std::vector<std::string>();
}

ResourceTxtFile* ResourceManager::GetTxtFileResource(const std::size_t txtKey)
{
	//Does the resource exist?
	if (resourceTxtFileMap.find(txtKey) != resourceTxtFileMap.end())
	{
		return resourceTxtFileMap[txtKey];
	}

	return nullptr;
}
void ResourceManager::SaveTxtFile(std::string filePath, std::vector<std::string> txtVec)
{
	ResourceTxtFile saveResource(txtVec, filePath);
	saveResource.SaveFile();
}	
#pragma endregion

#pragma region Json
LOAD_RESULT ResourceManager::LoadJson(std::string filePath, std::size_t& jsonKey)
{
	std::cout << "RM: ";

	jsonKey = ConstructMapKey(filePath);

	//Check if resource alr exists
	if (resourceJsonMap.find(jsonKey) != resourceJsonMap.end())
	{
		std::cout << filePath << " already exists" << std::endl;

		//Check if it was flagged for deletion
		if (resourceJsonMap[jsonKey]->FlaggedForDeletion())
		{
			//Remove the flag if it was flagged for deletion
			//(This will cause it to not delete in update)
			resourceJsonMap[jsonKey]->FlaggedForDeletion() = false;
			return LOAD_SUCCESS_STOPPED_UNLOAD;
		}
		else
			return LOAD_FAIL_PREEXIST;
	}

	//This loads the image via the ResourceImage class
	ResourceJson* newResource = new ResourceJson(filePath, jsonKey);
	newResource->LoadFile();

	//This checks if it is loaded or now
	if (!newResource->Loaded())
	{
		std::cout << "Failed to load json" << std::endl;

		if (newResource)
			delete newResource;

		return LOAD_FAIL_BADPATH;
	}
	else
	{
		std::cout << "Loaded json " << newResource->Path() << std::endl;

		//Insert the loaded image into the map only if managed to load
		resourceJsonMap.insert({ jsonKey, newResource });
		totalResourcesLoaded++;
		return LOAD_SUCCESS;
	}

	return LOAD_RESULT();
}
void ResourceManager::UnloadJson(std::size_t jsonKey)
{
	//Check if resource alr exists
	if (resourceJsonMap.find(jsonKey) == resourceJsonMap.end())
	{
		std::cout << "Resource does not exist, cannot unload" << std::endl;
		return;
	}

	resourceJsonMap[jsonKey]->FlaggedForDeletion() = true;

	//Change this to a queue system
	//Check if it exists witin the vec alr tho
	auto it = std::find(deletionVec.begin(), deletionVec.end(), resourceJsonMap[jsonKey]);
	if (it == deletionVec.end())
		deletionVec.push_back(resourceJsonMap[jsonKey]);
}
const nlohmann::json& ResourceManager::GetJson(const std::size_t jsonKey)
{
	ResourceJson* jsn = GetJsonResource(jsonKey);
	return jsn->GetJson();
}

ResourceJson* ResourceManager::GetJsonResource(const std::size_t jsonKey)
{
	//Does the resource exist?
	if (resourceJsonMap.find(jsonKey) != resourceJsonMap.end())
	{
		return resourceJsonMap[jsonKey];
	}

	return nullptr;
}
#pragma endregion

#pragma region Sound File Functions

LOAD_RESULT ResourceManager::LoadSound(std::string filePath, std::size_t& soundKey, FMOD::System* system)
{
	std::cout << "RM: ";

	soundKey = ConstructMapKey(filePath);

	//Check if resource alr exists
	if (resourceSoundMap.find(soundKey) != resourceSoundMap.end())
	{
		std::cout << filePath << " already exists" << std::endl;

		//Check if it was flagged for deletion
		if (resourceSoundMap[soundKey]->FlaggedForDeletion())
		{
			//Remove the flag if it was flagged for deletion
			//(This will cause it to not delete in update)
			resourceSoundMap[soundKey]->FlaggedForDeletion() = false;
			return LOAD_SUCCESS_STOPPED_UNLOAD;
		}
		else
			return LOAD_FAIL_PREEXIST;
	}

	//This loads the image via the ResourceImage class
	ResourceSound* newResource = new ResourceSound(filePath, soundKey);
	newResource->LoadFile(system);

	//This checks if it is loaded or now
	if (!newResource->Loaded())
	{
		std::cout << "Failed to load sound" << std::endl;

		if (newResource)
			delete newResource;

		return LOAD_FAIL_BADPATH;
	}
	else
	{
		std::cout << "Loaded sound " << newResource->Path() << std::endl;

		//Insert the loaded image into the map only if managed to load
		resourceSoundMap.insert({ soundKey, newResource });
		totalResourcesLoaded++;
		return LOAD_SUCCESS;
	}

	return LOAD_RESULT();
}

void ResourceManager::UnloadSound(std::size_t soundKey)
{
	//Check if resource alr exists
	if (resourceSoundMap.find(soundKey) == resourceSoundMap.end())
	{
		std::cout << "Resource does not exist, cannot unload" << std::endl;
		return;
	}

	resourceSoundMap[soundKey]->FlaggedForDeletion() = true;

	//Change this to a queue system
	//Check if it exists witin the vec alr tho
	auto it = std::find(deletionVec.begin(), deletionVec.end(), resourceSoundMap[soundKey]);
	if (it == deletionVec.end())
		deletionVec.push_back(resourceSoundMap[soundKey]);
	//deletionVec.push_back(resourceSoundMap[txtKey]);
}

FMOD::Sound* ResourceManager::GetSound(const std::size_t soundKey)
{
	ResourceSound* txt = GetSoundResource(soundKey);
	return (txt != nullptr) ? txt->Sound() : nullptr;
}

ResourceSound* ResourceManager::GetSoundResource(const std::size_t soundKey)
{
	//Does the resource exist?
	if (resourceSoundMap.find(soundKey) != resourceSoundMap.end())
	{
		return resourceSoundMap[soundKey];
	}

	return nullptr;
}
#pragma endregion


void ResourceManager::DeleteRM()
{
	delete this;
}

void ResourceManager::DebugTestEverything()
{
	ResourceManager* rm = this;

	//Testing a loading jpg
	std::size_t bananaKey = 0;
	std::cout << "Test Load Images (Png/JPG)" << std::endl;
	rm->LoadImage("assets/Banana.jpg", bananaKey); 

	if (rm->GetImage(bananaKey) != nullptr)
		std::cout << "Load Image(JPG) test: SUCCESS" << std::endl;
	else
		std::cout << "Load Image(JPG) test: FAIL" << std::endl;

	//Testing loading png
	std::size_t targetKey = 0;
	rm->LoadImage("assets/image_UI_target.png", targetKey);
	if (rm->GetImage(targetKey) != nullptr)
		std::cout << "Load Image(PNG) test: SUCCESS" << std::endl;
	else
		std::cout << "Load Image(PNG) test: FAIL" << std::endl;

	rm->UnloadImage(targetKey);
	switch (rm->LoadImage("assets/image_UI_target.png", targetKey))
	{
	case LOAD_SUCCESS_STOPPED_UNLOAD:
		std::cout << "Stop unload Image(PNG) test: SUCCESS" << std::endl;
		break;
	case LOAD_SUCCESS:
		std::cout << "Stop unload Image(PNG) test: FAILED => Load Result: LOAD_SUCCESS" << std::endl;
		break;
	case LOAD_FAIL_BADPATH:
		std::cout << "Stop unload Image(PNG) test: FAILED => Load Result: LOAD_FAIL_BADPATH" << std::endl;
		break;
	case LOAD_FAIL_PREEXIST:
		std::cout << "Stop unload Image(PNG) test: FAILED => Load Result: LOAD_FAIL_PREEXIST" << std::endl;
		break;
	default:
		std::cout << "Stop unload Image(PNG) test: FAILED => Load Result: UNKOWN (Swicth case default check)" << std::endl;
		break;
	}

	rm->UnloadImage(targetKey);
	rm->Update(0);
	if (rm->GetImage(targetKey) == nullptr)
		std::cout << "Unload Image(PNG): SUCCESS" << std::endl;
	else
		std::cout << "Unload Image(PNG): FAIL" << std::endl;

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
	std::cout << std::endl;

	std::cout << "Test Load Txt File" << std::endl;

	std::size_t txtKey = 0;
	rm->LoadTxtFile("assets/data/TestTxt.txt", txtKey);
	std::vector<std::string> txtVec = rm->GetTxtFile(txtKey);
	for (std::string strTest : txtVec)
	{
		std::cout << strTest << std::endl;
	}
	std::cout << std::endl;

	std::cout << "Save Txt File" << std::endl;
	std::vector<std::string> newTxt = {
		"This is a new txt file",
		"Newwww file",
		"THIS is line 4",
		"I lied it was 3",
		"This is 5 tho"
	};
	SaveTxtFile("assets/data/savedNewTxt.txt", newTxt);
	std::cout << "Attempted save to Txt File" << std::endl;


	std::cout << std::endl;
	std::cout << std::endl;

	rm->UnloadTxtFile(txtKey); //Actual unloading test
	rm->Update(0); //Run unload code

	std::cout << "Test Load Json File" << std::endl;
	std::size_t jsonKey = 0;
	rm->LoadJson("assets/data/jsonTestFile.json", jsonKey);

	//Riskier is there is a typo or something, it WILL crash if cannot find
	// std::cout << "boolean: " << (rm->GetJson(jsonKey)["testJson"]["boolean"]? "true" : "false") << std::endl;
	std::cout << "boolean: " << (rm->GetJson(jsonKey).at("testJson").at("boolean") ? "true" : "false") << std::endl;
	std::cout << "interger: " << rm->GetJson(jsonKey).at("testJson").at("interger") << std::endl;
	for (auto& [key, value] : rm->GetJson(jsonKey).items()) {
		std::cout << key << " : " << value << std::endl;
	}
	rm->UnloadJson(jsonKey);
	rm->Update(0); //Run unload code
	std::cout << std::endl;

	rm->LoadJson("assets/data/jsonTestFile2.json", jsonKey);
	for (auto& [key, value] : rm->GetJson(jsonKey).items()) {
		std::cout << key << " : " << value << std::endl;
	}

	rm->UnloadJson(jsonKey);
	rm->Update(0); //Run unload code
	std::cout << std::endl;
	std::cout << std::endl;

	FMOD::System* fSys = nullptr;
	FMOD::Channel* channel = nullptr;
	size_t soundKey = 0;
	FMOD_RESULT fResult = FMOD::System_Create(&fSys);
	if (fResult != FMOD_OK)
		std::cout << "FMOD SYSTEM create: FAILED" << std::endl;
	else
	{
		fSys->init(512, FMOD_INIT_NORMAL, nullptr);
		std::cout << "FMOD SYSTEM create: SUCCESS" << std::endl;
		if (rm->LoadSound("assets/Sound/SFX/sound_button_temp.wav", soundKey, fSys) == LOAD_SUCCESS)
		{
			std::cout << "Load Sound test: SUCCESS" << std::endl;
			fResult = fSys->playSound(rm->GetSound(soundKey), nullptr, false, &channel);
			fSys->update();
			bool isPlaying = true;
			do {
				channel->isPlaying(&isPlaying);
				fSys->update(); 
			} while (isPlaying);
		}
		else
			std::cout << "Load Sound test: FAIL" << std::endl;
	}

	if (fSys)
		fSys->release();

	rm->UnloadSound(soundKey);

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
			resourceTxtFileMap.erase(resource->MapKey());
			break;
		case RESOURCE_JSON:
			resourceJsonMap.erase(resource->MapKey());
			break;
		case RESOURCE_SOUND:
			resourceSoundMap.erase(resource->MapKey());
			break;
		}
		delete resource;
	}

	deletionVec.clear();
}


std::size_t ResourceManager::ConstructMapKey(std::string filePath)
{
	//Ignore this, no point to use just the file name, it dosnt matter so just use the entire path
	//size_t lastSlashPos = filePath.find_last_of('/');
	//if (lastSlashPos == filePath.npos)
	//{
	//	lastSlashPos = filePath.find_last_of('\\');
	//	if (lastSlashPos == filePath.npos)
	//	{
	//		lastSlashPos = 0;
	//	}
	//}

	//filePath = filePath.substr(lastSlashPos, filePath.size() - lastSlashPos);

	//Using hash to make a map key using the file path
	return std::hash<std::string>{}(filePath);
}
/******************************************************************************
 * File:        ResourceManager.cpp
 * Project:     NightSnack
 * Author(s):	Clement Ang Yang Ying (Primary, 99%) - Created manager class and logic
 *				Bryan Lim Jun Jie (Secondary, 1%) - Created orginal fmod system init code logic
 *
 * Description:
 * This file contains the definition of a ResourceManager class, that manages Resouorce pointers
 * It saves/creates Resources and puts them in a different map depending on the resource type. 
 * The key to the pointers made using the string of the file path and returning it to the caller.
 * When unloading Resources, it flags the Resource with a bool within the that class and unloads them all within update.
 * Each Resource has its own functions to interact with it (get/load/unload/etc.)
 * 
 * This class must be able to be function inderpendently of system do not add additional dependcies onto it.
 * This is as ErrorLog calls it outside of CoreEngine to save files, it must remain functuional by itself
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
#include "ResourceManager.h"
#include <fmod_errors.h>
#include <stb_image.h>

namespace NightSnack {

	ResourceManager::ResourceManager()
	{
		Init();
	}

	ResourceManager::~ResourceManager()
	{
		//Delete all resources 

		//Loop through entire image map AND DELETE IT MUHAHA
		//std::map<std::size_t, ResourceImage*>::iterator itImg;
		//for (itImg = m_resourceImageMap.begin(); itImg != m_resourceImageMap.end(); itImg++)
		//{
		//	delete itImg->second;
		//}
		//m_resourceImageMap.clear();

		////Delete txt map
		//std::map<std::size_t, ResourceTxtFile*>::iterator itTxt;
		//for (itTxt = m_resourceTxtFileMap.begin(); itTxt != m_resourceTxtFileMap.end(); itTxt++)
		//{
		//	delete itTxt->second;
		//}
		//m_resourceTxtFileMap.clear();

		////Delete json map
		//std::map<std::size_t, ResourceJson*>::iterator itJson;
		//for (itJson = m_resourceJsonMap.begin(); itJson != m_resourceJsonMap.end(); itJson++)
		//{
		//	delete itJson->second;
		//}
		//m_resourceJsonMap.clear();

		////Delete Sound map
		//std::map<std::size_t, ResourceSound*>::iterator itSound;
		//for (itSound = m_resourceSoundMap.begin(); itSound != m_resourceSoundMap.end(); itSound++)
		//{
		//	delete itSound->second;
		//}
		//m_resourceSoundMap.clear();

		////Delete Font map
		//std::map<std::size_t, ResourceFont*>::iterator itFont;
		//for (itFont = m_resourceFontMap.begin(); itFont != m_resourceFontMap.end(); itFont++)
		//{
		//	delete itFont->second;
		//}
		//m_resourceFontMap.clear();
		
		m_resourceImageMap.clear();
		m_resourceTxtFileMap.clear();
		m_resourceJsonMap.clear();
		m_resourceSoundMap.clear();
		m_resourceFontMap.clear();

		m_deletionVec.clear();

		//fModSystem deletion code is a lamba function created within Init();

	}

	void ResourceManager::Init()
	{
		totalResourcesLoaded = 0;
		stbi_set_flip_vertically_on_load(true); // Flip the y-axis of the images loaded

		FT_Init_FreeType(&m_fontLibrary);

		//Init Fmod
		//Orginal code from Audio System.Init() changed to shared ptr now :D

		//Just incase
		if (!m_fmodSystem)
		{
			m_fmodSystem.reset();
			m_fmodSystem = nullptr;
		}

		FMOD::System* systemPtr = nullptr;
		FMOD_RESULT result = FMOD::System_Create(&systemPtr);
		if (result != FMOD_OK)
		{
			std::cerr << "AudioSystem: Failed to create FMOD system - "
				<< FMOD_ErrorString(result) << std::endl;
		}
		else
		{
			result = systemPtr->init(32, FMOD_INIT_NORMAL, nullptr);
			if (result != FMOD_OK)
			{
				std::cerr << "AudioSystem: Failed to initialize FMOD system - "
					<< FMOD_ErrorString(result) << std::endl;
			}

			//std::cout << "AudioSystem: Initialized successfully with 32 channels" << std::endl;

			//Move the pointer into shared ptr;
			//Make a lamba func to rlease it when destoryed (turns out it crashes if i do i manually :( )
			m_fmodSystem = std::shared_ptr<FMOD::System>(systemPtr, [](FMOD::System* system) { if (system) system->release(); }); 
		}

		m_resourceImageMap.clear();
		m_resourceTxtFileMap.clear();
		m_resourceJsonMap.clear();
		m_resourceSoundMap.clear();
		m_resourceFontMap.clear();
	}

	void ResourceManager::Update()
	{
		DeleteFlaggedResources();
	}



#pragma region Image Functions
	LOAD_RESULT ResourceManager::LoadImage(std::string filePath, std::size_t& imageKey)
	{
		//std::cout << "RM: ";

		std::filesystem::path fp = std::filesystem::canonical(
			std::filesystem::current_path() / RESOURCE_PATH / filePath
		);

		imageKey = ConstructMapKey(fp.string());

		//Check if resource alr exists
		if (m_resourceImageMap.find(imageKey) != m_resourceImageMap.end())
		{
			//std::cout << filePath << " already exists" << std::endl;

			//Check if it was flagged for deletion
			if (m_resourceImageMap[imageKey]->FlaggedForDeletion())
			{
				//Remove the flag if it was flagged for deletion
				//(This will cause it to not delete in update)
				m_resourceImageMap[imageKey]->FlaggedForDeletion() = false;
				return LOAD_SUCCESS_STOPPED_UNLOAD;
			}
			else
				return LOAD_FAIL_PREEXIST;
		}

		//This loads the image via the ResourceImage class
		std::unique_ptr<ResourceImage> newResource = std::unique_ptr<ResourceImage>(new ResourceImage(fp.string(), imageKey));
		newResource->LoadFile();

		//This checks if it is loaded or now
		if (!newResource->Loaded())
		{
			//std::cout << "Failed to load image" << std::endl;

			return LOAD_FAIL_BADPATH;
		}
		else
		{
			//std::cout << "Loaded image " << newResource->Path() << std::endl;
			// 
			//Insert the loaded image into the map only if managed to load
			m_resourceImageMap.insert({imageKey, std::move(newResource) });
			totalResourcesLoaded++;
			return LOAD_SUCCESS;
		}

	}

	void ResourceManager::UnloadImage(std::size_t imageKey)
	{
		//Check if resource alr exists
		if (m_resourceImageMap.find(imageKey) == m_resourceImageMap.end())
		{
			//std::cout << "Resource does not exist, cannot unload" << std::endl;
			return;
		}

		m_resourceImageMap[imageKey]->FlaggedForDeletion() = true;

		//Change this to a queue system
		//Check if it exists witin the vec alr tho
		auto it = std::find(m_deletionVec.begin(), m_deletionVec.end(), std::make_pair(m_resourceImageMap[imageKey]->FileType(), imageKey));
		if (it == m_deletionVec.end())
			m_deletionVec.push_back({ m_resourceImageMap[imageKey]->FileType(), m_resourceImageMap[imageKey]->MapKey() });

		//delete m_resourceImageMap[filePath];
		//m_resourceImageMap[filePath] = nullptr;
		//m_resourceImageMap.erase(filePath);
	}


	unsigned char* ResourceManager::GetImage(const std::size_t imageKey)
	{
		//Same logic as func get image resource so just reuse it
		ResourceImage* image = GetImageResource(imageKey);
		return (image != nullptr) ? GetImageResource(imageKey)->Image() : nullptr;
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
		if (m_resourceImageMap.find(imageKey) != m_resourceImageMap.end())
		{
			return m_resourceImageMap[imageKey].get();
		}
		//Try to load the image if not loaded yet
		//else if (LoadImage(filePath))
		//{
		//	return m_resourceImageMap[filePath];
		//}

		//Return nullptr if failed to load
		return nullptr;
	}

	void ResourceManager::SaveCroppedImageResource(const std::size_t orginalKey, const std::string& newFileName, int startingX, int startingY, int newHeight, int newWidth)
	{
		if (m_resourceImageMap.find(orginalKey) == m_resourceImageMap.end())
		{
			return;
		}

		//int orginalHeight = m_resourceImageMap[orginalKey]->Height();
		int orginalWidth = m_resourceImageMap[orginalKey]->Width();
		int channels = m_resourceImageMap[orginalKey]->Channels();
		unsigned char* orginalImg = m_resourceImageMap[orginalKey]->Image();

		unsigned char* croppedImg = (unsigned char*)malloc(newHeight * newWidth * channels * sizeof(unsigned char));

		if (croppedImg)
		{
			for (int y = 0; y < newHeight; ++y) {
				for (int x = 0; x < newWidth; ++x) {

					//Calc pixel pos of original image
					int srcIndex = ((startingY + y) * orginalWidth + (startingX + x)) * channels;
					//Calc dest to place pixel
					int dstIndex = (y * newWidth + x) * channels;

					// Copy pixel data for all channels
					for (int c = 0; c < channels; ++c) {
						croppedImg[dstIndex + c] = orginalImg[srcIndex + c];
					}
				}
			}

			ResourceImage savedImaged(newFileName, *croppedImg, newWidth, newHeight, channels);
			savedImaged.SaveFile();

			free(croppedImg);
			croppedImg = NULL;
		}

	}

#pragma endregion

#pragma region Txt File Functions



	LOAD_RESULT ResourceManager::LoadTxtFile(std::string filePath, std::size_t& txtKey)
	{
		//std::cout << "RM: ";

		std::filesystem::path fp = std::filesystem::canonical(
			std::filesystem::current_path() / RESOURCE_PATH / filePath
		);

		txtKey = ConstructMapKey(fp.string());

		//Check if resource alr exists
		if (m_resourceTxtFileMap.find(txtKey) != m_resourceTxtFileMap.end())
		{
			//std::cout << filePath << " already exists" << std::endl;

			//Check if it was flagged for deletion
			if (m_resourceTxtFileMap[txtKey]->FlaggedForDeletion())
			{
				//Remove the flag if it was flagged for deletion
				//(This will cause it to not delete in update)
				m_resourceTxtFileMap[txtKey]->FlaggedForDeletion() = false;
				return LOAD_SUCCESS_STOPPED_UNLOAD;
			}
			else
				return LOAD_FAIL_PREEXIST;
		}

		//This loads the image via the ResourceImage class
		std::unique_ptr<ResourceTxtFile> newResource = std::unique_ptr<ResourceTxtFile>(new ResourceTxtFile(fp.string(), txtKey));
		newResource->LoadFile();

		//This checks if it is loaded or now
		if (!newResource->Loaded())
		{
			//std::cout << "Failed to load txt" << std::endl;

			return LOAD_FAIL_BADPATH;
		}
		else
		{

			//std::cout << "Loaded txt " << newResource->Path() << std::endl;

			//Insert the loaded image into the map only if managed to load
			m_resourceTxtFileMap.insert({ txtKey, std::move(newResource) });
			totalResourcesLoaded++;
			return LOAD_SUCCESS;
		}

		return LOAD_RESULT();
	}

	void ResourceManager::UnloadTxtFile(std::size_t txtKey)
	{
		//Check if resource alr exists
		if (m_resourceTxtFileMap.find(txtKey) == m_resourceTxtFileMap.end())
		{
			//std::cout << "Resource does not exist, cannot unload" << std::endl;
			return;
		}

		m_resourceTxtFileMap[txtKey]->FlaggedForDeletion() = true;

		//Change this to a queue system
		//Check if it exists witin the vec alr tho
		auto it = std::find(m_deletionVec.begin(), m_deletionVec.end(), std::make_pair(m_resourceTxtFileMap[txtKey]->FileType(), txtKey));
		if (it == m_deletionVec.end())
			m_deletionVec.push_back({ m_resourceTxtFileMap[txtKey]->FileType(), m_resourceTxtFileMap[txtKey]->MapKey() });
		//m_deletionVec.push_back(m_resourceTxtFileMap[txtKey]);
	}

	std::vector<std::string> ResourceManager::GetTxtFile(const std::size_t txtKey)
	{
		ResourceTxtFile* txt = GetTxtFileResource(txtKey);
		return (txt != nullptr) ? txt->TextStringVec() : std::vector<std::string>();
	}

	ResourceTxtFile* ResourceManager::GetTxtFileResource(const std::size_t txtKey)
	{
		//Does the resource exist?
		if (m_resourceTxtFileMap.find(txtKey) != m_resourceTxtFileMap.end())
		{
			return m_resourceTxtFileMap[txtKey].get();
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
	LOAD_RESULT ResourceManager::LoadJson(std::string filePath, std::size_t& jsonKey, bool overwrite)
	{
		//std::cout << "RM: ";

		//std::cout << "[RESOURCE] " << std::filesystem::current_path() << '\n';
		std::filesystem::path fp = std::filesystem::canonical(
			RESOURCE_PATH + filePath
		);

		jsonKey = ConstructMapKey(fp.string());

		//Check if resource alr exists
		if (m_resourceJsonMap.find(jsonKey) != m_resourceJsonMap.end() && !overwrite)
		{
			std::cout << filePath << " already exists" << std::endl;

			//Check if it was flagged for deletion
			if (m_resourceJsonMap[jsonKey]->FlaggedForDeletion())
			{
				//Remove the flag if it was flagged for deletion
				//(This will cause it to not delete in update)
				m_resourceJsonMap[jsonKey]->FlaggedForDeletion() = false;
				return LOAD_SUCCESS_STOPPED_UNLOAD;
			}
			else
				return LOAD_FAIL_PREEXIST;
		}

		//This loads the image via the ResourceImage class
		std::unique_ptr<ResourceJson> newResource = std::unique_ptr<ResourceJson>(new ResourceJson(fp.string(), jsonKey));
		newResource->LoadFile();

		//This checks if it is loaded or now
		if (!newResource->Loaded())
		{
			std::cout << "Failed to load json" << std::endl;

			return LOAD_FAIL_BADPATH;
		}
		else
		{

			//std::cout << "Loaded json " << newResource->Path() << std::endl;
			// 
			//Insert the loaded image into the map only if managed to load
			m_resourceJsonMap.insert({ jsonKey, std::move(newResource) });
			totalResourcesLoaded++;
			return LOAD_SUCCESS;
		}

		return LOAD_RESULT();
	}
	void ResourceManager::UnloadJson(std::size_t jsonKey)
	{
		//Check if resource alr exists
		if (m_resourceJsonMap.find(jsonKey) == m_resourceJsonMap.end())
		{
			//std::cout << "Resource does not exist, cannot unload" << std::endl;
			return;
		}

		m_resourceJsonMap[jsonKey]->FlaggedForDeletion() = true;

		//Change this to a queue system
		//Check if it exists witin the vec alr tho
		auto it = std::find(m_deletionVec.begin(), m_deletionVec.end(), std::make_pair(m_resourceJsonMap[jsonKey]->FileType(), jsonKey));
		if (it == m_deletionVec.end())
			m_deletionVec.push_back({ m_resourceJsonMap[jsonKey]->FileType(), m_resourceJsonMap[jsonKey]->MapKey() });

	}
	std::unordered_map<std::string, Archtype> ResourceManager::GetJson(const std::size_t jsonKey)
	{
		ResourceJson* jsn = GetJsonResource(jsonKey);
		return jsn->GetJson();
	}

	nlohmann::json& ResourceManager::GetJsonRaw(const std::size_t jsonKey)
	{
		ResourceJson* jsn = GetJsonResource(jsonKey);
		return jsn->GetJsonRaw();
	}

void ResourceManager::UpdateJson(size_t key, nlohmann::json const& newJson)
{
	if (m_resourceJsonMap.find(key) != m_resourceJsonMap.end())
	m_resourceJsonMap[key]->GetJsonRaw() = newJson;
}

	ResourceJson* ResourceManager::GetJsonResource(const std::size_t jsonKey)
	{
		//Does the resource exist?
		if (m_resourceJsonMap.find(jsonKey) != m_resourceJsonMap.end())
		{
			return m_resourceJsonMap[jsonKey].get();
		}

		return nullptr;
	}
	void ResourceManager::SaveJsonFile(std::string filePath, std::unordered_map<std::string, Archtype> const& newArchtypeMap)
	{
		std::filesystem::path fp = std::filesystem::canonical(
			std::filesystem::current_path() / RESOURCE_PATH / filePath
		);
		ResourceJson jsonResouce(newArchtypeMap, fp.string());
		jsonResouce.SaveFile();
	}

	std::shared_ptr<FMOD::System> ResourceManager::FmodSystem()
	{
		return m_fmodSystem;
	}

#pragma endregion

#pragma region Sound File Functions

	LOAD_RESULT ResourceManager::LoadSound(std::string filePath, std::size_t& soundKey, FMOD::System* system)
	{
		//std::cout << "RM: ";

		std::filesystem::path fp = std::filesystem::canonical(
			std::filesystem::current_path() / RESOURCE_PATH / filePath
		);
		filePath = fp.string();

		soundKey = ConstructMapKey(filePath);

		//Check if resource alr exists
		if (m_resourceSoundMap.find(soundKey) != m_resourceSoundMap.end())
		{

			//std::cout << filePath << " already exists" << std::endl;
			// 
			//Check if it was flagged for deletion
			if (m_resourceSoundMap[soundKey]->FlaggedForDeletion())
			{
				//Remove the flag if it was flagged for deletion
				//(This will cause it to not delete in update)
				m_resourceSoundMap[soundKey]->FlaggedForDeletion() = false;
				return LOAD_SUCCESS_STOPPED_UNLOAD;
			}
			else
				return LOAD_FAIL_PREEXIST;
		}

		//This loads the image via the ResourceImage class
		std::unique_ptr<ResourceSound> newResource = std::unique_ptr<ResourceSound>(new ResourceSound(filePath, soundKey));
		newResource->LoadFile(system);

		//This checks if it is loaded or now
		if (!newResource->Loaded())
		{
			//std::cout << "Failed to load sound" << std::endl;

			return LOAD_FAIL_BADPATH;
		}
		else
		{

			//std::cout << "Loaded sound " << newResource->Path() << std::endl;

			//Insert the loaded image into the map only if managed to load
			m_resourceSoundMap.insert({ soundKey, std::move(newResource) });
			totalResourcesLoaded++;
			return LOAD_SUCCESS;
		}

		return LOAD_RESULT();
	}

	void ResourceManager::UnloadSound(std::size_t soundKey)
	{
		//Check if resource alr exists
		if (m_resourceSoundMap.find(soundKey) == m_resourceSoundMap.end())
		{
			//std::cout << "Resource does not exist, cannot unload" << std::endl;
			return;
		}

		m_resourceSoundMap[soundKey]->FlaggedForDeletion() = true;

		//Change this to a queue system
		//Check if it exists witin the vec alr tho
		auto it = std::find(m_deletionVec.begin(), m_deletionVec.end(), std::make_pair(m_resourceSoundMap[soundKey]->FileType(), soundKey));
		if (it == m_deletionVec.end())
			m_deletionVec.push_back({ m_resourceSoundMap[soundKey]->FileType(), m_resourceSoundMap[soundKey]->MapKey() });

		//m_deletionVec.push_back(m_resourceSoundMap[txtKey]);
	}

	FMOD::Sound* ResourceManager::GetSound(const std::size_t soundKey)
	{
		ResourceSound* txt = GetSoundResource(soundKey);
		return (txt != nullptr) ? txt->Sound() : nullptr;
	}

	ResourceSound* ResourceManager::GetSoundResource(const std::size_t soundKey)
	{
		//Does the resource exist?
		if (m_resourceSoundMap.find(soundKey) != m_resourceSoundMap.end())
		{
			return m_resourceSoundMap[soundKey].get();
		}

		return nullptr;
	}
#pragma endregion

#pragma region Fonts
	FT_Library& ResourceManager::GetFontLibrary()
	{
		return m_fontLibrary;
	}

	LOAD_RESULT ResourceManager::LoadFont(std::string filePath, std::size_t& fontKey)
	{
		//std::cout << "RM: ";

		std::filesystem::path fp = std::filesystem::canonical(
			std::filesystem::current_path() / RESOURCE_PATH / filePath
		);
		filePath = fp.string();

		fontKey = ConstructMapKey(filePath);

		//Check if resource alr exists
		if (m_resourceFontMap.find(fontKey) != m_resourceFontMap.end())
		{
			//std::cout << filePath << " already exists" << std::endl;

			//Check if it was flagged for deletion
			if (m_resourceFontMap[fontKey]->FlaggedForDeletion())
			{
				//Remove the flag if it was flagged for deletion
				//(This will cause it to not delete in update)
				m_resourceFontMap[fontKey]->FlaggedForDeletion() = false;
				return LOAD_SUCCESS_STOPPED_UNLOAD;
			}
			else
				return LOAD_FAIL_PREEXIST;
		}

		//This loads the image via the ResourceImage class
		std::unique_ptr<ResourceFont> newResource = std::unique_ptr<ResourceFont>(new ResourceFont(filePath, fontKey));
		newResource->LoadFile(m_fontLibrary);

		//This checks if it is loaded or now
		if (!newResource->Loaded())
		{
			//std::cout << "Failed to load sound" << std::endl;

			return LOAD_FAIL_BADPATH;
		}
		else
		{

			//std::cout << "Loaded sound " << newResource->Path() << std::endl;

			//Insert the loaded image into the map only if managed to load
			m_resourceFontMap.insert({ fontKey, std::move(newResource) });
			totalResourcesLoaded++;
			return LOAD_SUCCESS;
		}

		return LOAD_RESULT();
	}

	void ResourceManager::UnloadFont(std::size_t fontKey)
	{
		//Check if resource alr exists
		if (m_resourceFontMap.find(fontKey) == m_resourceFontMap.end())
		{
			//std::cout << "Resource does not exist, cannot unload" << std::endl;
			return;
		}

		m_resourceFontMap[fontKey]->FlaggedForDeletion() = true;

		//Change this to a queue system
		//Check if it exists witin the vec alr tho
		auto it = std::find(m_deletionVec.begin(), m_deletionVec.end(), std::make_pair(m_resourceFontMap[fontKey]->FileType(), fontKey));
		if (it == m_deletionVec.end())
			m_deletionVec.push_back({ m_resourceFontMap[fontKey]->FileType(), m_resourceFontMap[fontKey]->MapKey() });

	}

	FT_Face ResourceManager::GetFont(const std::size_t fontKey)
	{
		ResourceFont* fnt = GetFontResource(fontKey);
		return (fnt != nullptr) ? fnt->Font() : nullptr;
	}
#pragma endregion

	ResourceFont* ResourceManager::GetFontResource(const std::size_t fontKey)
	{
		//Does the resource exist?
		if (m_resourceFontMap.find(fontKey) != m_resourceFontMap.end())
		{
			return m_resourceFontMap[fontKey].get();
		}

		return nullptr;
	}

	bool ResourceManager::ResourceLoaded(RESOURCE_TYPE type, size_t resourceKey)
	{
		Resource* resourceChecked = nullptr;
		//Try to avoid object slicing
		switch (type)
		{
		case RESOURCE_IMAGE:
			resourceChecked = GetImageResource(resourceKey);
			break;

		case RESOURCE_TXTFILE:
			resourceChecked = GetTxtFileResource(resourceKey);
			break;

		case RESOURCE_JSON:
			resourceChecked = GetJsonResource(resourceKey);
			break;

		case RESOURCE_SOUND:
			resourceChecked = GetSoundResource(resourceKey);
			break;

		case RESOURCE_FONT:
			resourceChecked = GetFontResource(resourceKey);
			break;

		default:
			//std::cout << "ResourceLoaded(): INVALID RESOURCE TYPE" << std::endl;
			return false;
		}

		if (resourceChecked && resourceChecked->Loaded())
			return true;
		else
			return false;
	}


	std::string ResourceManager::GetName() const
	{
		return "ResourceManager"; 
	}

	Signature ResourceManager::GetSignature() const
	{ return m_systemSignature; }


	//void ResourceManager::DebugTestEverything()
	//{
	//	ResourceManager* rm = this;

	//	//--------------------IMAGE FILE--------------------
	//	//Testing a loading jpg
	//	std::size_t bananaKey = 0;
	//	//std::cout << "<<<Test Load Images (Png/JPG)>>>" << std::endl;
	//	rm->LoadImage("assets/Banana.jpg", bananaKey);

	//	if (rm->GetImage(bananaKey) != nullptr)
	//		//std::cout << "Load Image(JPG) test: SUCCESS" << std::endl;
	//	else
	//		//std::cout << "Load Image(JPG) test: FAIL" << std::endl;

	//	//Testing loading png
	//	std::size_t targetKey = 0;
	//	rm->LoadImage("assets/image_UI_target.png", targetKey);
	//	if (ResourceLoaded(RESOURCE_IMAGE, targetKey))
	//		//std::cout << "Load Image(PNG) test: SUCCESS" << std::endl;
	//	else
	//		//std::cout << "Load Image(PNG) test: FAIL" << std::endl;

	//	rm->UnloadImage(targetKey);
	//	switch (rm->LoadImage("assets/image_UI_target.png", targetKey))
	//	{
	//	case LOAD_SUCCESS_STOPPED_UNLOAD:
	//		//std::cout << "Stop unload Image(PNG) test: SUCCESS" << std::endl;
	//		break;
	//	case LOAD_SUCCESS:
	//		//std::cout << "Stop unload Image(PNG) test: FAILED => Load Result: LOAD_SUCCESS" << std::endl;
	//		break;
	//	case LOAD_FAIL_BADPATH:
	//		//std::cout << "Stop unload Image(PNG) test: FAILED => Load Result: LOAD_FAIL_BADPATH" << std::endl;
	//		break;
	//	case LOAD_FAIL_PREEXIST:
	//		//std::cout << "Stop unload Image(PNG) test: FAILED => Load Result: LOAD_FAIL_PREEXIST" << std::endl;
	//		break;
	//	default:
	//		//std::cout << "Stop unload Image(PNG) test: FAILED => Load Result: UNKOWN (Swicth case default check)" << std::endl;
	//		break;
	//	}

	//	rm->UnloadImage(targetKey);
	//	rm->Update(0);
	//	if (rm->GetImage(targetKey) == nullptr)
	//		//std::cout << "Unload Image(PNG): SUCCESS" << std::endl;
	//	else
	//		//std::cout << "Unload Image(PNG): FAIL" << std::endl;

	//	rm->LoadImage("assets/image_UI_target.png", targetKey); //Testing loading a loaded image with deletion flagged
	//	rm->LoadImage("assets/image_UI_target.png", targetKey); //Testing loading same image
	//	rm->Update(0); //Run unload code
	//	rm->UnloadImage(targetKey); //Actual unloading test
	//	rm->Update(0); //Run unload code
	//	rm->LoadImage("assets/image_UI_target.png", targetKey); //Reloading a unloaded image test
	//	rm->GetImage(targetKey);
	//	rm->GetImage(bananaKey);
	//	if (ResourceLoaded(RESOURCE_IMAGE, targetKey))
	//	{
	//		//std::cout << "Get Image found target resource" << std::endl;
	//	}
	//	if (ResourceLoaded(RESOURCE_IMAGE, bananaKey))
	//	{
	//		//std::cout << "Get Image found banana resource" << std::endl;
	//	}

	//	//std::cout << std::endl;
	//	//std::cout << std::endl;


	//	//--------------------TXT FILE--------------------
	//	//std::cout << "<<<Test Txt File>>>" << std::endl;
	//	//std::cout << "Test Load Txt File" << std::endl;

	//	std::size_t txtKey = 0;
	//	rm->LoadTxtFile("assets/data/TestTxt.txt", txtKey);
	//	std::vector<std::string> txtVec = rm->GetTxtFile(txtKey);
	//	if (ResourceLoaded(RESOURCE_TXTFILE, txtKey))
	//	{
	//		//std::cout << "Load TXT: SUCCESS" << std::endl;
	//		for (std::string strTest : txtVec)
	//		{
	//			//std::cout << strTest << std::endl;
	//		}
	//	}
	//	else
	//		//std::cout << "Load TXT: FAIL" << std::endl;

	//	rm->UnloadTxtFile(txtKey); //Actual unloading test
	//	rm->Update(0); //Run unload code
	//	//std::cout << std::endl;


	//	//std::cout << "Test: Save Txt File" << std::endl;
	//	std::vector<std::string> newTxt = {
	//		"This is a new txt file",
	//		"Newwww file",
	//		"THIS is line 4",
	//		"I lied it was 3",
	//		"This is 5 tho"
	//	};
	//	SaveTxtFile("assets/data/savedNewTxt.txt", newTxt);
	//	//std::cout << "Attempted save to Txt File" << std::endl;

	//	//std::cout << std::endl;
	//	//std::cout << std::endl;

	//	//--------------------JSON FILE--------------------
	//	//std::cout << "<<<Test Json File>>>" << std::endl;
	//	//std::cout << "Test: Load Json File" << std::endl;
	//	std::size_t jsonKey = 0;
	//	rm->LoadJson("assets/data/jsonTestFile.json", jsonKey);
	//	if (ResourceLoaded(RESOURCE_JSON, jsonKey))
	//	{
	//		//std::cout << "Load JSON: SUCCESS" << std::endl;
	//		//Riskier is there is a typo or something, it WILL crash if cannot find
	//		//std::cout << "boolean: " << (rm->GetJsonRaw(jsonKey)["testJson"]["boolean"] ? "true" : "false") << std::endl;
	//		//std::cout << "boolean: " << (rm->GetJsonRaw(jsonKey).at("testJson").at("boolean") ? "true" : "false") << std::endl;
	//		//std::cout << "interger: " << rm->GetJsonRaw(jsonKey).at("testJson").at("interger") << std::endl;
	//		//for (auto& [key, value] : rm->GetJsonRaw(jsonKey).items()) {
	//		//	//std::cout << key << " : " << value << std::endl;
	//		//}
	//	}
	//	else
	//		//std::cout << "Load JSON: FAIL" << std::endl;

	//	rm->UnloadJson(jsonKey);
	//	rm->Update(0); //Run unload code
	//	//std::cout << std::endl;

	//	rm->LoadJson("assets/data/jsonTestFile2.json", jsonKey);
	//	if (ResourceLoaded(RESOURCE_JSON, jsonKey))
	//	{
	//		//std::cout << "Load JSON: SUCCESS" << std::endl;
	//	}
	//	else
	//		//std::cout << "Load JSON: FAIL" << std::endl;

	//	//for (auto& [key, value] : rm->GetJsonRaw(jsonKey).items()) {
	//	//	//std::cout << key << " : " << value << std::endl;
	//	//}

	//	//std::cout << "Test: Save Json File" << std::endl;
	//	rm->SaveJsonFile("assets/data/savedJsonTest2.json", rm->GetJson(jsonKey));


	//	rm->UnloadJson(jsonKey);
	//	rm->Update(0); //Run unload code
	//	//std::cout << std::endl;
	//	//std::cout << std::endl;

	//	//--------------------SOUND FILE--------------------
	//	//std::cout << "<<<Test FMOD/Sound File>>>" << std::endl;
	//	FMOD::System* fSys = nullptr;
	//	FMOD::Channel* channel = nullptr;
	//	size_t soundKey = 0;
	//	FMOD_RESULT fResult = FMOD::System_Create(&fSys);
	//	if (fResult != FMOD_OK)
	//		//std::cout << "FMOD SYSTEM create: FAILED" << std::endl;
	//	else
	//	{
	//		fSys->init(512, FMOD_INIT_NORMAL, nullptr);
	//		//std::cout << "FMOD SYSTEM create: SUCCESS" << std::endl;
	//		if (rm->LoadSound("assets/Sound/SFX/sound_button_temp.wav", soundKey, fSys) == LOAD_SUCCESS)
	//		{
	//			//std::cout << "Load Sound test: SUCCESS" << std::endl;
	//			fResult = fSys->playSound(rm->GetSound(soundKey), nullptr, false, &channel);
	//			fSys->update();
	//			bool isPlaying = true;
	//			do {
	//				channel->isPlaying(&isPlaying);
	//				fSys->update();
	//			} while (isPlaying);
	//		}
	//		else
	//			//std::cout << "Load Sound test: FAIL" << std::endl;
	//	}


	//	rm->UnloadSound(soundKey);
	//	rm->Update(0);

	//	if (fSys)
	//	{
	//		fSys->release();
	//		fSys = nullptr;
	//	}

	//	//std::cout << std::endl;
	//	//std::cout << std::endl;

	//	//--------------------FONT FILE--------------------
	//	//std::cout << "<<<Test FreeType/Font File>>>" << std::endl;

	//	size_t fontKey = 0;
	//	//std::cout << "Font Load Test" << std::endl;
	//	rm->LoadFont("assets/Font/liberation-mono.ttf", fontKey);
	//	//ResourceFont* recFont = rm->GetFontResource(fontKey);
	//	if (ResourceLoaded(RESOURCE_FONT, fontKey))
	//		//std::cout << "Font Load Test: SUCCESS" << std::endl;
	//	else
	//		//std::cout << "Font Load Test: FAILED" << std::endl;
	//	rm->UnloadFont(fontKey);
	//	rm->Update(0);

	//	//std::cout << std::endl;
	//	//std::cout << std::endl;
	//}

	void ResourceManager::DeleteFlaggedResources()
	{
		//This keeps changing so auto of the vec
		for (auto& resourcePair : m_deletionVec)
		{
			//If it was unflagged somewhere inbetween do not delete

			switch (resourcePair.second)
			{
			case RESOURCE_IMAGE:
				if (m_resourceImageMap[resourcePair.first]->FlaggedForDeletion())
					m_resourceImageMap.erase(resourcePair.first);
				break;
			case RESOURCE_TXTFILE:
				if (m_resourceTxtFileMap[resourcePair.first]->FlaggedForDeletion())
					m_resourceTxtFileMap.erase(resourcePair.first);
				break;
			case RESOURCE_JSON:
				if (m_resourceJsonMap[resourcePair.first]->FlaggedForDeletion())
					m_resourceJsonMap.erase(resourcePair.first);
				break;
			case RESOURCE_SOUND:
				if (m_resourceSoundMap[resourcePair.first]->FlaggedForDeletion())
					m_resourceSoundMap.erase(resourcePair.first);
				break;
			case RESOURCE_FONT:
				if (m_resourceFontMap[resourcePair.first]->FlaggedForDeletion())
					m_resourceFontMap.erase(resourcePair.first);
				break;

			}

			//DO NOT DELETED IT IS A UNIQUE PTR NOW
			//delete resource;
		}

		m_deletionVec.clear();
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

}	// NightSnack 

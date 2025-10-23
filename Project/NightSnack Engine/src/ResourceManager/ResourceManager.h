/******************************************************************************
 * File:        ResourceManager.h
 * Project:     NightSnack
 * Author(s):	Clement Ang Yang Ying (Primary, 100%) - Created manager class and logic
 *
 * Description:
 * This file contains the declarations of a ResourceManager class, that manages Resouorce pointers
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
 *************************
 ******************************************************************************/
#pragma once
#include <iostream>
#include <string>
#include <map>
#include <typeindex>
#include <vector>
#include <fmod.hpp>

#include "ResourceManager/Resource.h"
#include "Core/ECS/ISystem.h"

//PNG file formats, file bytes/critical chunks
//https://en.wikipedia.org/wiki/PNG#File_format

//Someone else trying to read PNG files (the compression format is really complicated apprently???)
//https://stackoverflow.com/questions/31079947/how-can-i-manually-read-png-files-in-c


//std::filesystem
//loading file paths
//https://www.geeksforgeeks.org/cpp/file-system-library-in-cpp-17/


//stb_image
//loadidng images
//https://solarianprogrammer.com/2019/06/10/c-programming-reading-writing-images-stb_image-libraries/

//freetype doccumentation
//https://freetype.org/freetype2/docs/tutorial/index.html

namespace NightSnack {

	enum LOAD_RESULT
	{
		LOAD_SUCCESS,		//Resource loaded in
		LOAD_SUCCESS_STOPPED_UNLOAD, //Resource already exists but was going to be unloaded soon, so it was stopped from being unloaded
		LOAD_FAIL_PREEXIST, //Resource already exists
		LOAD_FAIL_BADPATH,	//Resource fails to load due to bad file bath
	};

	class ResourceManager : public ISystem
	{
	private:
		unsigned int totalResourcesLoaded = 0;


		//Whenn creating a new map check this checlist
		//Init -> Cleared
		//Delete/Destructor -> deleted
		//DeleteFlaggedResources() -> erase in switch case
		//Load<Resource> -> Insert into map and increase resource counter
		//Unload<Resource> -> Set flag for dleetion
		//Get<Resource>
		//Get<Resource>Class
		//ResouceLoaded -> switch case
		//DebugTestEverything - No longer in use

		//Key, Value
		//Maps of resources
		std::map<std::size_t, ResourceImage*> resourceImageMap;
		std::map<std::size_t, ResourceTxtFile*> resourceTxtFileMap;
		std::map<std::size_t, ResourceJson*> resourceJsonMap;
		std::map<std::size_t, ResourceSound*> resourceSoundMap;
		std::map<std::size_t, ResourceFont*> resourceFontMap;

		//List of resources to track to delete
		std::vector<Resource*> deletionVec;

		//Font system
		FT_Library fontLibrary;

		std::shared_ptr<FMOD::System> fmodSystem; //Shared with audio system

		Signature m_systemSignature{};

	public:
		/************************************************************************/
		/*!
		\brief
		Default Constructor for Resource Manager
		*/
		/************************************************************************/
		ResourceManager();

		/************************************************************************/
		/*!
		\brief
		Destructor for Resource Manager
		*/
		/************************************************************************/
		~ResourceManager();

		/************************************************************************/
		/*!
		\brief
		Initalizes variables required for ResourceManager
		Also initalizes FT_Library
		*/
		/************************************************************************/
		void Init();

		/************************************************************************/
		/*!
		\brief
		Update function for logic (i.e calling flagged deletion)

		\param[in] dt
		delta time in float of time passed

		\param[in] coord
		Reference to Coordinator.
		*/
		/************************************************************************/
		void Update(float dt, Coordinator& Coord) override;


#pragma region Image Functions
		/************************************************************************/
		/*!
		\brief
		Load an image based on the file path.

		\param[in] filePath
		The input string of the file path.

		\param[out] imageKey
		The return of key to pass in to reobtain the resource (image).

		\return
		True if managed to load image successfully or if it already exists.
		False if failed to load.
		*/
		/************************************************************************/
		LOAD_RESULT LoadImage(std::string filePath, std::size_t& imageKey);

		/************************************************************************/
		/*!
		\brief
		Flags resource to be deleted/unloaded
		*/
		/************************************************************************/
		void UnloadImage(std::size_t imageKey);



		/************************************************************************/
		/*!
		\brief
		Get an image based on the file key.
		Uses GetImageResource to find class containing it.

		\param input
		The input string of the key to the resource

		\return
		Pointer of unsigned char of image.
		*/
		/************************************************************************/
		unsigned char* GetImage(const std::size_t imageKey);

		/************************************************************************/
		/*!
		\brief
		Get an image based on the file key.
		Uses GetImageResource to find class containing it.
		Returns variables to return width height and channels

		\param input[in]
		The input string of the file path.

		\param width[out]
		Output param, width of image

		\param height[out]
		Output param, height of image

		\return
		Pointer of unsigned char of image.
		*/
		/************************************************************************/
		unsigned char* GetImage(const std::size_t imageKey, int& width, int& height, int& channels);

		/************************************************************************/
		/*!
		\brief
		Get an image resource class pointer based on the file path

		\param input
		The input string of the file path

		\return
		Pointer of image resource
		*/
		/************************************************************************/
		ResourceImage* GetImageResource(const std::size_t imageKey);

#pragma endregion

#pragma region Txt File Functions
		/************************************************************************/
	/*!
	\brief
	Load an txt file based on the file path.

	\param[in] filePath
	The input string of the file path.

	\param[out] imageKey
	The return of key to pass in to reobtain the resource (txt file).

	\return
	LOAD_RESULT enum of what happened
	LOAD_SUCCESS - Resource loaded in.
	LOAD_SUCCESS_STOPPED_UNLOAD - Resource already exists but was going to be unloaded soon, so it was stopped from being unloaded.
	LOAD_FAIL_PREEXIST - Resource already exists, did not load.
	LOAD_FAIL_BADPATH -	Resource fails to load due to bad file bath.
	*/
	/************************************************************************/
		LOAD_RESULT LoadTxtFile(std::string filePath, std::size_t& txtKey);

		/************************************************************************/
		/*!
		\brief
		Flags resource to be deleted/unloaded
		*/
		/************************************************************************/
		void UnloadTxtFile(std::size_t txtKey);

		/************************************************************************/
		/*!
		\brief
		Get an image based on the file key.
		Uses GetImageResource to find class containing it.

		\param input
		The input string of the key to the resource

		\return
		Pointer of unsigned char of image.
		*/
		/************************************************************************/
		std::vector<std::string> GetTxtFile(const std::size_t txtKey);

		/************************************************************************/
		/*!
		\brief
		Get an Txt File resource class pointer based on the file path

		\param txtKey
		size__t key to the txt file

		\return
		Pointer of txt file resource
		*/
		/************************************************************************/
		ResourceTxtFile* GetTxtFileResource(const std::size_t txtKey);

		/************************************************************************/
		/*!
		\brief
		Saves a inputted vector of strings to save to the filepath inputted

		\param[in] filePath
		The input string of the file path

		\param[in] txtVec
		The inputed vector of strings to input
		*/
		/************************************************************************/
		void SaveTxtFile(std::string filePath, std::vector<std::string> txtVec);
#pragma endregion

#pragma region Json Functions
		/************************************************************************/
		/*!
		\brief
		Load an json file based on the file path.

		\param[in] filePath
		The input string of the file path.

		\param[out] imageKey
		The return of key to pass in to reobtain the resource (json).

		\return
		LOAD_RESULT enum of what happened
		LOAD_SUCCESS - Resource loaded in.
		LOAD_SUCCESS_STOPPED_UNLOAD - Resource already exists but was going to be unloaded soon, so it was stopped from being unloaded.
		LOAD_FAIL_PREEXIST - Resource already exists, did not load.
		LOAD_FAIL_BADPATH -	Resource fails to load due to bad file bath.
		*/
		/************************************************************************/
		LOAD_RESULT LoadJson(std::string filePath, std::size_t& imageKey);

		/************************************************************************/
		/*!
		\brief
		Flags resource to be deleted/unloaded
		*/
		/************************************************************************/
		void UnloadJson(std::size_t jsonKey);



		/************************************************************************/
		/*!
		\brief
		Get an unordered map of <std::string, Archtype> based on the file path.
		Uses GetImageResource to find class containing it.

		\param input
		The input string of the key to the resource

		\return
		Pointer of std::unordered_map<std::string, Archtype> of the json resource
		*/
		/************************************************************************/
		std::unordered_map<std::string, Archtype> GetJson(const std::size_t jsonKey);

		/************************************************************************/
		/*!
		\brief
		Get an nlohmann::json based on the file key.
		Uses GetImageResource to find class containing it.

		\param input
		The input string of the key to the resource

		\return
		Pointer of nlohmann::json of the json resource
		*/
		/************************************************************************/
		nlohmann::json GetJsonRaw(const std::size_t jsonKey);

		/************************************************************************/
		/*!
		\brief
		Get an json resource class pointer based on the file path

		\param input
		The input string of the file path

		\return
		Pointer of json resource
		*/
		/************************************************************************/
		ResourceJson* GetJsonResource(const std::size_t jsonKey);

		/************************************************************************/
		/*!
		\brief
		Saves a map containing json data to the filepath inputted

		\param[in] filePath
		The input string of the file path

		\param[in] newArchtypeMap
		std::unordered_map<std::string, Archtype>
		string are the is section name
		archtype struct of the component data
		*/
		/************************************************************************/
		void SaveJsonFile(std::string filePath, std::unordered_map<std::string, Archtype> const& newArchtypeMap);

#pragma endregion

#pragma region Sound Functions
		/************************************************************************/
		/*!
		/brief Gets the sharhed_tr of fmod system

		/return 
		FMOD::System shared_ptr
		*/
		/************************************************************************/
		std::shared_ptr<FMOD::System> FmodSystem();

		/************************************************************************/
		/*!
		\brief
		Load an sound based on the file path.

		\param[in] filePath
		The input string of the file path.

		\param[out] imageKey
		The return of key to pass in to reobtain the resource (sound).

		\param[out] system
		FMOD::System pointer to be used to init the sound

		\return
		LOAD_RESULT enum of what happened
		LOAD_SUCCESS - Resource loaded in.
		LOAD_SUCCESS_STOPPED_UNLOAD - Resource already exists but was going to be unloaded soon, so it was stopped from being unloaded.
		LOAD_FAIL_PREEXIST - Resource already exists, did not load.
		LOAD_FAIL_BADPATH -	Resource fails to load due to bad file bath.
		*/
		/************************************************************************/
		LOAD_RESULT LoadSound(std::string filePath, std::size_t& soundKey, FMOD::System* system);

		/************************************************************************/
		/*!
		\brief
		Flags resource to be deleted/unloaded
		*/
		/************************************************************************/
		void UnloadSound(std::size_t soundKey);


		/************************************************************************/
		/*!
		\brief
		Get an sound pointer based on the file key.
		Uses GetImageResource to find class containing it.

		\param input
		The input key to use for the resource map

		\return
		FMOD::Sound pointer of the sound resource
		*/
		/************************************************************************/
		FMOD::Sound* GetSound(const std::size_t soundKey);

		/************************************************************************/
		/*!
		\brief
		Get an sound resource class pointer based on the file path

		\param input
		The input string of the file path

		\return
		Pointer of sound resource
		*/
		/************************************************************************/
		ResourceSound* GetSoundResource(const std::size_t soundKey);
#pragma endregion

#pragma region Fonts
		/************************************************************************/
		/*!
		\brief
		Returns FT_Lirary refrence that is/was used to load fonts

		\return
		FT_Library refrence
		*/
		/************************************************************************/

		FT_Library& GetFontLibrary();

		/************************************************************************/
		/*!
		\brief
		Load an font based on the file path.

		\param[in] filePath
		The input string of the file path.

		\param[out] imageKey
		The return of key to pass in to reobtain the resource (font).

		\param[out] system
		FMOD::System pointer to be used to init the font

		\return
		LOAD_RESULT enum of what happened
		LOAD_SUCCESS - Resource loaded in.
		LOAD_SUCCESS_STOPPED_UNLOAD - Resource already exists but was going to be unloaded soon, so it was stopped from being unloaded.
		LOAD_FAIL_PREEXIST - Resource already exists, did not load.
		LOAD_FAIL_BADPATH -	Resource fails to load due to bad file bath.
		*/
		/************************************************************************/
		LOAD_RESULT LoadFont(std::string filePath, std::size_t& fontKey);

		/************************************************************************/
		/*!
		\brief
		Flags resource to be deleted/unloaded
		*/
		/************************************************************************/
		void UnloadFont(std::size_t fontKey);


		/************************************************************************/
		/*!
		\brief
		Get an FT_Face font based on the file key.
		Uses GetFontResource to find class containing it.

		\param input
		The input key to use for the resource map

		\return
		FT_Face of the font resource
		*/
		/************************************************************************/
		FT_Face GetFont(const std::size_t fontKey);

		/************************************************************************/
		/*!
		\brief
		Get an font resource class pointer based on the file path

		\param input
		The input string of the file path

		\return
		Pointer of font resource
		*/
		/************************************************************************/
		ResourceFont* GetFontResource(const std::size_t fontKey);
#pragma endregion


		/************************************************************************/
		/*!
		\brief
		Check if a resource is loaded/exists based on resource type and key

		\param type
		The type of resource to check for

		\return
		bool of is resource loaded/exists or not
		true - Resource exists AND loaded
		false - Resource does not exist OR is not loaded
		*/
		/************************************************************************/
		bool ResourceLoaded(RESOURCE_TYPE type, size_t resourceKey);

		/************************************************************************/
		/*!
		\brief
		Returns name of system (overridden from ISystem)
		Required by ISystem base class;

		\return
		Returns name of system (ResourceManager)
		*/
		/************************************************************************/
		std::string GetName() const override;
		/************************************************************************/
		/*!
		\brief
		Returns signature of system (overridden from ISystem)
		Required by ISystem base class;

		\return
		Returns name of system (ResourceManager)
		*/
		/************************************************************************/
		Signature GetSignature() const override;
		//void DebugTestEverything(); 

	private:

		/************************************************************************/
		/*!
		\brief
		Deletes all resources in the deletion vec that are also flagged for deletion
		If their flag is not set it will skip them
		*/
		/************************************************************************/
		void DeleteFlaggedResources();

		/************************************************************************/
		/*!
		\brief
		Constructs a key from a string to use in a map
		*/
		/************************************************************************/
		std::size_t ConstructMapKey(std::string filePath);

	};

}	// NightSnack

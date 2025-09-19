#pragma once
#include <iostream>
#include <string>
#include <map>
#include <typeindex>

#include "ResourceManager/Resource.h"
#include "../Singleton.h"

//PNG file formats, file bytes/critical chunks
//https://en.wikipedia.org/wiki/PNG#File_format

//Someone else trying to read PNG files (the compression format is really complicated apprently???)
//https://stackoverflow.com/questions/31079947/how-can-i-manually-read-png-files-in-c

#include <vector>

//std::filesystem
//loading file paths
//https://www.geeksforgeeks.org/cpp/file-system-library-in-cpp-17/


//stb_image
//loadidng images
//https://solarianprogrammer.com/2019/06/10/c-programming-reading-writing-images-stb_image-libraries/

enum LOAD_RESULT
{
	LOAD_SUCCESS,		//Resource loaded in
	LOAD_SUCCESS_STOPPED_UNLOAD, //Resource already exists but was going to be unloaded soon, so it was stopped from being unloaded
	LOAD_FAIL_PREEXIST, //Resource already exists
	LOAD_FAIL_BADPATH,	//Resource fails to load due to bad file bath
};

class ResourceManager : public Singleton<ResourceManager>
{
private:
	unsigned int totalResourcesLoaded = 0;

	//Key, Value
	//Maps of resources
	std::map<std::size_t, ResourceImage*> resourceImageMap;
	std::map<std::size_t, ResourceTxtFile*> resourceTxtFileMap;
	std::map<std::size_t, ResourceJson*> resourceJsonMap;

	//List of resources to track to delete
	std::vector<Resource*> deletionVec;

	ResourceManager();
	~ResourceManager();
	friend class Singleton<ResourceManager>;

public:

	/************************************************************************/
	/*!
	\brief
	Update function for logic (i.e calling flagged deletion)
	*/
	/************************************************************************/

	void Update(double dt);


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

	void UnloadImage(std::size_t imageKey);

	

	/************************************************************************/
	/*!
	\brief
	Get an image based on the file path.
	Uses GetImageResource to find class containing it.
	
	\param input
	The input string of the file path.
	
	\return
	Pointer of unsigned char of image.
	*/
	/************************************************************************/
	unsigned char* GetImage(const std::size_t imageKey);

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
	True if managed to load image successfully or if it already exists.
	False if failed to load.
	*/
	/************************************************************************/
	LOAD_RESULT LoadTxtFile(std::string filePath, std::size_t& txtKey);

	void UnloadTxtFile(std::size_t txtKey);

	/************************************************************************/
	/*!
	\brief
	Get an image based on the file path.
	Uses GetImageResource to find class containing it.
	
	\param input
	The input string of the file path.
	
	\return
	Pointer of unsigned char of image.
	*/
	/************************************************************************/
	std::vector<std::string> GetTxtFile(const std::size_t txtKey);

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
	ResourceTxtFile* GetTxtFileResource(const std::size_t txtKey);
#pragma endregion

#pragma region Json Functions
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
	LOAD_RESULT LoadJson(std::string filePath, std::size_t& imageKey);

	void UnloadJson(std::size_t jsonKey);



	/************************************************************************/
	/*!
	\brief
	Get an image based on the file path.
	Uses GetImageResource to find class containing it.

	\param input
	The input string of the file path.

	\return
	Pointer of unsigned char of image.
	*/
	/************************************************************************/
	const nlohmann::json& GetJson(const std::size_t jsonKey);

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
	ResourceJson* GetJsonResource(const std::size_t jsonKey);
#pragma endregion

	//Delete the Resource Manager
	void DeleteRM();

	void DebugTestEverything();

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
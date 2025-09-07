#pragma once
#include <iostream>
#include <string>
#include <map>


#include "ResourceManager/Resource.h"
#include "../Singleton.h"

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


class ResourceManager : public Singleton<ResourceManager>
{
private:
	//Key, Value
	std::map<std::string, ResourceImage*> resourceImageMap;

	ResourceManager();
	~ResourceManager();
	friend class Singleton<ResourceManager>;

public:

	void Update();

	/************************************************************************/
	/*!
	\brief
	Load an image based on the file path.
	
	\param input
	The input string of the file path.
	
	\return
	True if managed to load image successfully or if it already exists.
	False if failed to load.
	*/
	/************************************************************************/
	bool LoadImage(std::string filePath);

	void UnloadImage(std::string filePath);

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
	unsigned char* GetImage(std::string filePath);

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
	ResourceImage* GetImageResource(std::string filePath);

	//Delete the Resource Manager
	void DeleteRM();

};
#pragma once

#include <iostream>
#include <string>


//Unused for now (might not uise cause i used derived classes instead)
enum RESOURCE_TYPE
{
	RESOURCE_NONE,
	RESOURCE_IMAGE,
	RESOURCE_SOUND,
};

class Resource
{
protected:
	//Path of the file
	std::string path = "";

	RESOURCE_TYPE fileType = RESOURCE_NONE;

	bool resourceLoaded = false;


public:
	/************************************************************************/
	/*!
	\brief
	Virtual Constructor of resource class

	\param input
	The input string of the file path
	*/
	/************************************************************************/
	Resource(std::string filePath) {};
	virtual ~Resource();

	bool Loaded() const;
	std::string Path() const;

	RESOURCE_TYPE FileType() const;

private:
	//Default constructor is deleted
	Resource() = delete;
};

class ResourceImage : public Resource
{
protected:
	//Data ptr of the obj
	unsigned char* imageData = NULL;
	//Image imageData
	int width = 0, height = 0, channels = 0;

public:
	ResourceImage(std::string filePath);
	~ResourceImage();


	unsigned char* Image() const;


private:
	//Default constructor is deleted
	ResourceImage() = delete;
};

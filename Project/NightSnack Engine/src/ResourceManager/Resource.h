#pragma once

#include <iostream>
#include <string>


//Unused for now (might not uise cause i used derived classes instead)
enum RESOURCE_TYPE
{
	RESOURCE_IMAGE,
	RESOURCE_SOUND,
};

class Resource
{
protected:
	//Path of the file
	std::string path = "";
	//Data ptr of the obj
	unsigned char* data = NULL;

	bool resourceLoaded = false;


public:
	/************************************************************************/
	/*!
	\brief
	Virtual Constructor of resource class

	\param input
	The input string of the file path

	\return
	*/
	/************************************************************************/
	Resource(std::string filePath) {};
	virtual ~Resource();

	bool Loaded() const;
	std::string Path() const;

private:
	//Default constructor is deleted
	Resource() = delete;
};

class ResourceImage : public Resource
{
protected:
	//Image data
	int width = 0, height = 0, channels = 0;

public:
	ResourceImage(std::string filePath);
	~ResourceImage();

private:
	//Default constructor is deleted
	ResourceImage() = delete;
};

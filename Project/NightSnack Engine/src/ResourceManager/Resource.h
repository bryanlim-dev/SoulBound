#pragma once

#include <iostream>
#include <string>
#include <typeindex>
#include <vector>

#include <json.hpp>


//Unused for now (might not uise cause i used derived classes instead)
enum RESOURCE_TYPE
{
	RESOURCE_NONE,
	RESOURCE_IMAGE,
	RESOURCE_TXTFILE,
	RESOURCE_SOUND,
};

class Resource
{
protected:

	std::type_index mapKey;

	//Path of the file
	std::string path = "";

	RESOURCE_TYPE resourceType = RESOURCE_NONE;

	bool resourceLoaded = false;

	bool flaggedForDeletion = false;

public:
	/************************************************************************/
	/*!
	\brief
	Virtual Constructor of resource class

	\param input
	The input string of the file path
	*/
	/************************************************************************/
	Resource(const std::string &inFilePath, const std::type_index &inMapKey) : path(inFilePath), mapKey(inMapKey)
	{
	};
	virtual ~Resource();


	bool Loaded() const;
	std::string Path() const;

	std::type_index MapKey() const;

	bool& FlaggedForDeletion();

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
	ResourceImage(const std::string& inFilePath, const std::type_index& inMapKey);
	~ResourceImage();


	unsigned char* Image() const;


private:
	//Default constructor is deleted
	ResourceImage() = delete;
};


class ResourceTxtFile : public Resource
{
protected:
	//vector of strings of the txt file
	std::vector<std::string> txtStringVec;

public:
	ResourceTxtFile(const std::string& inFilePath, const std::type_index& inMapKey);
	~ResourceTxtFile();

	std::vector<std::string> TextStringVec() const;
};


class ResourceSound : public Resource
{

};
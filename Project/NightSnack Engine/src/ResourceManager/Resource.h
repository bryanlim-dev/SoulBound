#pragma once

#include <iostream>
#include <string>
#include <typeindex>
#include <vector>

#include <json.hpp>

#define RESOURCE_DEBUG_MODE 1


//Unused for now (might not uise cause i used derived classes instead)
enum RESOURCE_TYPE
{
	RESOURCE_NONE,
	RESOURCE_IMAGE,
	RESOURCE_TXTFILE,
	RESOURCE_JSON,
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

struct Archtype
{
	std::unordered_map<std::string, std::string> componentMap;
	std::unordered_map<std::string, Archtype> subArchtype;
};

class ResourceJson : public Resource
{
private:
	nlohmann::json jsonData;
	std::unordered_map<std::string, Archtype> archetypeMap;

public: 
	ResourceJson(const std::string& inFilePath, const std::type_index& jsonKey);
	~ResourceJson();

	nlohmann::json& GetJson();

private:
	void ConvertJsonToArchtypeMap();

	//Splits data in json into a archtype map
	//May be called recurrsivly if htere is nested data
	Archtype SplitJsonSectionData(const nlohmann::json &sectionData);


	//A function to print all data within the map
	//Calls PrintMapSubArchtypeData as a recurssive function if there is nested data;
	//This may not print data out as same format as json as all nested data is printed at end
	void PrintJsonMapData();


	//Prints the contents within a Archtype
	//Is a recurrsive function to print nested archtypes
	//This may not print data out as same format as json as all nested data is printed at end
	void PrintMapSubArchtypeData(const Archtype& arch, int indents);
};


class ResourceSound : public Resource
{

};
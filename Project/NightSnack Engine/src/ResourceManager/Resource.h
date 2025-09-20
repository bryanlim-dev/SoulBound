#pragma once

#include <iostream>
#include <string>
#include <typeindex>
#include <vector>

#include <json.hpp>

#define RESOURCE_DEBUG_MODE 1


//Type of resource loaded
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

	std::size_t mapKey = 0 ;

	//Path of the file
	std::string path = "";

	RESOURCE_TYPE resourceType = RESOURCE_NONE;

	bool resourceLoaded = false;

	bool flaggedForDeletion = false;

public:
	//Default constructor
	Resource();
	/************************************************************************/
	/*!
	\brief
	Virtual Constructor of resource class

	\param input
	The input string of the file path
	*/
	/************************************************************************/
	Resource(const std::string &inFilePath, const std::size_t &inMapKey) : path(inFilePath), mapKey(inMapKey)
	{
	};
	virtual ~Resource();


	bool Loaded() const;
	std::string Path() const;

	std::size_t MapKey() const;

	bool& FlaggedForDeletion();

	RESOURCE_TYPE FileType() const;

	virtual void LoadFile() {};

private:

};

class ResourceImage : public Resource
{
protected:
	//Data ptr of the obj
	unsigned char* imageData = NULL;
	//Image imageData
	int width = 0, height = 0, channels = 0;

public:
	ResourceImage();
	ResourceImage(const std::string& inFilePath, const std::size_t& inMapKey);
	~ResourceImage();

	void LoadFile() override;

	unsigned char* Image() const;


private:
};

class ResourceTxtFile : public Resource
{
protected:
	//vector of strings of the txt file
	std::vector<std::string> txtStringVec;

public:
	/************************************************************************/
	/*!
	\brief
	Default Constructor of txt resource class
	Creates a class with no no txt data
	*/
	/************************************************************************/
	ResourceTxtFile();

	/************************************************************************/
	/*!
	\brief
	Constructor of txt resource class
	Creates a class that stores filepath and mapKey
	Does NOT open file, call LoadFile for that
	*/
	/************************************************************************/
	ResourceTxtFile(const std::string& inFilePath, const std::size_t& inMapKey);
	~ResourceTxtFile();

	void LoadFile() override;

	std::vector<std::string> TextStringVec() const;

private:

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
	ResourceJson();
	ResourceJson(const std::string& inFilePath, const std::size_t& jsonKey);
	~ResourceJson();

	void LoadFile() override;

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
/******************************************************************************
 * File:        Resource.h
 * Project:     NightSnack
 * Author(s):	Clement Ang Yang Ying (Primary, 100%) - Created base class and child class
 *
 * Description:
 * This file contains the declarations of a base Resource class and its derived classes (Img, Sound, Json, Txt & Font)
 * It has various functions such as loading, gettinng and unloading, etc.
 * Some resouces have additional functions such as saving for txt or json, or converting json into a struct with maps
 * It also has enum to diffreinate differnt types sor resources
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <iostream>
#include <string>
#include <typeindex>
#include <vector>

#include <json.hpp>
#include <fmod.hpp>
#include <ft2build.h>

#define RESOURCE_DEBUG_MODE 1
#include FT_FREETYPE_H

//Type of resource loaded
enum RESOURCE_TYPE
{
	RESOURCE_NONE,
	RESOURCE_IMAGE,
	RESOURCE_TXTFILE,
	RESOURCE_JSON,
	RESOURCE_SOUND,
	RESOURCE_FONT,
};

class Resource
{
protected:

	//Key to the map in resourcemanager
	std::size_t mapKey = 0 ;

	//Path of the file
	std::string path = "";

	//Type of resource
	RESOURCE_TYPE resourceType = RESOURCE_NONE;

	//Is this resource loaded
	bool resourceLoaded = false;

	//This flag is to check if it will be deleted next cycle
	bool flaggedForDeletion = false;

public:
	//Default constructor deleted
	Resource() = delete;
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

	/************************************************************************/
	/*!
	\brief
	Virtual Destructor of resource class
	*/
	/************************************************************************/
	virtual ~Resource();


	/************************************************************************/
	/*!
	\brief
	Returns if the resosurce loaded or not.

	\return
	bool of var isLoaded
	true - Resource is loaded
	false - Resource is not loaded
	*/
	/************************************************************************/
	bool Loaded() const;
	/************************************************************************/
	/*!
	\brief
	Returns filepath of the resource

	\return
	std::string filepath of the resource
	*/
	/************************************************************************/
	std::string Path() const;
	/************************************************************************/
	/*!
	\brief
	Returns key to the map used to store this resource

	\return
	std::size_t of the key of the key-pair value in the map used to store this
	*/
	/************************************************************************/
	std::size_t MapKey() const;
	/************************************************************************/
	/*!
	\brief
	Returns refrence to the bool of is the Resource is flagged for deletion next cycle

	\return
	bool refrennce of vara flaggedForDeletion
	*/
	/************************************************************************/
	bool& FlaggedForDeletion();
	/************************************************************************/
	/*!
	\brief
	Returns enum RESOURCE_TYPE of the type resource this class is

	\return
	RESOURCE_TYPE enum of the resource type class is
	*/
	/************************************************************************/
	RESOURCE_TYPE FileType() const;

	/************************************************************************/
	/*!
	\brief
	Pure virtual function to load a file
	Does not do anything in the base class
	*/
	/************************************************************************/
	virtual void LoadFile() {};
	/************************************************************************/
	/*!
	\brief
	Pure virtual function to save a file
	Does not do anything in the base class
	*/
	/************************************************************************/
	virtual void SaveFile() {};

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
	//Default constructor deleted
	ResourceImage() = delete;
	ResourceImage(const std::string& inFilePath, const std::size_t& inMapKey);
	~ResourceImage();

	void LoadFile() override;


	unsigned char* Image() const;

	int Width() const { return width; }
	int Height() const { return height; }
	int Channels() const { return channels; }

private:
	//UNUSED FOR NOW DO NOT USE
	void SaveFile() override;
};

class ResourceTxtFile : public Resource
{
protected:
	//vector of strings of the txt file
	std::vector<std::string> txtStringVec;

public:
	//Default constructor deleted
	ResourceTxtFile() = delete;

	/************************************************************************/
	/*!
	\brief
	Constructor of txt resource class
	Creates a class that stores filepath and mapKey
	Does NOT open file, call LoadFile for that
	*/
	/************************************************************************/
	ResourceTxtFile(const std::string& inFilePath, const std::size_t& inMapKey);	

	/************************************************************************/
	/*!
	\brief
	Constructor of txt resource class
	Creates a class with set txtVec and path

	\param[in] txtVec
	Vector of strings to store

	\param[in] newPath
	string of the filepath to store
	*/
	/************************************************************************/
	ResourceTxtFile(std::vector<std::string>& txtVec, std::string newPath);

	~ResourceTxtFile();

	void LoadFile() override;
	void SaveFile() override;

	void EditResource(std::vector<std::string> &txtVec, std::string newPath);

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
	//Default constructor deleted
	ResourceJson() = delete;
	ResourceJson(const std::string& inFilePath, const std::size_t& jsonKey);
	ResourceJson(std::unordered_map<std::string, Archtype> const& newArchtypeMap, std::string newPath);
	~ResourceJson();

	void LoadFile() override;
	/************************************************************************/
	/*!
	\brief
	Converts map data into nlohmann::json via ConvertArchtypeToJson()
	Then saves that data into the filepath stored
	*/
	/************************************************************************/
	void SaveFile() override;

	std::unordered_map<std::string, Archtype> GetJson();
	nlohmann::json& GetJsonRaw();

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

	//Converts archtype to nlohmann::json
	//Calls convery sub 
	void ConvertArchtypeToJson();

	//Converts sub sections recurrsivly
	nlohmann::json ConvertSubArchtypeToJson(Archtype const& archType);
};


class ResourceSound : public Resource
{
private:
	FMOD::Sound* soundData = nullptr;

public:
	//Default constructor deleted
	ResourceSound() = delete;
	ResourceSound(const std::string& inFilePath, const std::size_t& inMapKey);
	~ResourceSound();

	void LoadFile(FMOD::System* system);

	FMOD::Sound* Sound() const;

private:
	void LoadFile() override {};


};

class ResourceFont : public Resource
{
private:
	FT_Face fontFace = nullptr;

public:
	ResourceFont(const std::string& inFilePath, const std::size_t& inMapKey);
	~ResourceFont();

	//Returns FT_Face stored
	FT_Face Font();

	void LoadFile(FT_Library& fontLib);

private:
	void LoadFile() override {};
	void SaveFile() override {};

};
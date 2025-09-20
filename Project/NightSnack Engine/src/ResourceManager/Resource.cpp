#include <iostream>
#include <fstream>
#include "Resource.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "fmod.hpp"

#pragma region Base Resource
Resource::Resource()
	: mapKey(0)
{
}

Resource::~Resource()
{
	std::cout << path << " is deleted!" << std::endl;
}


bool Resource::Loaded() const
{
	return resourceLoaded;
}

std::string Resource::Path() const
{
	return path;
}

std::size_t Resource::MapKey() const
{
	return mapKey;
}

RESOURCE_TYPE Resource::FileType() const
{
	return resourceType;
}

bool& Resource::FlaggedForDeletion()
{
	return flaggedForDeletion;
}
#pragma endregion


#pragma region Image
ResourceImage::ResourceImage()
{
	resourceType = RESOURCE_IMAGE;
}
ResourceImage::ResourceImage(const std::string& inFilePath, const std::size_t& inMapKey)
	: Resource(inFilePath, inMapKey)
{
	//Set file path
	resourceType = RESOURCE_IMAGE;
}


ResourceImage::~ResourceImage()
{
	//Check if the imageData exists incase
	if (imageData)
	{
		delete imageData;
		imageData = NULL;

		resourceLoaded = false;
	}
}

void ResourceImage::LoadFile()
{
	//Load file
	imageData = stbi_load(path.c_str(), &width, &height, &channels, 0);


	if (imageData)
		resourceLoaded = true;
	else
		resourceLoaded = false;
}

unsigned char* ResourceImage::Image() const
{
	return imageData;
}
#pragma endregion


#pragma region TxtFile
ResourceTxtFile::ResourceTxtFile()
{
	resourceType = RESOURCE_TXTFILE;
}

ResourceTxtFile::ResourceTxtFile(const std::string& inFilePath, const std::size_t& inMapKey)
	: Resource(inFilePath, inMapKey)
{
	resourceType = RESOURCE_TXTFILE;
}

ResourceTxtFile::~ResourceTxtFile()
{
}

void ResourceTxtFile::LoadFile()
{
	//Open filepath
	std::fstream inFile(path.c_str());
	if (!inFile.is_open())
	{
		resourceLoaded = false;
		return;
	}

	std::string fileLine{};
	while (std::getline(inFile, fileLine))
	{
		txtStringVec.push_back(fileLine);
	}

	inFile.close();

	resourceLoaded = true;
}

std::vector<std::string> ResourceTxtFile::TextStringVec() const
{
	return txtStringVec;
}

#pragma endregion

#pragma region JsonFile
ResourceJson::ResourceJson()
{
	resourceType = RESOURCE_JSON;
}
ResourceJson::ResourceJson(const std::string& inFilePath, const std::size_t& inMapKey)
	: Resource(inFilePath, inMapKey)
{
	resourceType = RESOURCE_JSON;
}                                                              

ResourceJson::~ResourceJson()
{
}

void ResourceJson::LoadFile()
{
	std::ifstream inFile(path);

	if (!inFile.is_open())
	{
		resourceLoaded = false;
		return;
	}

	inFile >> jsonData;

	ConvertJsonToArchtypeMap();

	resourceLoaded = true;

#if RESOURCE_DEBUG_MODE 1

	PrintJsonMapData();

#endif
}


nlohmann::json& ResourceJson::GetJson()
{
	return jsonData;
}
void ResourceJson::ConvertJsonToArchtypeMap()
{
	//Get header
	//structured bindings :D. declare multiple variables from a tuple/struct (in this case the json)
	//woah this is cool i didnt know this existed
	//check nlohmann::json github doccumentation for more stuff like this

	//This loop is for the section heading/names 
	 //structured bindings again whoo 
	for (auto& [sectionName, sectionData] : jsonData.items()) {
		
		Archtype archTypeData = SplitJsonSectionData(sectionData);
		archetypeMap.insert(std::pair(sectionName, archTypeData));

	}
}
Archtype ResourceJson::SplitJsonSectionData(const nlohmann::json& sectionData)
{
	Archtype archTypeData;

	//Split the data into componenets
	for (auto& [keyInput, value] : sectionData.items())
	{
		//Key is a string
		//Value is data

		//CHeck if is a simple type - int, bool, char, string, etc.
		if (value.is_primitive())
		{
			//For now it saves as a string, split this later
			archTypeData.componentMap.insert(std::pair(keyInput, value.dump()));
		}
		else if (value.is_array())
		{
			//Yeet the entire value array as a string
			archTypeData.componentMap[keyInput] = value.dump();
		}
		//It's a nested section
		else if (value.is_object())
		{
			//This loop is for the section heading/names 
			//Insert it into sub map
			Archtype subArchTypeData = SplitJsonSectionData(value);
			archTypeData.subArchtype.insert(std::pair(keyInput, std::move(subArchTypeData)));
		}
	}


	return archTypeData;
}

void ResourceJson::PrintJsonMapData()
{
	std::cout << "Printing JSON MAP DATA" << std::endl;
	//Loops through all top level sections
	//Then calls function to print data stored within
	//Auto resolves to whatever map type is being used, it might change in the future so auto type 
	for (const auto& kv : archetypeMap) {
		std::cout << "Section: " << kv.first << std::endl << " {" << std::endl;
		PrintMapSubArchtypeData(kv.second, 2);
		std::cout << "}" << std::endl << std::endl;
	}
}

void ResourceJson::PrintMapSubArchtypeData(const Archtype& arch, int indents = 0)
{
	//Add ' '/indentation acording to how many its stated
	std::string strIndents(indents, ' ');

	//Print all components frst
	//Auto resolves to whatever map type is being used, it might change in the future so auto type 
	for (const auto& data : arch.componentMap) {
		std::cout << strIndents << data.first << " : " << data.second << std::endl;
	}

	//Then print all nested data "D
	for (const auto& subData : arch.subArchtype) {
		std::cout << strIndents << subData.first << " : " << std::endl;
		std::cout << strIndents << "{" << std::endl;
		PrintMapSubArchtypeData(subData.second, indents + 2);
		std::cout << strIndents << "}" << std::endl;
	}
}
#pragma endregion
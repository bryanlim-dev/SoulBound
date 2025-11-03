/******************************************************************************
 * File:        Resource.cpp
 * Project:     NightSnack
 * Author(s):	Clement Ang Yang Ying (Primary, 100%) - Created base class and child class
 *
 * Description:
 * This file contains the definitions of a base Resource class and its derived classes (Img, Sound, Json, Txt & Font)
 * It has various functionss such as loading, gettinng and unloading, etc.
 * Some resouces have additional functions such as saving for txt or json, or converting json into a struct with maps
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
#include <iostream>
#include <fstream>
#include "Resource.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "fmod.hpp"

namespace NightSnack {

#pragma region Base Resource

	Resource::~Resource()
	{
#if RESOURCE_DEBUG_MODE
		std::cout << m_path << " is deleted!" << std::endl;
#endif
	}


	bool Resource::Loaded() const
	{
		return m_resourceLoaded;
	}

	std::string Resource::Path() const
	{
		return m_path;
	}

	std::size_t Resource::MapKey() const
	{
		return m_mapKey;
	}

	RESOURCE_TYPE Resource::FileType() const
	{
		return m_resourceType;
	}

	bool& Resource::FlaggedForDeletion()
	{
		return m_flaggedForDeletion;
	}
#pragma endregion


#pragma region Image

	ResourceImage::ResourceImage(const std::string& inFilePath, const std::size_t& inMapKey)
		: Resource(inFilePath, inMapKey)
	{
		//Set file path
		m_resourceType = RESOURCE_IMAGE;
	}


	ResourceImage::~ResourceImage()
	{
		//Check if the imageData exists incase
		if (m_imageData)
		{
			delete m_imageData;
			m_imageData = NULL;

			m_resourceLoaded = false;
		}
	}

	void ResourceImage::LoadFile()
	{
		stbi_set_flip_vertically_on_load(true);
		//Load file
		m_imageData = stbi_load(m_path.c_str(), &m_width, &m_height, &m_channels, 0);


		if (m_imageData)
			m_resourceLoaded = true;
		else
			m_resourceLoaded = false;
	}

	//void ResourceImage::SaveFile() {
	//	//Empty for now
	//}

	unsigned char* ResourceImage::Image() const
	{
		return m_imageData;
	}
#pragma endregion


#pragma region TxtFile


	ResourceTxtFile::ResourceTxtFile(const std::string& inFilePath, const std::size_t& inMapKey)
		: Resource(inFilePath, inMapKey)
	{
		m_resourceType = RESOURCE_TXTFILE;
	}

	ResourceTxtFile::ResourceTxtFile(std::vector<std::string>& txtVec, std::string newPath)
		: Resource(newPath, 0)
	{
		m_resourceType = RESOURCE_TXTFILE;

		m_txtStringVec = txtVec;
		m_path = newPath;
	}

	ResourceTxtFile::~ResourceTxtFile()
	{
	}

	void ResourceTxtFile::LoadFile()
	{
		//Open filepath
		std::fstream inFile(m_path.c_str());
		if (!inFile.is_open())
		{
			m_resourceLoaded = false;
			return;
		}

		std::string fileLine{};
		while (std::getline(inFile, fileLine))
		{
			m_txtStringVec.push_back(fileLine);
		}

		inFile.close();

		m_resourceLoaded = true;
	}

	void ResourceTxtFile::SaveFile()
	{
		std::ofstream outFile(m_path.c_str());

		if (outFile.is_open())
		{
			for (std::string str : m_txtStringVec)
			{
				outFile << str << std::endl;
			}

			outFile.close();
		}


	}

	void ResourceTxtFile::EditResource(std::vector<std::string>& txtVec, std::string newPath)
	{
		m_txtStringVec = txtVec;
		m_path = newPath;
	}

	std::vector<std::string> ResourceTxtFile::TextStringVec() const
	{
		return m_txtStringVec;
	}

#pragma endregion

#pragma region JsonFile
	ResourceJson::ResourceJson(const std::string& inFilePath, const std::size_t& inMapKey)
		: Resource(inFilePath, inMapKey)
	{
		m_resourceType = RESOURCE_JSON;
	}

	ResourceJson::ResourceJson(std::unordered_map<std::string, Archtype> const& newArchtypeMap, std::string newPath)
		: Resource(newPath, 0)
	{
		m_resourceType = RESOURCE_JSON;

		m_archetypeMap = newArchtypeMap;
	}

	ResourceJson::~ResourceJson()
	{

	}

	void ResourceJson::LoadFile()
	{
		std::ifstream inFile(m_path);

		if (!inFile.is_open())
		{
			m_resourceLoaded = false;
			return;
		}

		inFile >> m_jsonData;

		ConvertJsonToArchtypeMap();

		m_resourceLoaded = true;

#if RESOURCE_DEBUG_MODE

		PrintJsonMapData();

#endif
	}

	void ResourceJson::SaveFile()
	{
		ConvertArchtypeToJson();

		std::ofstream outFile(m_path.c_str());

		if (outFile.is_open())
		{
			outFile << m_jsonData;
			outFile.close();
		}
	}


	std::unordered_map<std::string, Archtype> ResourceJson::GetJson()
	{
		return m_archetypeMap;
	}

	nlohmann::json& ResourceJson::GetJsonRaw()
	{
		return m_jsonData;
	}
	void ResourceJson::ConvertJsonToArchtypeMap()
	{
		//Get header
		//structured bindings :D. declare multiple variables from a tuple/struct (in this case the json)
		//woah this is cool i didnt know this existed
		//check nlohmann::json github doccumentation for more stuff like this

		//This loop is for the section heading/names 
		 //structured bindings again whoo 
		for (auto& [sectionName, sectionData] : m_jsonData.items()) {

			Archtype archTypeData = SplitJsonSectionData(sectionData);
			m_archetypeMap.insert(std::pair(sectionName, archTypeData));

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
				//Splits strings and other types
				if (value.is_string())
					archTypeData.m_componentMap.insert(std::pair(keyInput, value.get<std::string>()));
				else
					archTypeData.m_componentMap.insert(std::pair(keyInput, value.dump()));
			}
			else if (value.is_array())
			{
				std::string arrStr;
				arrStr.clear();
				for (size_t arrPos = 0; arrPos < value.size(); ++arrPos)
				{
					if (value[arrPos].is_string())
						arrStr += value[arrPos].get<std::string>();
					else
						arrStr += value[arrPos].dump();

					if (arrPos + 1 < value.size())
						arrStr += ", ";
				}

				archTypeData.m_componentMap[keyInput] = arrStr;
			}
			//It's a nested section
			else if (value.is_object())
			{
				//This loop is for the section heading/names 
				//Insert it into sub map
				Archtype subArchTypeData = SplitJsonSectionData(value);
				archTypeData.m_subArchtype.insert(std::pair(keyInput, std::move(subArchTypeData)));
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
		for (const auto& kv : m_archetypeMap) {
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
		for (const auto& data : arch.m_componentMap) {
			std::cout << strIndents << data.first << " : " << data.second << std::endl;
		}

		//Then print all nested data "D
		for (const auto& subData : arch.m_subArchtype) {
			std::cout << strIndents << subData.first << " : " << std::endl;
			std::cout << strIndents << "{" << std::endl;
			PrintMapSubArchtypeData(subData.second, indents + 2);
			std::cout << strIndents << "}" << std::endl;
		}
	}

	void ResourceJson::ConvertArchtypeToJson()
	{
		m_jsonData.clear();

		//structured bindings round 3
		//Save top level section  and sub section as its data
		for (const auto& [archTypeKey, archtypeStructData] : m_archetypeMap)
		{
			m_jsonData.emplace(archTypeKey, ConvertSubArchtypeToJson(archtypeStructData));
		}
	}

	nlohmann::json ResourceJson::ConvertSubArchtypeToJson(Archtype const& m_subArchtype)
	{
		nlohmann::json subArch;
		//Save all the components first
		//Then recurrsive call this function to save sub section componenets

		//Basic key-value components saved
		for (auto const& [subArchTypeKey, subArchTypeData] : m_subArchtype.m_componentMap)
		{
			subArch.emplace(subArchTypeKey, subArchTypeData);
		}

		//Now save all further key-seciton components
		for (auto const& [subSectionArchKey, subSectionArchTypeData] : m_subArchtype.m_subArchtype)
		{
			subArch.emplace(subSectionArchKey, ConvertSubArchtypeToJson(subSectionArchTypeData));
		}

		return subArch;
	}

#pragma endregion

#pragma region Sound
	ResourceSound::ResourceSound(const std::string& inFilePath, const std::size_t& inMapKey)
		: Resource(inFilePath, inMapKey)
	{
		m_resourceType = RESOURCE_SOUND;
	}
	ResourceSound::~ResourceSound()
	{
		if (m_soundData)
		{
			m_soundData->release();
			//delete m_soundData;
			m_soundData = nullptr;
		}

		m_soundData = nullptr;
	}

	void ResourceSound::LoadFile(FMOD::System* system)
	{
		FMOD_RESULT result = system->createSound(m_path.c_str(), FMOD_DEFAULT, nullptr, &m_soundData);

		if (result != FMOD_OK)
		{
			m_resourceLoaded = false;
			return;
		}

		m_resourceLoaded = true;
	}

	FMOD::Sound* ResourceSound::Sound() const
	{
		return m_soundData;
	}

#pragma endregion

#pragma region Font
	ResourceFont::ResourceFont(const std::string& inFilePath, const std::size_t& inMapKey)
		: Resource(inFilePath, inMapKey)
	{
		m_resourceType = RESOURCE_FONT;
	}

	ResourceFont::~ResourceFont()
	{
		if (fontFace)
			FT_Done_Face(fontFace);

		fontFace = nullptr;

		m_resourceLoaded = false;
	}

	FT_Face ResourceFont::Font()
	{
		return fontFace;
	}

	void ResourceFont::LoadFile(FT_Library& fontLib)
	{
		//Load font based on file path using system
		FT_Error error = FT_New_Face(fontLib, m_path.c_str(), 0, &fontFace);

		if (error)
		{
			m_resourceLoaded = false;

#if RESOURCE_DEBUG_MODE
			std::cout << m_path << " font type cannot be loaded!" << std::endl;
			if (error == FT_Err_Unknown_File_Format)
			{
				std::cout << m_path << " font type is unnsupported and cannot be loaded!" << std::endl;
			}
#endif
		}
		else
			m_resourceLoaded = true;
	}
#pragma endregion

}	// NightSnack
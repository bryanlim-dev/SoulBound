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

namespace NightSnack {

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
		std::size_t m_mapKey = 0;

		//Path of the file
		std::string m_path = "";

		//Type of resource
		RESOURCE_TYPE m_resourceType = RESOURCE_NONE;

		//Is this resource loaded
		bool m_resourceLoaded = false;

		//This flag is to check if it will be deleted next cycle
		bool m_flaggedForDeletion = false;

	public:
		//Default constructor deleted
		Resource() = delete;
		/************************************************************************/
		/*!
		\brief
		Virtual Constructor of resource class

		\param[in] inFilePath
		std::string const ref file path of the file to load

		\param[in] inMapKey
		std::size_t const ref of the key that is used in the map to store it
		*/
		/************************************************************************/
		Resource(const std::string& inFilePath, const std::size_t& inMapKey) : m_path(inFilePath), m_mapKey(inMapKey)
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
		bool refrennce of vara m_flaggedForDeletion
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

		//virtual void SaveFile() {};

	private:

	};

	class ResourceImage : public Resource
	{
	protected:
		//Data ptr of the obj
		unsigned char* m_imageData = NULL;
		//Image imageData
		int m_width = 0, m_height = 0, m_channels = 0;

	public:
		//Default constructor deleted
		ResourceImage() = delete;
		/************************************************************************/
		/*!
		\brief
		Constructor of ResourceImage that handles image resources

		\param[in] inFilePath
		std::string const ref file path of the file to load

		\param[in] inMapKey
		std::size_t const ref of the key that is used in the map to store it
		*/
		/************************************************************************/
		ResourceImage(const std::string& inFilePath, const std::size_t& inMapKey);
		/************************************************************************/
		/*!
		\brief
		Destructor of ResouceImage.
		Desstoys image resource if it is loaded
		*/
		/************************************************************************/
		~ResourceImage();
		/************************************************************************/
		/*!
		\brief
		Loads image resource from saved file path
		*/
		/************************************************************************/
		void LoadFile() override;

		/************************************************************************/
		/*!
		\brief
		Gets unsigned char* of the loaded image reosurce

		\return
		Returns unsigned char* of the image
		*/
		/************************************************************************/
		unsigned char* Image() const;
		/************************************************************************/
		/*!
		\brief
		Gets width of image loaded

		\return
		int of width of image
		*/
		/************************************************************************/
		int Width() const { return m_width; }
		/************************************************************************/
		/*!
		\brief
		Gets height of image loaded

		\return
		int of height of image
		*/
		/************************************************************************/
		int Height() const { return m_height; }
		/************************************************************************/
		/*!
		\brief
		Gets channels of image loaded

		\return
		int of channels of image
		*/
		/************************************************************************/
		int Channels() const { return m_channels; }

	private:

		//UNUSED FOR NOW DO NOT USE
		//void SaveFile() override;
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

		\param[in] inFilePath
		std::string const ref of the file path of the text file to open

		\param[in] inMapKey
		std::size_t const ref of the key that is used in the map to store it
		*/
		/************************************************************************/
		ResourceTxtFile(const std::string& inFilePath, const std::size_t& inMapKey);

		/************************************************************************/
		/*!
		\brief
		Constructor of txt resource class
		Creates a class with set txtVec and path
		Does NOT open file, call LoadFile for that

		\param[in] txtVec
		Vector of strings to store

		\param[in] newPath
		string of the filepath to store
		*/
		/************************************************************************/
		ResourceTxtFile(std::vector<std::string>& txtVec, std::string newPath);
		/************************************************************************/
		/*!
		\brief
		Destructor of ResouceTxt.
		Desstoys txt file resource if it is loaded
		*/
		/************************************************************************/
		~ResourceTxtFile();

		/************************************************************************/
		/*!
		\brief
		Loads txt file resource from saved file path
		*/
		/************************************************************************/
		void LoadFile() override;
		/************************************************************************/
		/*!
		\brief
		Saved image resource to stored file path
		*/
		/************************************************************************/
		void SaveFile();
		/************************************************************************/
		/*!
		\brief
		Edited stored data within resource class

		\param[in] txtVec
		std::vector of std::strings of the new txt
		Each string is a new line

		\param[in] newPath
		std::string of new path/name of file
		*/
		/************************************************************************/
		void EditResource(std::vector<std::string>& txtVec, std::string newPath);

		/************************************************************************/
		/*!
		\brief
		Gets std::vector of std::string of info within txt resource class

		\return
		std::vector of std::string of text
		Each std::string within the vector is a new line of text
		*/
		/************************************************************************/
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
		/************************************************************************/
		/*!
		\brief
		Constructor of json resource class
		Creates a class that stores filepath and mapKey
		Does NOT open file, call LoadFile for that

		\param[in] inFilePath
		std::string const ref of the file path of the text file to open

		\param[in] jsonKey
		std::size_t const ref of the key that is used in the map to store it
		*/
		/************************************************************************/
		ResourceJson(const std::string& inFilePath, const std::size_t& jsonKey);
		/************************************************************************/
		/*!
		\brief
		Constructor of json resource class
		Creates a class that stores filepath and mapKey
		Does NOT open file, call LoadFile for that

		\param[in] newArchtypeMap
		std::unordered_map<std::string, Archtype> of section name (key) to the Archtype struct of data

		\param[in] newPath
		std::string const ref of the file path stored

		*/
		/************************************************************************/
		ResourceJson(std::unordered_map<std::string, Archtype> const& newArchtypeMap, std::string newPath);
		/************************************************************************/
		/*!
		\brief
		Destructor of ResourceJson.
		*/
		/************************************************************************/
		~ResourceJson();

		/************************************************************************/
		/*!
		\brief
		Loads json from store file path
		Converts nlohmann::json into map with Archtype struct via ConvertJsonToArchtypeMap()
		*/
		/************************************************************************/
		void LoadFile() override;
		/************************************************************************/
		/*!
		\brief
		Converts map data into nlohmann::json via ConvertArchtypeToJson()
		Then saves that data into the filepath stored
		*/
		/************************************************************************/
		void SaveFile();

		/************************************************************************/
		/*!
		\brief
		Gets map of std::string (key/section name) to Archtype (section data)

		\return
		std::unordered_map<std::string, Archtype> of json file
		std::string - Key, Section Name
		Archtype struct - Value, stores component values and any sub sections
		*/
		/************************************************************************/
		std::unordered_map<std::string, Archtype> GetJson();
		/************************************************************************/
		/*!
		\brief
		Gets nlohmann::json data stored

		\return
		json data stored as type nlohmann::json
		*/
		/************************************************************************/
		nlohmann::json& GetJsonRaw();

	private:
		/************************************************************************/
		/*!
		\brief
		Converts nlohmann::json stored into std::unordered_map of
		sections name (string) to Archtype struct
		*/
		/************************************************************************/
		void ConvertJsonToArchtypeMap();

		//Splits data in json into a archtype map
		//May be called recurrsivly if htere is nested data
		/************************************************************************/
		/*!
		\brief
		Splits data in sectionData into a archtype map
		May be called recurrsivly if htere is nested data

		\return
		Archtype struct of stored component data
		*/
		/************************************************************************/
		Archtype SplitJsonSectionData(const nlohmann::json& sectionData);


		/************************************************************************/
		/*!
		\brief
		A function to print all data within the map
		Calls PrintMapSubArchtypeData as a recurssive function if there is nested data;
		This may not print data out as same format as json as all nested data is printed at end
		*/
		/************************************************************************/
		void PrintJsonMapData();


		/************************************************************************/
		/*!
		\brief
		Prints the contents within a Archtype
		Is a recurrsive function to print nested archtypes
		This may not print data out as same format as json as all nested data is printed at end
		*/
		/************************************************************************/
		void PrintMapSubArchtypeData(const Archtype& arch, int indents);

		/************************************************************************/
		/*!
		\brief
		Converts archtype to nlohmann::json
		Calls ConvertSubArchtypeToJson as recurrsive function if there is nested data
		*/
		/************************************************************************/
		void ConvertArchtypeToJson();

		/************************************************************************/
		/*!
		\brief
		Converts archtype to nlohmann::json
		Converts sub sections recurrsivly
		*/
		/************************************************************************/
		nlohmann::json ConvertSubArchtypeToJson(Archtype const& archType);
	};


	class ResourceSound : public Resource
	{
	private:
		FMOD::Sound* soundData = nullptr;

	public:
		//Default constructor deleted
		ResourceSound() = delete;
		/************************************************************************/
		/*!
		\brief
		Constructor of sound resource class
		Inits a resource class that stores filepath and mapKey
		Does NOT open file, call LoadFile for that

		\param[in] inFilePath
		std::string const ref of the file path of the text file to open

		\param[in] inMapKey
		std::size_t const ref of the key that is used in the map to store it
		*/
		/************************************************************************/
		ResourceSound(const std::string& inFilePath, const std::size_t& inMapKey);
		/************************************************************************/
		/*!
		\brief
		Destructor of ResourceSound.
		Frees soundData if loaded
		*/
		/************************************************************************/
		~ResourceSound();

		/************************************************************************/
		/*!
		\brief
		Loads soune from stored file path via the inputted Fmod system

		\param[in] system
		FMOD::System pointer used to load the sound file
		*/
		/************************************************************************/
		void LoadFile(FMOD::System* system);


		/************************************************************************/
		/*!
		\brief
		Gets sound data

		\return
		FMOD::Sound pointer of loaded sound
		*/
		/************************************************************************/
		FMOD::Sound* Sound() const;

	private:
		/************************************************************************/
		/*!
		\brief
		Unused hidden private function.
		As the load requires fmod system, this cannot be used
		*/
		/************************************************************************/
		void LoadFile() override {};


	};

	class ResourceFont : public Resource
	{
	private:
		FT_Face fontFace = nullptr;

	public:
		/************************************************************************/
		/*!
		\brief
		Constructor of font resource class
		Inits a resource class that stores filepath and mapKey
		Does NOT open file, call LoadFile for that

		\param[in] inFilePath
		std::string const ref of the file path of the text file to open

		\param[in] inMapKey
		std::size_t const ref of the key that is used in the map to store it
		*/
		/************************************************************************/
		ResourceFont(const std::string& inFilePath, const std::size_t& inMapKey);
		/************************************************************************/
		/*!
		\brief
		Destructor of ResourceFont.
		Frees fontFace if loaded
		*/
		/************************************************************************/
		~ResourceFont();

		/************************************************************************/
		/*!
		\brief
		Gets font data

		\return
		FT_Face of loaded font
		*/
		/************************************************************************/
		FT_Face Font();

		/************************************************************************/
		/*!
		\brief
		Loads font resource from saved file path using inputted FT_Library

		\param[in]
		FT_Library refrence of library to use to load font
		*/
		/************************************************************************/
		void LoadFile(FT_Library& fontLib);

	private:
		/************************************************************************/
		/*!
		\brief
		Unused hidden private function.
		As the load requires FT_Library, this cannot be used
		*/
		/************************************************************************/
		void LoadFile() override {};
		//void SaveFile() override {};

	};

}	// NightSnack

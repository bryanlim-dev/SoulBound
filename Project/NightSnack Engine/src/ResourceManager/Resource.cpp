#include <iostream>
#include <fstream>;
#include "Resource.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "fmod.hpp"

#pragma region Base Resource
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

std::type_index Resource::MapKey() const
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
ResourceImage::ResourceImage(const std::string& inFilePath, const std::type_index& inMapKey) 
	: Resource(inFilePath, inMapKey)
{
	//Set file path
	resourceType = RESOURCE_IMAGE;

	//Load file
	imageData = stbi_load(path.c_str(), &width, &height, &channels, 0);


	if (imageData)
		resourceLoaded = true;
	else
		resourceLoaded = false;
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

unsigned char* ResourceImage::Image() const
{
	return imageData;
}
#pragma endregion

ResourceTxtFile::ResourceTxtFile(const std::string& inFilePath, const std::type_index& inMapKey)
	: Resource(inFilePath, inMapKey)
{

	resourceType = RESOURCE_TXTFILE;

	//Open filepath
	std::fstream inFile(inFilePath.c_str());
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

ResourceTxtFile::~ResourceTxtFile()
{
}

const std::vector<const std::string> ResourceTxtFile::TextStringVec() const
{
	return std::vector<const std::string>();
}

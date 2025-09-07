#include "Resource.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



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

RESOURCE_TYPE Resource::FileType() const
{
	return fileType;
}

bool& Resource::FlaggedForDeletion()
{
	return flaggedForDeletion;
}



ResourceImage::ResourceImage(std::string filePath) : Resource(filePath)
{
	//Set file path
	path = filePath;
	fileType = RESOURCE_IMAGE;

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

#include "Resource.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



Resource::~Resource()
{
	//Check if the data exists (incase deleted in derived class)
	if (data)
	{
		delete data;
		data = NULL;

		resourceLoaded = false;
	}

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




ResourceImage::ResourceImage(std::string filePath) : Resource(filePath)
{
	//Set file path
	path = filePath;

	//Load file
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);


	if (data)
		resourceLoaded = true;
	else
		resourceLoaded = false;
}


ResourceImage::~ResourceImage()
{

}

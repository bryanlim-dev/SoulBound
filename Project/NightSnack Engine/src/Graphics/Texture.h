/******************************************************************************
 * File:        Texture.h
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created Texture class
 *
 * Description:
 * This Texture class loads image data from the ResourceManager.
 * It then creates an OpenGL 2D texture and configures default parameters.
 * There are helper functions for binding and getting texture properties.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

#include "ResourceManager/ResourceManager.h"

namespace NightSnack {

	class Texture {

	public:

		/************************************************************************/
		/*!
		\brief
		Constructor for the Texture class.
		Loads an image from the ResourceManager and creates a 2D OpenGL texture.
		Sets up default texture parameters.

		\param[in] textureImage
		File path to the texture image
		*/
		/************************************************************************/
		Texture(std::string textureImage);

		/************************************************************************/
		/*!
		\brief
		Destructor for the Texture class.
		Deletes the OpenGL texture object
		*/
		/************************************************************************/
		~Texture();

		/************************************************************************/
		/*!
		\brief
		Binds the texture to a specified OpenGL texture unit, for the shader files
		to sample from it.

		\param[in] unit
		The texture unit index
		*/
		/************************************************************************/
		void Bind(GLuint unit = 0) const;

		/************************************************************************/
		/*!
		\brief
		Retrieves the OpenGL texture ID of this Texture object

		\return
		The OpenGL texture ID
		*/
		/************************************************************************/
		GLuint GetID() const;

		/************************************************************************/
		/*!
		\brief
		Checks whether this texture has an alpha component

		\return
		True if the texture has an alpha component
		*/
		/************************************************************************/
		bool HasAlpha() const;

	private:

		GLuint m_texID{};
		bool m_hasAlphaComponent{ false };

	};

}	// NightSnack

#endif // TEXTURE_H
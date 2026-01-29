/******************************************************************************
 * File:        Texture.cpp
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

#include "Texture.h"

namespace NightSnack {

	Texture::Texture(std::string textureImage, std::shared_ptr<ResourceManager> rm, int cols, int rows) 
		: m_textureName{ textureImage }, m_columns { cols }, m_rows{ rows } {

		if (textureImage.empty()) {
			std::cerr << "Error: Texture filename where??\n";
			m_texID = 0;
			return;
		}

		// ResourceManager stuff
		size_t key = 0;
		rm->LoadImage(textureImage, key);

		// Texture parameters
		int width, height, channels;

		unsigned char* pixels = rm->GetImage(key, width, height, channels);

		// Actual Texture stuff
		if (channels == 4) { m_hasAlphaComponent = true; }

		// Create the OpenGL texture object
		glCreateTextures(GL_TEXTURE_2D, 1, &m_texID);
		// Allocate storage for the texture
		glTextureStorage2D(m_texID, 1, (m_hasAlphaComponent ? GL_RGBA8 : GL_RGB8), width, height);
		// Upload actual pixel data to the texture
		glTextureSubImage2D(m_texID, 0, 0, 0, width, height, (m_hasAlphaComponent ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, pixels);

		// Sets wrap mode so the texture will tile instead of clamping
		glTextureParameteri(m_texID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_texID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_texID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_texID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	}

	Texture::~Texture() {
		if (m_texID != 0)
			glDeleteTextures(1, &m_texID);
	}

	void Texture::Bind(GLuint unit) const {
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, m_texID);
	}

	GLuint Texture::GetID() const {
		return m_texID;
	}

	bool Texture::HasAlpha() const {
		return m_hasAlphaComponent;
	}

	int Texture::GetColumns() const {
		return m_columns;
	}

	int Texture::GetRows() const {
		return m_rows;
	}

	int Texture::GetWidth() const {
		return m_width;
	}

	int Texture::GetHeight() const {
		return m_height;
	}

}	// NightSnack

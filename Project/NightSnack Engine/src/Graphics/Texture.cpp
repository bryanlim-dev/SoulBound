#include "Texture.h"
#include <iostream>

// Constructor to create an OpenGL texture --------------------------------------------------------
// ------------------------------------------------------------------------------------------------
Texture::Texture(const unsigned char* data, const int width, const int height, const int channels) {
	if (!data) {
		std::cerr << "Error: Invalid image resource passed to Texture\n";
		m_texID = 0;
		return;
	}

	// Only enable alpha blend for png textures
	if (channels == 4) // rgba
		m_hasAlphaComponent = true;

	// Create the OpenGL texture object
	glCreateTextures(GL_TEXTURE_2D, 1, &m_texID);

	// Allocate storage for the texture
	glTextureStorage2D(m_texID, 1,
		((channels == 3) ? GL_RGB8 : GL_RGBA8),
		width, height);

	// Upload actual pixel data to the texture
	glTextureSubImage2D(m_texID, 0, 0, 0, 
		width, height,
		((channels == 3) ? GL_RGB : GL_RGBA),
		GL_UNSIGNED_BYTE, data);

	// Sets default wrap mode - clamp to edge
	glTextureParameteri(m_texID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_texID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

// Destructor -------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
Texture::~Texture() {
	if (m_texID != 0)
		glDeleteTextures(1, &m_texID);
}

//Texture::Texture(Texture&& other) noexcept
//	: m_texID(other.m_texID), m_hasAlphaComponent(other.m_hasAlphaComponent) {
//	other.m_texID = 0; // take ownership
//}
//
//Texture& Texture::operator=(Texture&& other) noexcept {
//	if (this != &other) {
//		if (m_texID != 0) {
//			glDeleteTextures(1, &m_texID);
//		}
//		m_texID = other.m_texID;
//		m_hasAlphaComponent = other.m_hasAlphaComponent;
//		other.m_texID = 0;
//	}
//	return *this;
//}

// Binds the texture to the specified texture unit ------------------------------------------------
// ------------------------------------------------------------------------------------------------
void Texture::Bind(GLuint unit) const {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, m_texID);
}

// Get the texture ID -----------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
GLuint Texture::GetID() const { 
	return m_texID;
}

// Get whether this texture has an Alpha component ------------------------------------------------
// ------------------------------------------------------------------------------------------------
bool Texture::HasAlpha() const { 
	return m_hasAlphaComponent; 
}

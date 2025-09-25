#include "Texture.h"

// Constructor to create an OpenGL texture --------------------------------------------------------
// ------------------------------------------------------------------------------------------------
Texture::Texture(std::string textureImage) {

	if (textureImage.empty()) {
		std::cerr << "Error: Texture filename where??\n";
		m_texID = 0;
		return;
	}

	// ResourceManager stuff...
	size_t key = 0;
	ResourceManager* rm = ResourceManager::GetInstance();
	rm->LoadImage(textureImage, key);

	ResourceImage* img = rm->GetImageResource(key);
	unsigned char* pixels = rm->GetImage(key);
	if (!img || !pixels) {
		std::cerr << "Error: Failed to load image " << textureImage << "\n";
		m_texID = 0;
		return;
	}

	// Actual Texture stuff...
	if (img->Channels() == 4) { m_hasAlphaComponent = true; }
	int width = img->Width(), height = img->Height();

	// Create the OpenGL texture object
	glCreateTextures(GL_TEXTURE_2D, 1, &m_texID);
	// Allocate storage for the texture
	glTextureStorage2D(m_texID, 1, (m_hasAlphaComponent ? GL_RGBA8 : GL_RGB8), width, height);
	// Upload actual pixel data to the texture
	glTextureSubImage2D(m_texID, 0, 0, 0, width, height, (m_hasAlphaComponent ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, pixels);

	// Sets default wrap mode - clamp to edge
	glTextureParameteri(m_texID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_texID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_texID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(m_texID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

}

// Destructor -------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
Texture::~Texture() {
	if (m_texID != 0)
		glDeleteTextures(1, &m_texID);
}

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

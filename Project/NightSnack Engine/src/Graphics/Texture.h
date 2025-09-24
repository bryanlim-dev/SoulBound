#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

#include "ResourceManager/ResourceManager.h"

class Texture {

public:

	//Texture() {}
	// Constructor to create an OpenGL texture
	Texture(std::string);
	Texture(const unsigned char* data, const int width, const int height, const int channels);
	~Texture();

	//Texture(const Texture&) = delete; // no copy
	//Texture& operator=(const Texture&) = delete; // no copy
	//Texture(Texture&& other) noexcept; // move
	//Texture& operator=(Texture&& other) noexcept; // move

	// Binds the texture to the specified texture unit
	void Bind(GLuint unit = 0) const;

	GLuint GetID() const;
	bool HasAlpha() const;

private:

	GLuint m_texID{};
	bool m_hasAlphaComponent{ false };

};

#endif // TEXTURE_H
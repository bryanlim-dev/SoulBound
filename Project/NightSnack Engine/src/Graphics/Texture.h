#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

#include "ResourceManager/ResourceManager.h"

class Texture {

public:

	// Constructor to create an OpenGL texture
	Texture(std::string);
	~Texture();

	// Binds the texture to the specified texture unit
	void Bind(GLuint unit = 0) const;

	GLuint GetID() const;
	bool HasAlpha() const;

private:

	GLuint m_texID{};
	bool m_hasAlphaComponent{ false };

};

#endif // TEXTURE_H
#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

class Texture {

public:

	// Constructor to create an OpenGL texture
	Texture(const unsigned char* data, const int width, const int height, const int channels);
	~Texture();

	// Binds the texture to the specified texture unit
	void Bind(GLuint unit) const;

	GLuint GetID() const;
	bool HasAlpha() const;

private:

	GLuint m_texID{};
	bool m_hasAlphaComponent{ false };

};

#endif // TEXTURE_H
#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

class Mesh {

public:

	Mesh(GLenum primitiveType, 
		const std::vector<glm::vec2>& positions, 
		const std::vector<unsigned int>& indices);
	~Mesh();

	void Draw() const;

	// Helper functions to create basic debug shapes of unit length
	static Mesh CreatePoint();
	static Mesh CreateLine();
	static Mesh CreateSquare();
	static Mesh CreateCircle();

	GLsizei indexCount{ 0 }, vertexCount{ 0 };
private:

	GLenum primitiveType = GL_TRIANGLES;
	GLuint vao{ 0 }, vboPos{ 0 }, vboColor{ 0 }, ebo{ 0 };

};
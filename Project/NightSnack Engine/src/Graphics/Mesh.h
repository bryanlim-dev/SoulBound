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

	GLsizei m_indexCount{ 0 }, m_vertexCount{ 0 };
private:

	GLenum m_primitiveType = GL_TRIANGLES;
	GLuint m_vao{ 0 }, m_vboPos{ 0 }, m_vboColor{ 0 }, m_ebo{ 0 };

};
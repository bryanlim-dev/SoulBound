#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

class Mesh {

public:

	// Contains geometry data
	struct Vertex {
		glm::vec2 position;
		glm::vec2 texCoord;
	};

	Mesh() {}
	// Constructs mesh and sets up vertex array and buffer objects for rendering
	Mesh(GLenum primitiveType, 
		std::vector<Vertex> const& vertices,
		std::vector<unsigned int> const& indices);
	~Mesh();

	void Draw() const;

	// Helper functions to create basic debug shapes of unit length
	static Mesh CreatePoint();
	static Mesh CreateLine(const glm::vec2& startPos, const glm::vec2& endPos);
	static Mesh CreateSquare();
	static Mesh CreateCircle();

	GLsizei m_indexCount{ 0 }, m_vertexCount{ 0 };
private:

	GLenum m_primitiveType = GL_TRIANGLES;
	GLuint m_vao{ 0 };
	GLuint m_vbo{ 0 };
	GLuint m_ebo{ 0 };

};
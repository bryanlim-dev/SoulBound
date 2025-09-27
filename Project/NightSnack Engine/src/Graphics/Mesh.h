#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

class Mesh {

public:
	// Contains geometry data
	struct Vertex {
		glm::vec3 position;
		glm::vec2 texCoord;
	};

	// Disallow copy;
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	// Move Semantics;
	Mesh(Mesh&& other) noexcept
		: m_primitiveType(other.m_primitiveType),
		m_vao(other.m_vao),
		m_vbo(other.m_vbo),
		m_ebo(other.m_ebo),
		m_vertexCount(other.m_vertexCount),
		m_indexCount(other.m_indexCount)
	{
		// zero out source so its destructor won't delete handles
		other.m_vao = 0;
		other.m_vbo = 0;
		other.m_ebo = 0;
	}

	Mesh& operator=(Mesh&& other) noexcept {
		if (this != &other) {
			// delete existing
			if (m_ebo) glDeleteBuffers(1, &m_ebo);
			if (m_vbo) glDeleteBuffers(1, &m_vbo);
			if (m_vao) glDeleteVertexArrays(1, &m_vao);

			// steal
			m_primitiveType = other.m_primitiveType;
			m_vao = other.m_vao;
			m_vbo = other.m_vbo;
			m_ebo = other.m_ebo;
			m_vertexCount = other.m_vertexCount;
			m_indexCount = other.m_indexCount;

			other.m_vao = 0;
			other.m_vbo = 0;
			other.m_ebo = 0;
		}
		return *this;
	}

	// Constructs mesh and sets up vertex array and buffer objects for rendering
	Mesh(GLenum primitiveType, 
		std::vector<Vertex> const& vertices,
		std::vector<unsigned int> const& indices);
	~Mesh();

	void Draw() const;

	// Helper functions to create basic debug shapes of unit length
	static std::unique_ptr<Mesh> CreatePoint();
	static std::unique_ptr<Mesh> CreateLine(const glm::vec3& startPos, const glm::vec3& endPos);
	static std::unique_ptr<Mesh> CreateSquare();
	static std::unique_ptr<Mesh> CreateCircle();

	GLsizei m_indexCount{ 0 }, m_vertexCount{ 0 };

private:
	GLuint m_vao{ 0 };
	GLenum m_primitiveType = GL_TRIANGLES;
	GLuint m_vbo{ 0 };
	GLuint m_ebo{ 0 };

};
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

	static Mesh CreatePoint(const glm::vec2& pos);
	static Mesh CreateLine(const glm::vec2& startPos, const glm::vec2& endPos);
	static Mesh CreateRectangle(const glm::vec2& centerPos, const float width, const float height);
	static Mesh CreateCircle(const glm::vec2& centerPos, const float radius);

	GLsizei indexCount{ 0 }, vertexCount{ 0 };
private:

	GLenum primitiveType = GL_TRIANGLES;
	GLuint vao{ 0 }, vboPos{ 0 }, vboColor{ 0 }, ebo{ 0 };

};
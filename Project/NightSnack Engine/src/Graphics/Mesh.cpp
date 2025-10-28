/******************************************************************************
 * File:        Mesh.cpp
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created Mesh class
 *
 * Description:
 * This Mesh class represents renderable geometry data.
 * A Mesh stores vertex, index and primitive information and sets up the OpenGL 
 * VAO, VBO, and optional EBO. 
 * It also provides helper functions for creating shapes such as points, lines, 
 * rectangles, circles. 
 * 
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "Mesh.h"
#include <iostream>

#include <GLFW/glfw3.h>

namespace NightSnack {

	Mesh::Mesh(GLenum primitiveType,
		std::vector<Vertex> const& vertices,
		std::vector<unsigned int> const& indices)
		: m_primitiveType(primitiveType)
	{
		m_indexCount = static_cast<GLsizei>(indices.size());
		m_vertexCount = static_cast<GLsizei>(vertices.size());

		// Create VAO
		glCreateVertexArrays(1, &m_vao);

		// Create VBO
		glCreateBuffers(1, &m_vbo);
		glNamedBufferStorage(m_vbo, vertices.size() * sizeof(Vertex),
			vertices.data(), 0);

		// Bind vertex buffer to VAO
		glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, sizeof(Vertex));

		// Position attribute (location = 0)
		glEnableVertexArrayAttrib(m_vao, 0);
		glVertexArrayAttribFormat(m_vao, 0, 3, GL_FLOAT, GL_FALSE,
			offsetof(Vertex, position));
		glVertexArrayAttribBinding(m_vao, 0, 0);

		// TexCoord attribute (location = 1)
		glEnableVertexArrayAttrib(m_vao, 1);
		glVertexArrayAttribFormat(m_vao, 1, 2, GL_FLOAT, GL_FALSE,
			offsetof(Vertex, texCoord));
		glVertexArrayAttribBinding(m_vao, 1, 0);

		// Optional EBO
		if (!indices.empty()) {
			glCreateBuffers(1, &m_ebo);
			glNamedBufferStorage(m_ebo,
				indices.size() * sizeof(unsigned int),
				indices.data(), 0);

			// Try DSA binding first
			glVertexArrayElementBuffer(m_vao, m_ebo);

			// Verify binding
			GLint boundEBO = 0;
			glGetVertexArrayiv(m_vao, GL_ELEMENT_ARRAY_BUFFER_BINDING, &boundEBO);

			//if (boundEBO == 0) {
			//	std::cerr << "[Mesh] DSA element buffer binding failed, "
			//		<< "falling back to legacy bind.\n";

			//	// Legacy fallback
			//	glBindVertexArray(m_vao);
			//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
			//	glBindVertexArray(0);
			//}
		}

	}

	Mesh::~Mesh() {
		if (m_ebo)
			glDeleteBuffers(1, &m_ebo);
		if (m_vbo)
			glDeleteBuffers(1, &m_vbo);
		if (m_vao)
			glDeleteVertexArrays(1, &m_vao);
	}

	void Mesh::Draw() const {
		glBindVertexArray(m_vao);

		// Call the correct draw call based on the primitive type
		switch (m_primitiveType) {
		case GL_POINTS:
			glPointSize(10.f);
			glDrawArrays(GL_POINTS, 0, m_vertexCount);
			glPointSize(1.f);
			break;

		case GL_LINES:
			glDrawArrays(GL_LINES, 0, m_vertexCount);
			break;

		case GL_TRIANGLES:
			if (m_ebo)
				glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
			else
				glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
			break;

		case GL_TRIANGLE_FAN:
			glDrawArrays(GL_TRIANGLE_FAN, 0, m_vertexCount);
			break;
		}

		glBindVertexArray(0);
	}

	void Mesh::DrawInstanced(size_t instanceCount) const {
		glBindVertexArray(m_vao);

		// Call the correct draw call based on the primitive type
		switch (m_primitiveType) {
		case GL_POINTS:
			glPointSize(10.f);
			glDrawArraysInstanced(GL_POINTS, 0, m_vertexCount, instanceCount);
			glPointSize(1.f);
			break;

		case GL_LINES:
			glDrawArraysInstanced(GL_LINES, 0, m_vertexCount, instanceCount);
			break;

		case GL_TRIANGLES:
			if (m_ebo)
				glDrawElementsInstanced(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr, instanceCount);
			else
				glDrawArraysInstanced(GL_TRIANGLES, 0, m_vertexCount, instanceCount);
			break;

		case GL_TRIANGLE_FAN:
			glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, m_vertexCount, instanceCount);
			break;
		}

		glBindVertexArray(0);
	}

	std::unique_ptr<Mesh> Mesh::CreatePoint() {
		// Default position is at the center of the screen
		std::vector<Vertex> vertices{
			{ glm::vec3(0.f, 0.f, 0.f), // position
			glm::vec2(0.f, 0.f) } // dummy texCoord
		};

		// No indices since we will be using glDrawArrays for points
		return std::make_unique<Mesh>(GL_POINTS, vertices, std::vector<unsigned int>{});
	}

	std::unique_ptr<Mesh> Mesh::CreateLine(const glm::vec3& startPos, const glm::vec3& endPos) {
		std::vector<Vertex> vertices{
			{ startPos, glm::vec2(0.f, 0.f) }, // dummy texCoord
			{ endPos, glm::vec2(0.f, 0.f) }
		};

		// No indices since we will be using glDrawArrays for lines
		return std::make_unique<Mesh>(GL_LINES, vertices, std::vector<unsigned int>{});
	}

	std::unique_ptr<Mesh> Mesh::CreateSquare() {
		// Default position is at the center of the screen
		// Order of texCoords must follow the order of vertices

		std::vector<Vertex> vertices{ // position, texCoord
			{ { 0.5f, 0.5f, 0.f }, { 1.f, 1.f } },	// Top right
			{ { -0.5f, 0.5f, 0.f }, { 0.f, 1.f } },	// Top left
			{ { -0.5f, -0.5f, 0.f }, { 0.f, 0.f } }, // Bottom left
			{ { 0.5f, -0.5f, 0.f }, { 1.f, 0.f } }	// Bottom right
		};

		std::vector<unsigned int> indices{ 0,1,2,2,3,0 };

		return std::make_unique<Mesh>(GL_TRIANGLES, vertices, indices);
	}

	std::unique_ptr<Mesh> Mesh::CreateCircle() {
		glm::vec3 centerPos{ 0.f, 0.f, 0.f };
		const float radius{ 1.f };

		std::vector<Vertex> vertices{
			{ centerPos, glm::vec2(0.5f, 0.5f) } // texCoord starts from the middle too
		};

		int slices = 30;
		float degree = 360.f / slices;
		for (int i{ 0 }; i <= slices; ++i) {
			float rad = glm::radians(i * degree);

			glm::vec3 pos(centerPos.x + radius * cos(rad),
				centerPos.y + radius * sin(rad),
				0.f);

			glm::vec2 tex(0.5f + 0.5f * cos(rad),
				0.5f + 0.5f * sin(rad));

			vertices.push_back({ pos, tex });
		}

		// No indices since we will be using glDrawArrays for triangle fans
		return std::make_unique<Mesh>(GL_TRIANGLE_FAN, vertices, std::vector<unsigned int>{});
	}

	std::unique_ptr<Mesh> Mesh::CreateAABBMesh() {
		// Default position is at the center of the screen
		// Order of texCoords must follow the order of vertices

		std::vector<Vertex> vertices{ // position, texCoord
			{ { 0.5f, 0.5f, 0.f }, { 1.f, 1.f } },	// Top right
			{ { -0.5f, 0.5f, 0.f }, { 0.f, 1.f } },	// Top left
			{ { -0.5f, 0.5f, 0.f }, { 0.f, 1.f } },	// Top left
			{ { -0.5f, -0.5f, 0.f }, { 0.f, 0.f } }, // Bottom left
			{ { -0.5f, -0.5f, 0.f }, { 0.f, 0.f } }, // Bottom left
			{ { 0.5f, -0.5f, 0.f }, { 1.f, 0.f } },	// Bottom right
			{ { 0.5f, -0.5f, 0.f }, { 1.f, 0.f } },	// Bottom right
			{ { 0.5f, 0.5f, 0.f }, { 1.f, 1.f } }	// Top right
		};

		// No indices since we will be using glDrawArrays for GL_LINES
		return std::make_unique<Mesh>(GL_LINES, vertices, std::vector<unsigned int>{});
	}

	std::unique_ptr<Mesh> Mesh::CreateCircleMesh() {

		glm::vec3 centerPos{ 0.f, 0.f, 0.f };
		const float radius{ 1.f };
		std::vector<Vertex> vertices;

		int slices = 30;
		float degree = 360.f / slices;
		for (int i{ 0 }; i <= slices; ++i) {
			float rad = glm::radians(i * degree);

			glm::vec3 pos(centerPos.x + radius * cos(rad),
				centerPos.y + radius * sin(rad),
				0.f);

			glm::vec2 tex(0.5f + 0.5f * cos(rad),
				0.5f + 0.5f * sin(rad));

			vertices.push_back({ pos, tex });

			// Push back the same position twice
			if (vertices.size() > 1 || slices != 30) // Except for the first and last vertice
				vertices.push_back({ pos, tex });
		}

		// No indices since we will be using glDrawArrays for GL_LINES
		return std::make_unique<Mesh>(GL_LINES, vertices, std::vector<unsigned int>{});
	}

}	// NightSnack

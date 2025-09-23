#include "Mesh.h"

// Construct a Mesh with the actual geometry data -------------------------------------------------
// Sets up the VAO, VBO, and EBO for rendering ----------------------------------------------------
// ------------------------------------------------------------------------------------------------
Mesh::Mesh(GLenum primitiveType, std::vector<Vertex> const& vertices, std::vector<unsigned int> const& indices) : 
m_primitiveType(primitiveType) {
	// The number of indices, so we can use it for glDrawElements()
	m_indexCount = static_cast<GLsizei>(indices.size());
	m_vertexCount = static_cast<GLsizei>(vertices.size());

	// Create VAO
	glCreateVertexArrays(1, &m_vao);

	// Create VBO
	glCreateBuffers(1, &m_vbo);
	glNamedBufferStorage(m_vbo, vertices.size() * sizeof(Vertex), vertices.data(), 0);

	// Bind vertex buffer to VAO
	glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, sizeof(Vertex));

	// Position attribute (location = 0)
	glEnableVertexArrayAttrib(m_vao, 0);
	glVertexArrayAttribFormat(m_vao, 0, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
	glVertexArrayAttribBinding(m_vao, 0, 0);

	// TexCoord attribute (location = 1)
	glEnableVertexArrayAttrib(m_vao, 1);
	glVertexArrayAttribFormat(m_vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texCoord));
	glVertexArrayAttribBinding(m_vao, 1, 0);

	// Create EBO (optional if using indices)
	if (!indices.empty()) {
		glCreateBuffers(1, &m_ebo);
		glNamedBufferStorage(m_ebo, indices.size() * sizeof(unsigned int), indices.data(), 0);
		glVertexArrayElementBuffer(m_vao, m_ebo);
	}
}

// Destructor -------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
Mesh::~Mesh() {
	if (m_ebo)
		glDeleteBuffers(1, &m_ebo);
	if (m_vbo)
		glDeleteBuffers(1, &m_vbo);
	if (m_vao)
		glDeleteVertexArrays(1, &m_vao);
}

// Render the mesh according to its primitive type ------------------------------------------------
// ------------------------------------------------------------------------------------------------
void Mesh::Draw() const {
	glBindVertexArray(m_vao);

	switch (m_primitiveType) {
	case GL_POINTS:
		glPointSize(10.f);
		glDrawArrays(GL_POINTS, 0, m_vertexCount);
		glPointSize(1.f);
		break;

	case GL_LINES:
		glLineWidth(3.f);
		glDrawArrays(GL_LINES, 0, m_vertexCount);
		glLineWidth(1.f);
		break;

	case GL_TRIANGLES:
		if (m_ebo)
			glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
		else
			glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
		break;

	case GL_TRIANGLE_FAN:
		glDrawArrays(GL_TRIANGLE_FAN, 0, m_vertexCount);
		break;
	}

	glBindVertexArray(0);
}

// Helper function to create a mesh representing a point at the center of the screen --------------
// ------------------------------------------------------------------------------------------------
Mesh Mesh::CreatePoint() {
	// Default position is at the center of the screen
	std::vector<Vertex> vertices{
		{ glm::vec2(0.f, 0.f), // position
		glm::vec2(0.f, 0.f) } // dummy texCoord
	};

	// No indices since we will be using glDrawArrays for points
	return Mesh(GL_POINTS, vertices, {});
}

// Helper function to create a mesh representing a line -------------------------------------------
// ------------------------------------------------------------------------------------------------
Mesh Mesh::CreateLine(const glm::vec2& startPos, const glm::vec2& endPos) {
	std::vector<Vertex> vertices{
		{ startPos, glm::vec2(0.f, 0.f) }, // dummy texCoord
		{ endPos, glm::vec2(0.f, 0.f) }
	};
	
	// No indices since we will be using glDrawArrays for lines
	return Mesh(GL_LINES, vertices, {});
}

// Helper function to create a mesh representing a rectangle at the center of the screen ----------
// ------------------------------------------------------------------------------------------------
Mesh Mesh::CreateSquare() {
	// Default position is at the center of the screen
	// Order of texCoords must follow the order of vertices

	std::vector<Vertex> vertices{ // position, texCoord
		{ { 0.5f, 0.5f }, { 1.f, 1.f } },	// Top right
		{ { -0.5f, 0.5f }, { 0.f, 1.f } },	// Top left
		{ { -0.5f, -0.5f }, { 0.f, 0.f } }, // Bottom left
		{ { 0.5f, -0.5f }, { 1.f, 0.f } }	// Bottom right
	};

	std::vector<unsigned int> indices{ 0,1,2,2,3,0 };

	return Mesh(GL_TRIANGLES, vertices, indices);
}

// Helper function to create a mesh representing a circle at the center of the screen -------------
// ------------------------------------------------------------------------------------------------
Mesh Mesh::CreateCircle() {
	glm::vec2 centerPos{ 0.f, 0.f };
	const float radius{ 1.f };

	std::vector<Vertex> vertices{
		{ centerPos, glm::vec2(0.5f, 0.5f) } // texCoord starts from the middle too
	};

	int slices = 30;
	float degree = 360.f / slices;
	for (int i{ 0 }; i <= slices; ++i) {
		float rad = glm::radians(i * degree);

		glm::vec2 pos(centerPos.x + radius * cos(rad),
					centerPos.y + radius * sin(rad));

		glm::vec2 tex(0.5f + 0.5f * cos(rad),
					0.5f + 0.5f * sin(rad));

		vertices.push_back({ pos, tex });
	}

	// No indices since we will be using glDrawArrays for triangle fans
	return Mesh(GL_TRIANGLE_FAN, vertices, {});
}
#include "Mesh.h"

// Construct a Mesh with the actual geometry data -------------------------------------------------
// Sets up the VAO, VBO, and EBO for rendering ----------------------------------------------------
// ------------------------------------------------------------------------------------------------
Mesh::Mesh(GLenum primitiveType, const std::vector<glm::vec2>& positions, const std::vector<unsigned int>& indices)
	: primitiveType{ primitiveType } {
	
	// The number of indices, so we can use it for glDrawElements()
	indexCount = static_cast<GLsizei>(indices.size());
	// Vertex count used for glDrawArrays()
	vertexCount = static_cast<GLsizei>(positions.size());

	// Create storage buffers
	// Position buffer
	glCreateBuffers(1, &vboPos);
	glNamedBufferStorage(vboPos, positions.size() * sizeof(glm::vec2), positions.data(), 0);
	
	// Index buffer
	if (!indices.empty()) { // Since not all meshes will use indices
		glCreateBuffers(1, &ebo);
		glNamedBufferStorage(ebo, indices.size() * sizeof(unsigned int), indices.data(), 0);
	}

	// Create a VAO
	glCreateVertexArrays(1, &vao);

	// Bind position buffer (binding index 0)
	glVertexArrayVertexBuffer(vao, 0, vboPos, 0, sizeof(glm::vec2));
	glEnableVertexArrayAttrib(vao, 0);
	glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vao, 0, 0); // Link attribute 0 to binding index 0

	// Attach EBO to VAO
	if (ebo) {
		glVertexArrayElementBuffer(vao, ebo);
	}
}

// Destructor -------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
Mesh::~Mesh() {
	if (vao)
		glDeleteVertexArrays(1, &vao);
	if (vboPos)
		glDeleteBuffers(1, &vboPos);
	if (ebo)
		glDeleteBuffers(1, &ebo);
}

// Render the mesh according to its primitive type ------------------------------------------------
// ------------------------------------------------------------------------------------------------
void Mesh::Draw() const {
	glBindVertexArray(vao);

	switch (primitiveType) {
	case GL_POINTS:
		glPointSize(10.f);
		glDrawArrays(GL_POINTS, 0, vertexCount);
		glPointSize(1.f);
		break;

	case GL_LINES:
		glLineWidth(3.f);
		glDrawArrays(GL_LINES, 0, vertexCount);
		glLineWidth(1.f);
		break;

	case GL_TRIANGLES:
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
		break;

	case GL_TRIANGLE_FAN:
		glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
		break;
	}

	glBindVertexArray(0);
}

// Helper function to create a mesh representing a point at the center of the screen --------------
// ------------------------------------------------------------------------------------------------
Mesh Mesh::CreatePoint() {
	// Default position is at the center of the screen
	std::vector<glm::vec2> positions{ glm::vec2(0.f, 0.f) };

	// No indices since we will be using glDrawArrays for points
	return Mesh(GL_POINTS, positions, {});
}

// Helper function to create a mesh representing a horizontal line at the center of the screen ----
// ------------------------------------------------------------------------------------------------
Mesh Mesh::CreateLine() {
	std::vector<glm::vec2> positions{ glm::vec2(0.f, 0.f), glm::vec2(1.f, 0.f) };
	
	// No indices since we will be using glDrawArrays for lines
	return Mesh(GL_LINES, positions, {});
}

// Helper function to create a mesh representing a rectangle at the center of the screen ----------
// ------------------------------------------------------------------------------------------------
Mesh Mesh::CreateSquare() {
	const glm::vec2 centerPos{ 0.f, 0.f };
	const float width = 1.f;
	const float height = 1.f;

	std::vector<glm::vec2> positions{
		glm::vec2(centerPos.x + width * 0.5f, centerPos.y + height * 0.5f), // Top right
		glm::vec2(centerPos.x - width * 0.5f, centerPos.y + height * 0.5f), // Top left
		glm::vec2(centerPos.x - width * 0.5f, centerPos.y - height * 0.5f), // Bottom left
		glm::vec2(centerPos.x + width * 0.5f, centerPos.y - height * 0.5f)  // Bottom right
	};
	std::vector<unsigned int> indices{ 0,1,2,2,3,0 };

	return Mesh(GL_TRIANGLES, positions, indices);
}

// Helper function to create a mesh representing a circle at the center of the screen -------------
// ------------------------------------------------------------------------------------------------
Mesh Mesh::CreateCircle() {
	glm::vec2 centerPos{ 0.f, 0.f };
	const float radius{ 1.f };

	std::vector<glm::vec2> positions{
		centerPos // Starting position is the center of the circle
	};

	int slices = 20;
	float degree = 360.f / slices;
	for (int i{ 0 }; i <= slices; ++i) {
		positions.emplace_back(glm::vec2(
			centerPos.x + radius * cos(glm::radians(i * degree)),
			centerPos.y + radius * sin(glm::radians(i * degree)))
		);
	}

	// No indices since we will be using glDrawArrays for triangle fans
	return Mesh(GL_TRIANGLE_FAN, positions, {});
}
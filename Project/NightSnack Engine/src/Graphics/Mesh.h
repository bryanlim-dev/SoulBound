/******************************************************************************
 * File:        Mesh.h
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

	/************************************************************************/
	/*!
	\brief
	Constructor for the Mesh class.
	Initializes a mesh with given vertices, indices, and primitive type.
	Creates VAO, VBO, EBO.

	\param[in] primitiveType
	The OpenGL primitive type used for rendering (GL_TRIANGLES etc)

	\param[in] vertices
	Vector of Vertex structs containing position and texCoord attributes

	\param[in] indices
	Vector of unsigned int representing the indices that define mesh element order
	*/
	/************************************************************************/
	Mesh(GLenum primitiveType, 
		std::vector<Vertex> const& vertices,
		std::vector<unsigned int> const& indices);

	/************************************************************************/
	/*!
	\brief
	Destructor for the Mesh class.
	Deletes the VAO, VBO, EBO associated with this Mesh
	*/
	/************************************************************************/
	~Mesh();

	/************************************************************************/
	/*!
	\brief
	Renders the mesh. 
	Calls the correct draw call based on the mesh's primitive type
	*/
	/************************************************************************/
	void Draw() const;

	/************************************************************************/
	/*!
	\brief
	Helper function to create a mesh consisting of a single point at the center
	of the screen, of unit length.

	\return
	Unique pointer to a Mesh configured as a GL_POINTS primitive
	*/
	/************************************************************************/
	static std::unique_ptr<Mesh> CreatePoint();

	/************************************************************************/
	/*!
	\brief
	Helper function to create a mesh consisting of a line between 2 positions.

	\return
	Unique pointer to a Mesh configured as a GL_LINES primitive
	*/
	/************************************************************************/
	static std::unique_ptr<Mesh> CreateLine(const glm::vec3& startPos, const glm::vec3& endPos);
	
	/************************************************************************/
	/*!
	\brief
	Helper function to create a mesh consisting of a square, consisting of 2
	triangles, at the center of the screen, of unit length.
	Includes texture coordinates too.

	\return
	Unique pointer to a Mesh configured as a GL_TRIANGLES primitive
	*/
	/************************************************************************/
	static std::unique_ptr<Mesh> CreateSquare();

	/************************************************************************/
	/*!
	\brief
	Helper function to create a mesh consisting of a circle, using a triangle fan,
	at the center of the screen, of unit length.

	\return
	Unique pointer to a Mesh configured as a GL_TRIANGLE_FAN primitive
	*/
	/************************************************************************/
	static std::unique_ptr<Mesh> CreateCircle();

	/************************************************************************/
	/*!
	\brief
	Helper function to create a mesh that represents the outline of an AABB collider.

	\return
	Unique pointer to a Mesh configured as a GL_LINES primitive
	*/
	/************************************************************************/
	static std::unique_ptr<Mesh> CreateAABBMesh();

	/************************************************************************/
	/*!
	\brief
	Helper function to create a mesh that represents the outline of a circle collider.

	\return
	Unique pointer to a Mesh configured as a GL_LINES primitive
	*/
	/************************************************************************/
	static std::unique_ptr<Mesh> CreateCircleMesh();

	GLsizei m_indexCount{ 0 }, m_vertexCount{ 0 };

private:
	GLuint m_vao{ 0 };
	GLenum m_primitiveType = GL_TRIANGLES;
	GLuint m_vbo{ 0 };
	GLuint m_ebo{ 0 };

};
/******************************************************************************
 * File:        Shader.h
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created Shader class
 *
 * Description:
 * This Shader class is the Shader Manager, where it loads GLSL source files like the 
 * vertex and fragment shaders, and compile and link them into a program. 
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <glm/glm.hpp>

class Shader {

public:

	/************************************************************************/
	/*!
	\brief
	Constructor for the Shader class.
	Loads, compiles, and links vertex and fragment shaders into a complete shader program

	\param[in] vertexPath
	File path to the vertex shader

	\param[in] fragmentPath
	File path to the fragment shader
	*/
	/************************************************************************/
	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	
	/************************************************************************/
	/*!
	\brief
	Destructor for the Shader class
	Deletes the OpenGL shader program
	*/
	/************************************************************************/
	~Shader();

	/************************************************************************/
	/*!
	\brief
	Activates this shader program for subsequent rendering draw calls
	All OpenGL rendering after calling this will use this shader program
	*/
	/************************************************************************/
	void Use() const;

	/************************************************************************/
	/*!
	\brief
	Retrieves the OpenGL program ID of this shader

	\return
	The shader program ID
	*/
	/************************************************************************/
	GLuint GetID() const;

	/************************************************************************/
	/*!
	\brief
	Uploads a 4x4 matrix uniform variable to the shader program

	\param[in] name
	The name of the uniform variable in the shader

	\param[in] mat
	The 4x4 matrix to upload
	*/
	/************************************************************************/
	void SetMat4(const std::string& name, const glm::mat4& mat) const;
	
	/************************************************************************/
	/*!
	\brief
	Uploads a 3x3 matrix uniform variable to the shader program

	\param[in] name
	The name of the uniform variable in the shader

	\param[in] mat
	The 3x3 matrix to upload
	*/
	/************************************************************************/
	void SetMat3(const std::string& name, const glm::mat3& mat) const;
	
	/************************************************************************/
	/*!
	\brief
	Uploads a vec3 uniform variable to the shader program

	\param[in] name
	The name of the uniform variable in the shader

	\param[in] value
	The vec3 to upload
	*/
	/************************************************************************/
	void SetVec3(const std::string& name, const glm::vec3& value) const;
	
	/************************************************************************/
	/*!
	\brief
	Uploads an int uniform variable to the shader program

	\param[in] name
	The name of the uniform variable in the shader

	\param[in] value
	The int to upload
	*/
	/************************************************************************/
	void SetInt(const std::string& name, int value) const;

	/************************************************************************/
	/*!
	\brief
	Uploads a bool uniform variable, using an int to the shader program

	\param[in] name
	The name of the uniform variable in the shader

	\param[in] value
	The int to upload
	*/
	/************************************************************************/
	void SetBool(const std::string& name, bool value) const;

	/************************************************************************/
	/*!
	\brief
	Uploads a float uniform variable to the shader program

	\param[in] name
	The name of the uniform variable in the shader

	\param[in] mat
	The float to upload
	*/
	/************************************************************************/
	void SetFloat(const std::string& name, float value) const;

private:

	/************************************************************************/
	/*!
	\brief
	Reads and loads the contents of a shader source file into a string

	\param[in] path
	File path to the shader source file

	\return
	A string containing the entire shader source code
	*/
	/************************************************************************/
	std::string LoadFile(const std::string& path);

	/************************************************************************/
	/*!
	\brief
	Compiles GLSL source code into an OpenGL shader object of the specified type

	\param[in] src
	The GLSL shader source code as a string

	\param[in] type
	The type of shader to compile (GL_VERTEX_SHADER etc)

	\return
	The ID of the compiled shader object
	*/
	/************************************************************************/
	GLuint CompileShader(const std::string& src, GLenum type);

	/************************************************************************/
	/*!
	\brief
	Links the compiled vertex and fragment shaders into a single OpenGL program

	\param[in] vs
	ID of the compiled vertex shader

	\param[in] fs
	ID of the compiled fragment shader
	*/
	/************************************************************************/
	void LinkProgram(GLuint vs, GLuint fs);

	GLuint programID{ 0 };
};
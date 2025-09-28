#pragma once

#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Load, compile and link shaders into an OpenGL program ------------------------------------------
// ------------------------------------------------------------------------------------------------
Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
	// Load GLSL source code from files
	std::string vsSrc = LoadFile(vertexPath);
	std::string fsSrc = LoadFile(fragmentPath);

	// Compile individual shaders
	GLuint vs = CompileShader(vsSrc, GL_VERTEX_SHADER);
	GLuint fs = CompileShader(fsSrc, GL_FRAGMENT_SHADER);

	// Link the shaders into a complete program
	LinkProgram(vs, fs);

	// Shaders are no longer needed after linking, delete them
	glDeleteShader(vs);
	glDeleteShader(fs);
}

// Destructor -------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
Shader::~Shader() {
	if (programID)
		glDeleteProgram(programID);
}

// Use or make this shader program active for draw calls ------------------------------------------
// ------------------------------------------------------------------------------------------------
void Shader::Use() const {
	glUseProgram(programID);
}

// Get the OpenGL program ID ----------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
GLuint Shader::GetID() const {
	return programID;
}

// Reads a shader source file into a string -------------------------------------------------------
// ------------------------------------------------------------------------------------------------
std::string Shader::LoadFile(const std::string& path) {

	std::ifstream file(path);
	
	if (!file.is_open())
		throw std::runtime_error("Failed to open Shader File: " + path);
	
	std::stringstream ss;
	ss << file.rdbuf();
	file.close();
	return ss.str();
}

// Compiles GLSL source code into a shader object -------------------------------------------------
// ------------------------------------------------------------------------------------------------
GLuint Shader::CompileShader(const std::string& src, GLenum type) {

	GLuint shader = glCreateShader(type);
	const char* csrc = src.c_str();
	glShaderSource(shader, 1, &csrc, nullptr);
	glCompileShader(shader);

	// Check the compile status
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char log[512];
		glGetShaderInfoLog(shader, 512, nullptr, log);
		std::cerr << "Shader compile error: " << log << std::endl;
	}

	return shader;
}

// Links the compiled shaders into a single program -----------------------------------------------
// ------------------------------------------------------------------------------------------------
void Shader::LinkProgram(GLuint vs, GLuint fs) {

	programID = glCreateProgram();
	glAttachShader(programID, vs);
	glAttachShader(programID, fs);
	glLinkProgram(programID);

	// Check the link status
	GLint success;
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success) {
		char log[512];
		glGetProgramInfoLog(programID, 512, nullptr, log);
		std::cerr << "Program link error: " << log << std::endl;
	}
}

// Helper function to upload a 4x4 Matrix uniform to the shader program ---------------------------
// ------------------------------------------------------------------------------------------------
void Shader::SetMat4(const std::string& name, const glm::mat4& mat) const {
	GLint location = glGetUniformLocation(programID, name.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
}

// Helper function to upload a 3x3 Matrix uniform to the shader program ---------------------------
// ------------------------------------------------------------------------------------------------
void Shader::SetMat3(const std::string& name, const glm::mat3& mat) const {
	GLint location = glGetUniformLocation(programID, name.c_str());
	glUniformMatrix3fv(location, 1, GL_FALSE, &mat[0][0]);
}

// Helper function to upload a vec3 uniform to the shader program ---------------------------------
// ------------------------------------------------------------------------------------------------
void Shader::SetVec3(const std::string& name, const glm::vec3& value) const {
	GLint location = glGetUniformLocation(programID, name.c_str());
	glUniform3fv(location, 1, &value[0]);
}

// Helper function to upload an Int uniform to the shader program ---------------------------------
// ------------------------------------------------------------------------------------------------
void Shader::SetInt(const std::string& name, int value) const {
	GLint location = glGetUniformLocation(programID, name.c_str());
	glUniform1i(location, value);
}
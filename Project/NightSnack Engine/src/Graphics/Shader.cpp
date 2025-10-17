/******************************************************************************
 * File:        Shader.cpp
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

#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace NightSnack {

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

	Shader::~Shader() {
		if (programID)
			glDeleteProgram(programID);
	}

	void Shader::Use() const {
		glUseProgram(programID);
	}

	GLuint Shader::GetID() const {
		return programID;
	}

	std::string Shader::LoadFile(const std::string& path) {

		std::ifstream file(path);

		if (!file.is_open())
			throw std::runtime_error("Failed to open Shader File: " + path);

		// Read and loads the contents of the shader source file into a string
		std::stringstream ss;
		ss << file.rdbuf();
		file.close();
		return ss.str();
	}

	GLuint Shader::CompileShader(const std::string& src, GLenum type) {
		// Compile the GLSL source code into an OpenGL shader object
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

	void Shader::LinkProgram(GLuint vs, GLuint fs) {
		// Link the compiled vertex and fragment shaders into a single OpenGL program
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

	void Shader::SetMat4(const std::string& name, const glm::mat4& mat) const {
		GLint location = glGetUniformLocation(programID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
	}

	void Shader::SetMat3(const std::string& name, const glm::mat3& mat) const {
		GLint location = glGetUniformLocation(programID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, &mat[0][0]);
	}

	void Shader::SetVec3(const std::string& name, const glm::vec3& value) const {
		GLint location = glGetUniformLocation(programID, name.c_str());
		glUniform3fv(location, 1, &value[0]);
	}

	void Shader::SetInt(const std::string& name, int value) const {
		GLint location = glGetUniformLocation(programID, name.c_str());
		glUniform1i(location, value);
	}

	void Shader::SetBool(const std::string& name, bool value) const {
		GLint location = glGetUniformLocation(programID, name.c_str());
		glUniform1i(location, (int)value);
	}

	void Shader::SetFloat(const std::string& name, float value) const {
		GLint location = glGetUniformLocation(programID, name.c_str());
		glUniform1f(location, value);
	}

}	// NightSnack

/******************************************************************************
 * File:        Shader.h
 * Project:     NightSnack
 * Author(s):
 *
 * Description:
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

	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	~Shader();

	void Use() const;
	GLuint GetID() const;

	// Uniform helpers
	void SetMat4(const std::string& name, const glm::mat4& mat) const;
	void SetMat3(const std::string& name, const glm::mat3& mat) const;
	void SetVec3(const std::string& name, const glm::vec3& value) const;
	void SetInt(const std::string& name, int value) const;

    void SetBool(std::string const& name, bool value) const {
        glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
    }
    void SetFloat(std::string const& name, float value) const {
        glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
    }

private:

	GLuint programID{ 0 };

	std::string LoadFile(const std::string& path);
	GLuint CompileShader(const std::string& src, GLenum type);
	void LinkProgram(GLuint vs, GLuint fs);
};
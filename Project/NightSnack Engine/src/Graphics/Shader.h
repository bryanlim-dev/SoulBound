#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>

class Shader {

public:

	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	~Shader();

	void Use() const;
	GLuint GetId() const;

	// Uniform helpers
	void SetMat4(const std::string& name, const glm::mat4& mat) const;
	void SetMat3(const std::string& name, const glm::mat3& mat) const;
	void SetVec3(const std::string& name, const glm::vec3& value) const;
	void SetInt(const std::string& name, int value) const;

private:

	GLuint programID{ 0 };

	std::string LoadFile(const std::string& path);
	GLuint CompileShader(const std::string& src, GLenum type);
	void LinkProgram(GLuint vs, GLuint fs);
};
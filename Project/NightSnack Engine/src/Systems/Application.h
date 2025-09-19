#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "ECS/Coordinator.h"

class ApplicationSystem : public ISystem
{
public:
	ApplicationSystem() {};
	~ApplicationSystem();

	void Init(int width, int height, std::string const& title);
	void Update(double dt, Coordinator& coord) override;
	std::string GetName() const { return "ApplicationSystem"; }

	void PollEvents();
	void SwapBuffers();
	bool ShouldClose() const;

	void SetWindowTitle(GLFWwindow* window, const char* title);
	int GetWidth() const { return m_appWidth; }
	int GetHeight() const { return m_appHeight; }

	GLFWwindow* GetNativeHandle() const { return m_windowHandle; }

	Signature GetSignature() const override { return m_systemSignature; }

private:
	static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

	GLFWwindow* m_windowHandle{ nullptr };
	int m_appWidth { 1600 };
	int m_appHeight { 900 };
	std::string m_appTitle;

	Signature m_systemSignature{};
};

#endif // APPLICATION_HPP
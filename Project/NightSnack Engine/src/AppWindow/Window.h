#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window
{
public:
	Window() = delete;
	Window(int width, int height, std::string const& title);
	~Window();

	void SetTitle(const std::string& title);
	void PollEvents();
	void SwapBuffers();
	bool ShouldClose() const;

	int GetWidth() const { return Width_; }
	int GetHeight() const { return 	Height_; }

private:
	static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

	GLFWwindow* Handle_;
	int Width_;
	int Height_;
	std::string Title_;
};

#endif // WINDOW_HPP
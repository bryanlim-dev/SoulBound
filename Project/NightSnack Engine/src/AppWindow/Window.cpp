#include "Window.h"
#include "InputManager.h"
#include <iostream>

// Required OpenGL version;
static constexpr int REQUIRED_GL_MAJOR = 4;
static constexpr int REQUIRED_GL_MINOR = 5;

// Constructor ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
Window::Window(int width, int height, std::string const& title)
	: Width_(width), Height_(height), Title_(title)
{
	// Initialize GLFW;
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW\n";
		throw std::runtime_error("Failed to initialize GLFW");
	}

	// OpenGL version and profile;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, REQUIRED_GL_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, REQUIRED_GL_MINOR);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Framebuffer settings;
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_RED_BITS, 8); glfwWindowHint(GLFW_GREEN_BITS, 8);
	glfwWindowHint(GLFW_BLUE_BITS, 8); glfwWindowHint(GLFW_ALPHA_BITS, 8);

	// Create window;
	Handle_ = glfwCreateWindow(Width_, Height_, Title_.c_str(), nullptr, nullptr);
	if(!Handle_)
	{
		std::cerr << "Failed to create GLFW window\n";
		glfwTerminate();
		throw std::runtime_error("GLFW window creation failed");
	}

	// Make context current;
	glfwMakeContextCurrent(Handle_);

	// Load OpenGL functions using GLAD;
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD\n";
		throw std::runtime_error("GLAD init failed");
	}

	// Set viewport and callback;
	glViewport(0, 0, width, height);
	glfwSetFramebufferSizeCallback(Handle_, FramebufferSizeCallback);

	// Initialise the Input Manager
	InputManager::Init(Handle_);
}

// Destructor -------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
Window::~Window() {
	// Check if window exists;
	if (Handle_) { glfwDestroyWindow(Handle_); }
	// Terminate program;
	glfwTerminate();
}

// Poll for and process events --------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
void Window::PollEvents()
{
	glfwPollEvents();
}

// Swap front and back buffers --------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
void Window::SwapBuffers()
{
	glfwSwapBuffers(Handle_);
}

// Check if window should close -------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
bool Window::ShouldClose() const
{
	return glfwWindowShouldClose(Handle_);
}

// Changes the viewport when the window is resized ------------------------------------------------
// ------------------------------------------------------------------------------------------------
void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
#include "Application.h"
#include <iostream>

// Required OpenGL version;
static constexpr int REQUIRED_GL_MAJOR = 4;
static constexpr int REQUIRED_GL_MINOR = 5;

void ApplicationSystem::Init(int width, int height, std::string const& title) {
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
	m_appWidth = width;
	m_appHeight = height;
	m_appTitle = title;
	m_windowHandle = glfwCreateWindow(m_appWidth, m_appHeight, m_appTitle.c_str(), nullptr, nullptr);
	if (!m_windowHandle)
	{
		std::cerr << "Failed to create GLFW window\n";
		glfwTerminate();
		throw std::runtime_error("GLFW window creation failed");
	}

	// Make context current;
	glfwMakeContextCurrent(m_windowHandle);

	// Load OpenGL functions using GLAD;
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD\n";
		throw std::runtime_error("GLAD init failed");
	}

	// Set viewport and callback;
	glViewport(0, 0, m_appWidth, m_appHeight);
	glfwSetFramebufferSizeCallback(m_windowHandle, FramebufferSizeCallback);

	// Depth testing
	glEnable(GL_DEPTH_TEST);
	// Depth buffer stores values in NDC [0,1], render the fragment if z is lesser/closer
	glDepthFunc(GL_LESS); 
}

// Destructor -------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
ApplicationSystem::~ApplicationSystem() {
	// Check if window exists;
	if (m_windowHandle) { glfwDestroyWindow(m_windowHandle); }
	// Terminate program;
	glfwTerminate();
}

// Update -----------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
void ApplicationSystem::Update(float dt, Coordinator& coord) {
	(void)dt;
	(void)coord;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Displaying active system timers into Console (temp.)
	//m_timer.DisplaySystemTimers();
}

// Set Window Title -------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
//void Window::SetTitle(const std::string& title)
//{
//	Title_ = title;
//	glfwSetWindowTitle(Handle_, Title_.c_str());
//}

// Poll for and process events --------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
void ApplicationSystem::PollEvents() {
	glfwPollEvents();
}

// Swap front and back buffers --------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
void ApplicationSystem::SwapBuffers() {
	glfwSwapBuffers(m_windowHandle);
}

// Update the window title ------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
void ApplicationSystem::SetWindowTitle(GLFWwindow* window, const char* title) {
	glfwSetWindowTitle(window, title);
}

// Check if window should close -------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
bool ApplicationSystem::ShouldClose() const {
	return glfwWindowShouldClose(m_windowHandle);
}

// Changes the viewport when the window is resized ------------------------------------------------
// ------------------------------------------------------------------------------------------------
void ApplicationSystem::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
	(void)window;
	glViewport(0, 0, width, height);
}
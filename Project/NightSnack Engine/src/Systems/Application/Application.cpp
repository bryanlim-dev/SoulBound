/******************************************************************************
 * File:        Application.cpp
 * Project:     NightSnack
 * Author(s):	Lim Wei Jie (Primary, 100%) - Created Application system to
 *
 * Description: This file contains the definitions of the Application System, 
 *				which manages the application window, OpenGL context, and timing.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "Application.h"
#include <iostream>

namespace NightSnack {

	// Required OpenGL version;
	static constexpr int REQUIRED_GL_MAJOR = 4;
	static constexpr int REQUIRED_GL_MINOR = 5;

	// Default constructor to create a window with given width, height and title.
	ApplicationSystem::ApplicationSystem(int width, int height, std::string const& title, Coordinator& coord, EventDispatcher& dispatcher) {
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

		// Store the event dispatcher
		m_dispatcher = &dispatcher;
		Init(coord);
	}

	// Destructor to clean up and terminate GLFW.
	ApplicationSystem::~ApplicationSystem() {
		// Check if window exists;
		if (m_windowHandle) { glfwDestroyWindow(m_windowHandle); }
		// Terminate program;
		glfwTerminate();
	}

	// Initialize the system and set up event subscriptions
	void ApplicationSystem::Init(Coordinator& Coord) {

		// Subscribe to input events
		m_dispatcher->Subscribe<InputEvent>([this, &Coord](const InputEvent& event) {
			if (event.pressed == InputEvent::InputActionState::Pressed) {
				//switch (event.inputAction) {
				//case InputEvent::InputAction::ToggleSystemTimers:
					// Toggle switch for System Timers Debug Mode
					//systemTimersEnabled = !systemTimersEnabled;
					//break;
				//}
			}
			});
	}

	// Update function called every frame
	void ApplicationSystem::Update(float dt, Coordinator& coord) {
		(void)dt;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		PollEvents();

		size_t livingEntities = coord.GetEntityNum();

		std::string title = "NightSnack Engine | FPS: " + std::to_string(static_cast<int>(GetTimer().GetFPS()))
			+ " | Game Time: " + std::to_string(static_cast<int>(GetTimer().GetTotalSystemTime()))
			+ " | Number of Entities: " + std::to_string(livingEntities);

		SetWindowTitle(GetNativeHandle(), title.c_str());

		if (systemTimersEnabled) { m_timer.DisplaySystemTimers(); }
	}

	// Poll for and process events
	void ApplicationSystem::PollEvents()
	{
		glfwPollEvents();
	}

	// Swap front and back buffers
	void ApplicationSystem::SwapBuffers() {
		glfwSwapBuffers(m_windowHandle);
	}

	// Update the window title
	void ApplicationSystem::SetWindowTitle(GLFWwindow* window, const char* title) {
		glfwSetWindowTitle(window, title);
	}

	// Check if window should close
	bool ApplicationSystem::ShouldClose() const {
		return glfwWindowShouldClose(m_windowHandle);
	}

	// Changes the viewport when the window is resized
	void ApplicationSystem::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
		(void)window;
		glViewport(0, 0, width, height);
	}

}	// NightSnack
/******************************************************************************
 * File:        Application.cpp
 * Project:     NightSnack
 * Author(s):	Lim Wei Jie (Primary, 60%) - Created Application system to
 *			    handle window creation, OpenGL context and Time Handling
 *				Goh Bei Lai (Secondary, 40%) - Implemented Interruption Handling logic. 
 *
 * Description: This file contains the definitions of the Application System,
 *				which manages the application window, OpenGL context, and timing.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "Application.h"
#include "Systems/Graphics.h"
#include "./Factory/EntityFactory.h"
#include <iostream>

namespace NightSnack {

	std::vector<std::string> ApplicationSystem::s_droppedFiles;
	int ApplicationSystem::m_appWidth = 1600;
	int ApplicationSystem::m_appHeight = 900;
	int ApplicationSystem::m_appWindowedWidth = 1600;
	int ApplicationSystem::m_appWindowedHeight = 900;
	int ApplicationSystem::m_appWindowedPosX = 200;
	int ApplicationSystem::m_appWindowedPosY = 130;
	bool ApplicationSystem::m_appToPause = false;
	bool ApplicationSystem::m_appChangedPauseState = false;
	bool ApplicationSystem::m_appIsFullscreen = true; // By default, the application is in full screen
	bool ApplicationSystem::m_appIsFullscreenViaButton = false;
	bool ApplicationSystem::m_pressedMaximizeButton = false;
	bool ApplicationSystem::m_showFPS = false;
	GLFWwindow* ApplicationSystem::m_windowHandle = nullptr;

	// Required OpenGL version;
	static constexpr int REQUIRED_GL_MAJOR = 4;
	static constexpr int REQUIRED_GL_MINOR = 5;

	// Default constructor to create a window with given width, height and title.
	ApplicationSystem::ApplicationSystem(std::string const& title, Coordinator& coord, EventDispatcher& dispatcher)
		: m_coord{ coord }, m_appTitle{ title } {
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
		m_windowHandle = glfwCreateWindow(m_appWidth, m_appHeight, m_appTitle.c_str(), nullptr, nullptr);
		EnterFullscreen(); // By default, start with fullscreen
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
		glfwSetWindowIconifyCallback(m_windowHandle, WindowIconifyCallback); // Application window is minimized
		glfwSetWindowFocusCallback(m_windowHandle, WindowFocusCallback); // Application window loses focus
		glfwSetWindowMaximizeCallback(m_windowHandle, WindowMaximizeCallback); // Application window is maximized

		// file drop callback
		glfwSetDropCallback(m_windowHandle, FileDropCallback);
		// //std::cout << "[Application] File drop callback initialized\n";

		// Disable Depth testing since the primary way of rendering is using Layers 
		glDisable(GL_DEPTH_TEST);
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
				switch (event.inputAction) {
				case InputEvent::InputAction::ToggleFullscreen:
					if (m_appIsFullscreenViaButton) {
						// Maximized via button
						ExitFullscreen();
					} else {
						// Toggled full screen using F11 key press
						m_appIsFullscreen ? ExitFullscreen() : EnterFullscreen();
					}
					break;

				//case InputEvent::InputAction::AltTab:
				//	glfwIconifyWindow(m_windowHandle);
				//	break;

				case InputEvent::InputAction::DisplayFPS:
					m_showFPS = !m_showFPS;
					ToggleFPSDisplay();
					break;
				}
			}
			});
	}

	// Update function called every frame
	void ApplicationSystem::Update() {
		PollEvents();

		// There is a change in application focus state
		if (!m_appChangedPauseState) {
			// Pause the application if the application will be minimized
			if (m_appToPause) {
				m_previousPauseState = m_coord.IsPaused(); // Store the current pause state first, basing on level editor mode
				m_coord.Pause();
			} else if (!m_previousPauseState) {
				// Only unpause the application if the application is being restored 
				// and the application was not currently paused (level editor mode)
				m_coord.Unpause();
			}
			m_appChangedPauseState = true;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//size_t livingEntities = m_coord.GetEntityNum();
		std::string instanced = m_coord.GetSystem<GraphicsSystem>()->GetEnableInstancedRendering() ? "Yes" : "No";

		std::string title = "NightSnack Engine";


		SetWindowTitle(GetNativeHandle(), title.c_str());

		if (systemTimersEnabled) { m_timer.DisplaySystemTimers(); }

		// If the application window is not in fullscreen mode, keep updating the window properties
		if (!m_appIsFullscreen && !m_appIsFullscreenViaButton) {
			glfwGetWindowPos(m_windowHandle, &m_appWindowedPosX, &m_appWindowedPosY);
			glfwGetWindowSize(m_windowHandle, &m_appWindowedWidth, &m_appWindowedHeight);
		}

		if (m_pressedMaximizeButton) {
			// Restore the window first, to update the window's properties like the maximise/minimise button
			glfwRestoreWindow(m_windowHandle);

			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			// Remove borders
			glfwSetWindowAttrib(m_windowHandle, GLFW_DECORATED, GLFW_FALSE);
			glfwSetWindowAttrib(m_windowHandle, GLFW_RESIZABLE, GLFW_FALSE);

			// Resize to monitor
			glfwSetWindowPos(m_windowHandle, 0, 0);
			glfwSetWindowSize(m_windowHandle, mode->width, mode->height);

			m_appHeight = mode->height;
			m_appWidth = mode->width;

			glfwSetWindowMonitor(m_windowHandle, monitor, 0, 0, m_appWidth, m_appHeight, GLFW_DONT_CARE);

			m_pressedMaximizeButton = false;
		}

		// Display FPS object in game if toggled on
		if (m_showFPS && m_fpsEntity != INVALID_ENTITY && m_coord.HasComponent<Font>(m_fpsEntity))
		{
			float fps = GetTimer().GetFPS();
			auto& font = m_coord.ModifyComponent<Font>(m_fpsEntity);
			font.text = "FPS: " + std::to_string(static_cast<int>(fps));
		}
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

	void ApplicationSystem::OnResize(int width, int height) {
		m_appWidth = width;
		m_appHeight = height;
		glViewport(0, 0, width, height);
	}

	// Check if window should close
	bool ApplicationSystem::ShouldClose() const {
		return glfwWindowShouldClose(m_windowHandle);
	}

	// Changes the viewport when the window is resized
	void ApplicationSystem::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
		(void)window;
		m_appWidth = width;
		m_appHeight = height;
		glViewport(0, 0, width, height);
	}

	void ApplicationSystem::WindowIconifyCallback(GLFWwindow* window, int iconified) {
		(void)window;

		// iconified == true means the window is minimized
		m_appToPause = iconified ? true : false;
		m_appChangedPauseState = false;

		// iconified == false means the window is restored
		if (!iconified && m_appIsFullscreen) {
			// This scenario is for when the application was in fullscreen mode when it minimized, and is now being restored 
			// Manually resize the monitor and remove the borders, to avoid windows taskbar from showing up
			EnterFullscreen();
		}
	}

	void ApplicationSystem::WindowFocusCallback(GLFWwindow* window, int focused) {
		//(void)window;
		// User press ALT-TAB or clicked away, causing the window to lose focus
		if (!focused) {
			// Minimize the window whenever the window is not in focus
			glfwIconifyWindow(window);
		}
	}

	void ApplicationSystem::WindowMaximizeCallback(GLFWwindow* window, int maximized) {
		(void)window;
		// This callback is called only when the user pressed the window's maximize button
		if (maximized && !m_appIsFullscreen) {
			m_appIsFullscreenViaButton = true;
			m_pressedMaximizeButton = true;
		}
	}

	void ApplicationSystem::EnterFullscreen() {
		// Enter full screen mode without any window bars
		// This function is called when the user toggles the F11 key to enter fullscreen mode

		if (!m_appIsFullscreen) {
			m_appIsFullscreen = true;
		}

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		// Remove borders
		glfwSetWindowAttrib(m_windowHandle, GLFW_DECORATED, GLFW_FALSE);
		glfwSetWindowAttrib(m_windowHandle, GLFW_RESIZABLE, GLFW_FALSE);

		// Resize to monitor
		glfwSetWindowPos(m_windowHandle, 0, 0);
		glfwSetWindowSize(m_windowHandle, mode->width, mode->height);

		m_appHeight = mode->height;
		m_appWidth = mode->width;
	}

	void ApplicationSystem::ExitFullscreen() {
		// Restore previous windowed mode position and size
		// This function have a slightly different behaviour based on how the screen was maximized

		// Restore the window first, to update the window's properties like the maximise/minimise button
		glfwRestoreWindow(m_windowHandle);

		// Add back borders
		glfwSetWindowAttrib(m_windowHandle, GLFW_DECORATED, GLFW_TRUE);
		glfwSetWindowAttrib(m_windowHandle, GLFW_RESIZABLE, GLFW_TRUE);

		if (m_appIsFullscreen) {
			// Resize to previous window properties
			glfwSetWindowPos(m_windowHandle, m_appWindowedPosX, m_appWindowedPosY);
			glfwSetWindowSize(m_windowHandle, m_appWindowedWidth, m_appWindowedHeight);

			m_appIsFullscreen = false;

		} else if (m_appIsFullscreenViaButton) {
			// Switch back to windowed mode specifically
			glfwSetWindowMonitor(m_windowHandle, NULL, m_appWindowedPosX, m_appWindowedPosY, m_appWindowedWidth, m_appWindowedHeight, GLFW_DONT_CARE);

			m_appIsFullscreenViaButton = false;
		}
	}

	// file drop call back
	void ApplicationSystem::FileDropCallback(GLFWwindow* window, int count, const char** paths)
	{
		(void)window;

		s_droppedFiles.clear();
		////std::cout << "[FileDrop] Received " << count << " file(s):\n";

		for (int i = 0; i < count; i++)
		{
			s_droppedFiles.push_back(std::string(paths[i]));
			////std::cout << "[FileDrop]   [" << i << "] " << paths[i] << "\n";
		}
	}

	// Toggle FPS display on or off
	void ApplicationSystem::ToggleFPSDisplay()
	{
		auto factory = m_coord.GetSystem<EntityFactory>();
		if (!factory) { return; }

		if (m_showFPS)
		{
			if (m_fpsEntity == INVALID_ENTITY)
			{
				m_fpsEntity = m_coord.CreateEntity();
				Transform t;
				t.UIbasePos = Vec3{ 150.f, 120.f, 0.f };
				t.layer = LayerType::GAMEUI;
				m_coord.AddComponent<Transform>(m_fpsEntity, t);

				Font font;
				font.fontSize = 0.7f;
				font.text = "FPS: 0";
				font.fontType = "buggy-font";
				font.textColor = Vec3{ 0.215f, 1.0f, 0.736f };
				m_coord.AddComponent<Font>(m_fpsEntity, font);
			}
		}
		else
		{
			if (m_fpsEntity != INVALID_ENTITY)
			{
				m_coord.DestroyEntity(m_fpsEntity);
				m_fpsEntity = INVALID_ENTITY;
			}
		}
	}

}	// NightSnack
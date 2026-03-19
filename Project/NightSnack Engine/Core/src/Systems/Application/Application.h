/******************************************************************************
 * File:        Application.h
 * Project:     NightSnack
 * Author(s):	Lim Wei Jie (Primary, 60%) - Created Application system to
 *				handle window creation, OpenGL context and Time Handling
 *				Goh Bei Lai (Secondary, 40%) - Implemented Interruption Handling logic. 
 *
 * Description: This file contains the declarations of the Application System,
 *				which manages the application window, OpenGL context, and timing.
 *
 * Copyright � 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Core/ECS/Coordinator.h"
#include "Systems/Application/Timer.h"
#include "Systems/Application/IEvent.h"

namespace NightSnack {

	class ApplicationSystem : public ISystem
	{
	public:
		/************************************************************************/
		/*!
		\brief
		Default constructor to create a window with given width, height and title.

		\param[in] width
		Width of the application window

		\param[in] height
		Height of the application window

		\param[in] title
		Title of the application window

		\param[in] coord
		Reference to the ECS Coordinator

		\param[in] dispatcher
		Reference to the EventDispatcher used for system events
		*/
		/************************************************************************/
		ApplicationSystem(std::string const& title, bool const& isFullscreen, Coordinator& coord, EventDispatcher& dispatcher);

		/************************************************************************/
		/*!
		\brief
		Destructor to clean up and terminate GLFW.
		*/
		/************************************************************************/
		~ApplicationSystem();

		/************************************************************************/
		/*!
		\brief
		Sets up subscribers to system events.

		\param[in] coord
		Reference to the ECS Coordinator
		*/
		/************************************************************************/
		void Init(Coordinator& Coord);

		/************************************************************************/
		/*!
		\brief
		Updates the application system each frame, by polling events and
		updating the window title with FPS and entity count.

		\param[in] dt
		Delta time since last frame

		\param[in] coord
		Reference to the ECS Coordinator
		*/
		/************************************************************************/
		void Update() override;

		/************************************************************************/
		/*!
		\brief
		Returns name of the system
		*/
		/************************************************************************/
		std::string GetName() const { return "ApplicationSystem"; }

		/************************************************************************/
		/*!
		\brief
		Polls for and processes events, such as keyboard and mouse input.
		*/
		/************************************************************************/
		void PollEvents();

		/************************************************************************/
		/*!
		\brief
		Swaps the front and back buffers of the window.
		*/
		/************************************************************************/
		void SwapBuffers();

		/************************************************************************/
		/*!
		\brief
		Checks if the window should close (e.g., if the user has requested to close it).
		*/
		/************************************************************************/
		bool ShouldClose() const;

		/************************************************************************/
		/*!
		\brief
		Updates the window title with the provided title string.

		\param[in] window
		GLFWwindow pointer to the application window

		\param[in] title
		Title string to set for the window
		*/
		/************************************************************************/
		void SetWindowTitle(GLFWwindow* window, const char* title);

		/************************************************************************/
		/*!
		\brief
		Trigger when the window is resized.

		\param[in] width
		New width of the application window

		\param[in] height
		New height of the application window
		*/
		/************************************************************************/
		void OnResize(int width, int height);

		/************************************************************************/
		/*!
		\brief
		Gets the current width of the application window.
		*/
		/************************************************************************/
		int GetWidth() const { return m_appWidth; }

		/************************************************************************/
		/*!
		\brief
		Gets the current height of the application window.
		*/
		/************************************************************************/
		int GetHeight() const { return m_appHeight; }

		/************************************************************************/
		/*!
		\brief
		Gets the native GLFW window handle.
		*/
		/************************************************************************/
		GLFWwindow* GetNativeHandle() const { return m_windowHandle; }

		/************************************************************************/
		/*!
		\brief
		Gets the Timer instance for managing frame timing and system timers.
		*/
		/************************************************************************/
		Timer& GetTimer() { return m_timer; }

		/************************************************************************/
		/*!
		\brief
		Timer interface.
		*/
		/************************************************************************/
		void StartFrame() { m_timer.StartFrame(); }
		void EndFrame() { m_timer.EndFrame(); }
		void StartSystemTimer(std::string&& s) { m_timer.StartSystemTimer(s); }
		void EndSystemTimer(std::string&& s) { m_timer.EndSystemTimer(s); }
		void EnableVSync(bool enabled = true) { m_timer.EnableVSync(enabled); }
		void SetFrameCap(float targetFPS) { frameCap = targetFPS; }
		void FrameCapUpdate() { if (frameCap > 0.f) m_timer.FrameCap(frameCap); }
		float GetFrameCap() { return frameCap; }
		float GetDT() { return m_timer.GetDT(); }

		/************************************************************************/
		/*!
		\brief
		Enter fullscreen without any borders or windows taskbar. 
		*/
		/************************************************************************/
		static void EnterFullscreen();

		/************************************************************************/
		/*!
		\brief
		Exit fullscreen and restore previous windowed mode position and size. 
		Way of restoring depends on how it was maximized previously. 
		*/
		/************************************************************************/
		void ExitFullscreen();

		void ToggleFPSDisplay();

		Signature GetSignature() const override { return m_systemSignature; }

		const std::vector<std::string>& GetDroppedFiles() const { return s_droppedFiles; }
		void ClearDroppedFiles() { s_droppedFiles.clear(); }

		static int m_appWidth, m_appHeight;
		// Window's 'windowed' properties to store
		static int m_appWindowedWidth, m_appWindowedHeight;
		static int m_appWindowedPosX, m_appWindowedPosY;
		static bool m_appToPause, m_appChangedPauseState;
		static bool m_appIsFullscreen; // User pressed F11 key to toggle fullscreen mode
		static bool m_appIsFullscreenViaButton; // User pressed the window's maximize button
		static bool m_pressedMaximizeButton;
		static bool m_showFPS;
		Entity m_fpsEntity = INVALID_ENTITY;
	private:
		/************************************************************************/
		/*!
		\brief
		Callback function triggered by GLFW when the framebuffer is resized.
		*/
		/************************************************************************/
		static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

		/************************************************************************/
		/*!
		\brief
		Callback function triggered by GLFW when the window toggles minimized state. 
		*/
		/************************************************************************/
		static void WindowIconifyCallback(GLFWwindow* window, int iconified);

		/************************************************************************/
		/*!
		\brief
		Callback function triggered by GLFW when the window toggles focus state. 
		*/
		/************************************************************************/
		static void WindowFocusCallback(GLFWwindow* window, int focused);

		/************************************************************************/
		/*!
		\brief
		Callback function triggered by GLFW when the window toggles maximized state.
		*/
		/************************************************************************/
		static void WindowMaximizeCallback(GLFWwindow* window, int maximized);

		// Window properties
		static GLFWwindow* m_windowHandle;
		std::string m_appTitle;
		bool m_previousPauseState = false;

		// Tick Tocker
		Timer m_timer;
		float frameCap = 0.f;
		bool systemTimersEnabled = false;

		Signature m_systemSignature{};

		// System event dispatcher
		EventDispatcher* m_dispatcher = nullptr;

		static std::vector<std::string> s_droppedFiles;

		static void FileDropCallback(GLFWwindow* window, int count, const char** paths);
		// Reference to ECS coordinator
		Coordinator& m_coord;
	};

}	// NightSnack

#endif // APPLICATION_HPP
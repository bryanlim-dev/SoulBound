/******************************************************************************
 * File:        Application.h
 * Project:     NightSnack
 * Author(s):
 *
 * Description:
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "ECS/Coordinator.h"
#include "Systems/Timer.h"
#include "Systems/IEvent.h"

class ApplicationSystem : public ISystem
{
public:
	ApplicationSystem(int width, int height, std::string const& title, Coordinator& coord, EventDispatcher& dispatcher);
	~ApplicationSystem();

	void Init(Coordinator& Coord);
	void Update(float dt, Coordinator& coord) override;
	std::string GetName() const { return "ApplicationSystem"; }

	void PollEvents();
	void SwapBuffers();
	bool ShouldClose() const;

	void SetWindowTitle(GLFWwindow* window, const char* title);
	int GetWidth() const { return m_appWidth; }
	int GetHeight() const { return m_appHeight; }

	GLFWwindow* GetNativeHandle() const { return m_windowHandle; }

	Timer& GetTimer() { return m_timer; }

	Signature GetSignature() const override { return m_systemSignature; }

private:
	static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

	GLFWwindow* m_windowHandle{ nullptr };
	int m_appWidth { 1600 };
	int m_appHeight { 900 };
	std::string m_appTitle;

	// Tick Tocker
	Timer m_timer;
	bool systemTimersEnabled = false;

	Signature m_systemSignature{};

	// System event dispatcher
	EventDispatcher* m_dispatcher = nullptr;
};

#endif // APPLICATION_HPP
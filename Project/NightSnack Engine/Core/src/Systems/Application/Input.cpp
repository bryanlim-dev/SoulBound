/******************************************************************************
 * File:        Input.cpp
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 95%) - Created Input System
 *				Lim Wei Jie (Secondary, 5%) - Initialized event dispatcher to
 *				dispatch Input Events on key/mouse input
 *
 * Description:
 * This file contains the definitions of the Input System.
 * The Input System gets its key and mouse inputs via GLFW callbacks.
 * It will then dispatch Input Events via the Event Dispatcher.
 * Other Systems in the ECS can check for the state of the specific keys via Getter functions.
 *
 * Copyright � 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "Input.h"
#include "Systems/Graphics.h"
#include <imgui.h>

namespace NightSnack {

	// Define static variables
	std::array<bool, static_cast<size_t>(InputSystem::NUM_TOTAL_KEYBOARD_KEY)> InputSystem::m_keysDown;
	std::array<bool, static_cast<size_t>(InputSystem::NUM_TOTAL_KEYBOARD_KEY)> InputSystem::m_keysPressed;
	std::array<bool, static_cast<size_t>(InputSystem::NUM_TOTAL_KEYBOARD_KEY)> InputSystem::m_keysReleased;

	std::array<bool, static_cast<size_t>(InputSystem::NUM_TOTAL_MOUSE_BUTTON)> InputSystem::m_mouseDown;
	std::array<bool, static_cast<size_t>(InputSystem::NUM_TOTAL_MOUSE_BUTTON)> InputSystem::m_mousePressed;
	std::array<bool, static_cast<size_t>(InputSystem::NUM_TOTAL_MOUSE_BUTTON)> InputSystem::m_mouseReleased;

	int InputSystem::m_currentModifiers = 0;
	float InputSystem::m_mousePosX = 0.f;
	float InputSystem::m_mousePosY = 0.f;

	EventDispatcher* InputSystem::m_dispatcher = nullptr;

	void InputSystem::Init(GLFWwindow* window, EventDispatcher& dispatcher) {
		glfwSetKeyCallback(window, KeyCallback); // key button
		glfwSetMouseButtonCallback(window, MouseButtonCallback); // mouse button
		glfwSetCursorPosCallback(window, CursorPosCallback); // mouse position

		m_prevMouseWorldPosX = m_prevMouseWorldPosY = m_deltaWorldPosX = m_deltaWorldPosY = 0.f;

		// Store the event dispatcher
		m_dispatcher = &dispatcher;
	}

	void InputSystem::Update() {
		// Find current mouse position (World)
		float Hvp = static_cast<float>(m_coord.GetWindowHeight());
		float Wvp = static_cast<float>(m_coord.GetWindowWidth());
		Entity cam = m_coord.GetSystem<GraphicsSystem>()->FindCamera();
		m_mouseWorldPosX = (-Wvp / 2.f) + m_mousePosX + m_coord.GetComponent<Transform>(cam).pos.x;
		m_mouseWorldPosY = (Hvp / 2.f) - m_mousePosY + m_coord.GetComponent<Transform>(cam).pos.y;
		// Compute mouse delta - the relative movement since the last frame
		m_deltaWorldPosX = m_mouseWorldPosX - m_prevMouseWorldPosX;
		m_deltaWorldPosY = m_mouseWorldPosY - m_prevMouseWorldPosY;
		m_prevMouseWorldPosX = m_mouseWorldPosX;
		m_prevMouseWorldPosY = m_mouseWorldPosY;

		// Process all queued events after input update and before the other systems
		m_dispatcher->ProcessEvents();
	}

	void InputSystem::ResetInputStates() {
		// Reset states back to false
		m_keysPressed.fill(false);
		m_keysReleased.fill(false);
		m_mousePressed.fill(false);
		m_mouseReleased.fill(false);
	}

	InputSystem::KeyboardKeys InputSystem::TranslateGLFWKeyboardKey(int glfwKey) {
		switch (glfwKey) {
		case GLFW_KEY_W: return KeyboardKeys::W;
		case GLFW_KEY_A: return KeyboardKeys::A;
		case GLFW_KEY_S: return KeyboardKeys::S;
		case GLFW_KEY_D: return KeyboardKeys::D;
		case GLFW_KEY_H: return KeyboardKeys::H;
		case GLFW_KEY_SPACE: return KeyboardKeys::SPACE;
		case GLFW_KEY_RIGHT: return KeyboardKeys::RIGHT_ARROW;
		case GLFW_KEY_LEFT: return KeyboardKeys::LEFT_ARROW;
		case GLFW_KEY_UP: return KeyboardKeys::UP_ARROW;
		default: return KeyboardKeys::NUM_TOTAL_KEYBOARD_KEY;
		}
	}

	InputSystem::MouseButtons InputSystem::TranslateGLFWMouseButton(int glfwButton) {
		switch (glfwButton) {
		case GLFW_MOUSE_BUTTON_LEFT: return MouseButtons::LMB;
		case GLFW_MOUSE_BUTTON_RIGHT: return MouseButtons::RMB;
		default: return MouseButtons::NUM_TOTAL_MOUSE_BUTTON;
		}
	}

	InputEvent::InputAction InputSystem::TranslateGLFWKeyToAction(int glfwKey, int modifier) {
		switch (glfwKey) {
			// Map every single key/mouse press to their relevant input events
			// 
			// Player movement
		case GLFW_KEY_A: return InputEvent::InputAction::PlayerMoveLeft;
		case GLFW_KEY_D: return InputEvent::InputAction::PlayerMoveRight;
		case GLFW_KEY_W: return InputEvent::InputAction::PlayerJump;

			// Window tests
		case GLFW_KEY_F1: return InputEvent::InputAction::DisplayFPS;
		case GLFW_KEY_F4: return InputEvent::InputAction::ToggleFullscreen;
		case GLFW_KEY_ESCAPE: return InputEvent::InputAction::PauseGame;
		case GLFW_KEY_F5: return InputEvent::InputAction::RestartGame;

			// Debug tests
		//case GLFW_KEY_TAB: return InputEvent::InputAction::ToggleSystemTimers;
		case GLFW_KEY_O: return InputEvent::InputAction::ToggleDebugDraw;
		case GLFW_KEY_BACKSLASH: return InputEvent::InputAction::ToggleUIEditor;
		case GLFW_KEY_X: return InputEvent::InputAction::ChangeAnimation;
			// Mouse events
		case GLFW_MOUSE_BUTTON_LEFT: return InputEvent::InputAction::ClickLMB;

			// Modifier events
		case GLFW_KEY_S: // (CTRL+S)
			return ((modifier & GLFW_MOD_CONTROL) > 0) ? InputEvent::InputAction::Save : InputEvent::InputAction::NumTotalInputAction;
		case GLFW_KEY_P: // (CTRL+P)
			return ((modifier & GLFW_MOD_CONTROL) > 0) ? InputEvent::InputAction::PhysicsDebugMode : InputEvent::InputAction::PhysicsDebugStep;
		case GLFW_KEY_Z: // (CTRL+Z)
			return ((modifier & GLFW_MOD_CONTROL) > 0) ? InputEvent::InputAction::UndoStep : InputEvent::InputAction::NumTotalInputAction;

		default: return InputEvent::InputAction::NumTotalInputAction;
		}
	}

	InputEvent::InputActionState InputSystem::TranslateGLFWKeyStateToAction(int glfwKeyState) {
		switch (glfwKeyState) {
		case GLFW_PRESS: return InputEvent::InputActionState::Pressed;
		case GLFW_RELEASE: return InputEvent::InputActionState::Released;
		case GLFW_REPEAT: return InputEvent::InputActionState::Repeat;

		default: return InputEvent::InputActionState::NumTotalInputActionState;
		}
	}

	// Callback functions --------------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------

	void InputSystem::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		(void)scancode; // not needed, unless implementing with custom keyboard layouts
		(void)window;

		InputEvent::InputAction mappedAction = TranslateGLFWKeyToAction(key, mods);
		// The key that triggered this callback function is not a key that we look out for
		if (mappedAction == InputEvent::InputAction::NumTotalInputAction)
			return;

		InputEvent::InputActionState mappedActionState = TranslateGLFWKeyStateToAction(action);

		// Dispatch InputEvent when key is pressed, and if user is not typing in an ImGui text field
		if (m_dispatcher)
		{
			// Check whether the ImGui context exists first
			if (ImGui::GetCurrentContext()) {
				ImGuiIO& io = ImGui::GetIO();
				// Do not queue any events if the user is in a ImGui text field
				if (io.WantCaptureKeyboard) {
					return;
				}
			}
			m_dispatcher->QueueEvent(std::make_unique<InputEvent>(mappedAction, mappedActionState));
		}

		// Update the modifier state
		m_currentModifiers = mods;

		// Evalute the respective key states
		KeyboardKeys mappedKey = TranslateGLFWKeyboardKey(key);
		if (mappedKey == NUM_TOTAL_KEYBOARD_KEY)
			return;

		if (action == GLFW_PRESS) {
			m_keysDown[mappedKey] = true;
			m_keysPressed[mappedKey] = true;
		} else if (action == GLFW_RELEASE) {
			m_keysDown[mappedKey] = false;
			m_keysReleased[mappedKey] = true;
		}
	}

	void InputSystem::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
		(void)window;

		InputEvent::InputAction mappedAction = TranslateGLFWKeyToAction(button, mods);
		// The button that triggered this callback function is not a button that we look out for
		if (mappedAction == InputEvent::InputAction::NumTotalInputAction)
			return;

		InputEvent::InputActionState mappedActionState = TranslateGLFWKeyStateToAction(action);

		// Dispatch InputEvent when key is pressed
		if (m_dispatcher)
		{
			m_dispatcher->QueueEvent(std::make_unique<InputEvent>(mappedAction, mappedActionState));
		}

		// Update the modifier state
		m_currentModifiers = mods;

		// Evalute the respective mouse states
		KeyboardKeys mappedButton = TranslateGLFWKeyboardKey(button);
		if (mappedButton == NUM_TOTAL_KEYBOARD_KEY)
			return;

		if (action == GLFW_PRESS) {
			m_mouseDown[mappedButton] = true;
			m_mousePressed[mappedButton] = true;
		} else if (action == GLFW_RELEASE) {
			m_mouseDown[mappedButton] = false;
			m_mouseReleased[mappedButton] = true;
		}
	}

	void InputSystem::CursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
		(void)window;

		// Update the current mouse position
		m_mousePosX = static_cast<float>(xPos);
		m_mousePosY = static_cast<float>(yPos);
	}

	// Queries for key states -------------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------

	bool InputSystem::IsKeyDown(int key) const { return m_keysDown[key]; }
	bool InputSystem::IsKeyPressed(int key) const { return m_keysPressed[key]; }
	bool InputSystem::IsKeyReleased(int key) const { return m_keysReleased[key]; }

	// Queries for modifier key states ----------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------

	bool InputSystem::IsShiftDown() const { return m_currentModifiers & GLFW_MOD_SHIFT; }
	bool InputSystem::IsCtrlDown() const { return m_currentModifiers & GLFW_MOD_CONTROL; }
	bool InputSystem::IsAltDown() const { return m_currentModifiers & GLFW_MOD_ALT; }

	// Queries for mouse states -----------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------

	bool InputSystem::IsMouseButtonDown(int button) const { return m_mouseDown[button]; }
	bool InputSystem::IsMouseButtonPressed(int button) const { return m_mousePressed[button]; }
	bool InputSystem::IsMouseButtonReleased(int button) const { return m_mouseReleased[button]; }

	// Queries for mouse position -----------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------

	float InputSystem::GetMousePosX() const { return m_mousePosX; }
	float InputSystem::GetMousePosY() const { return m_mousePosY; }
	float InputSystem::GetMouseWorldPosX() const { return m_mouseWorldPosX; }
	float InputSystem::GetMouseWorldPosY() const { return m_mouseWorldPosY; }
	float InputSystem::GetMouseDeltaPosX() const { return m_deltaWorldPosX; }
	float InputSystem::GetMouseDeltaPosY() const { return m_deltaWorldPosY; }

}	// NightSnack

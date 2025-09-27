#include "Input.h"
#include <iostream> // just for inputevent testing (removing later)

// Define static variables
//std::array<bool, static_cast<size_t>(InputSystem::NUM_TOTAL_KEYBOARD_KEY)> InputSystem::m_keysDown;
//std::array<bool, static_cast<size_t>(InputSystem::NUM_TOTAL_KEYBOARD_KEY)> InputSystem::m_keysPressed;
//std::array<bool, static_cast<size_t>(InputSystem::NUM_TOTAL_KEYBOARD_KEY)> InputSystem::m_keysReleased;
//
//std::array<bool, static_cast<size_t>(InputSystem::NUM_TOTAL_MOUSE_BUTTON)> InputSystem::m_mouseDown;
//std::array<bool, static_cast<size_t>(InputSystem::NUM_TOTAL_MOUSE_BUTTON)> InputSystem::m_mousePressed;
//std::array<bool, static_cast<size_t>(InputSystem::NUM_TOTAL_MOUSE_BUTTON)> InputSystem::m_mouseReleased;

int InputSystem::m_currentModifiers = 0;
float InputSystem::m_mousePosX = 0.f;
float InputSystem::m_mousePosY = 0.f;

EventDispatcher* InputSystem::m_dispatcher = nullptr;

// Set up the respective callbacks with GLFW ------------------------------------------------------
// ------------------------------------------------------------------------------------------------
void InputSystem::Init(GLFWwindow* window, EventDispatcher& dispatcher) {
	glfwSetKeyCallback(window, KeyCallback); // key button
	glfwSetMouseButtonCallback(window, MouseButtonCallback); // mouse button
	glfwSetCursorPosCallback(window, CursorPosCallback); // mouse position

	m_prevMousePosX = m_prevMousePosY = m_deltaPosX = m_deltaPosY = 0.f;

	// Store the event dispatcher
	m_dispatcher = &dispatcher;
}

// Update the mouse positions every update loop / frame -------------------------------------------
// ------------------------------------------------------------------------------------------------
void InputSystem::Update(float dt, Coordinator& coord) {
	(void)dt;
	(void)coord;

	// Compute mouse delta - the relative movement since the last frame
	m_deltaPosX = m_mousePosX - m_prevMousePosX;
	m_deltaPosY = m_mousePosY - m_prevMousePosY;
	m_prevMousePosX = m_mousePosX;
	m_prevMousePosY = m_mousePosY;

	ResetInputStates();
}

// Reset the key and mouse button states at the end of every update loop / frame ------------------
// ------------------------------------------------------------------------------------------------
void InputSystem::ResetInputStates() {
	// Reset states back to false
	//m_keysPressed.fill(false);
	//m_keysReleased.fill(false);
	//m_mousePressed.fill(false);
	//m_mouseReleased.fill(false);
}

InputEvent::InputAction InputSystem::TranslateGLFWKeyToAction(int glfwKey) {
	switch (glfwKey) {
	case GLFW_KEY_W: return InputEvent::InputAction::SelectNext;
	case GLFW_KEY_A: return InputEvent::InputAction::ScaleDown;
	case GLFW_KEY_D: return InputEvent::InputAction::ScaleUp;
	case GLFW_KEY_R: return InputEvent::InputAction::Rotate;
	case GLFW_KEY_SPACE: return InputEvent::InputAction::ApplyForce;
	case GLFW_MOUSE_BUTTON_LEFT: return InputEvent::InputAction::ClickLMB;
	default: return InputEvent::InputAction::NumTotalInputAction;
	}
}

// Translates GLFW mouse input into InputSystem's enum MouseButtons ------------------------------
// ------------------------------------------------------------------------------------------------
//InputSystem::MouseButtons InputSystem::TranslateGLFWMouseButton(int glfwButton) {
//	switch (glfwButton) {
//	case GLFW_MOUSE_BUTTON_LEFT: return MouseButtons::LMB;
//	case GLFW_MOUSE_BUTTON_RIGHT: return MouseButtons::RMB;
//	default: return MouseButtons::NUM_TOTAL_MOUSE_BUTTON;
//	}
//}

// Key callback function --------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
void InputSystem::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	(void)scancode; // not needed, unless implementing with custom keyboard layouts
	(void)window;

	// This is a key that we are not tracking
	//KeyboardKeys mappedKey = TranslateGLFWKeyboardKey(key);
	//if (mappedKey == NUM_TOTAL_KEYBOARD_KEY)
	//	return;
	InputEvent::InputAction mappedAction = TranslateGLFWKeyToAction(key);
	if (mappedAction == InputEvent::InputAction::NumTotalInputAction)
		return;

	// Evaluate the respective key states
	/*if (action == GLFW_PRESS) {
		m_keysDown[mappedKey] = true;
		m_keysPressed[mappedKey] = true;
	} else if (action == GLFW_RELEASE) {
		m_keysDown[mappedKey] = false;
		m_keysReleased[mappedKey] = true;
	}*/

	// Update the modifier state
	m_currentModifiers = mods;

	// Dispatch InputEvent when key is pressed
	if (m_dispatcher)
	{
		m_dispatcher->Dispatch(InputEvent(mappedAction, action == GLFW_PRESS));
	}
}

// Mouse callback function ------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
void InputSystem::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	(void)window;

	InputEvent::InputAction mappedAction = TranslateGLFWKeyToAction(button);
	if (mappedAction == InputEvent::InputAction::NumTotalInputAction)
		return;

	// This is a key that we are not tracking
	//MouseButtons mappedButton = TranslateGLFWMouseButton(button);
	//if (mappedButton == NUM_TOTAL_MOUSE_BUTTON)
	//	return;

	//// Evaluate the respective mouse states
	//if (action == GLFW_PRESS) {
	//	m_mouseDown[mappedButton] = true;
	//	m_mousePressed[mappedButton] = true;
	//} else if (action == GLFW_RELEASE) {
	//	m_mouseDown[mappedButton] = false;
	//	m_mouseReleased[mappedButton] = true;
	//}

	// Update the modifier state
	m_currentModifiers = mods;

	// Dispatch InputEvent when key is pressed
	if (m_dispatcher)
	{
		m_dispatcher->Dispatch(InputEvent(mappedAction, action == GLFW_PRESS));
	}
}

// Mouse position callback function ---------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
void InputSystem::CursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
	(void)window;

	// Update the current mouse position
	m_mousePosX = static_cast<float>(xPos);
	m_mousePosY = static_cast<float>(yPos);
}

// Queries for key states -------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
//bool InputSystem::IsKeyDown(int key) const { return m_keysDown[key]; }
//bool InputSystem::IsKeyPressed(int key) const { return m_keysPressed[key]; }
//bool InputSystem::IsKeyReleased(int key) const { return m_keysReleased[key]; }

// Queries for modifier key states ----------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
bool InputSystem::IsShiftDown() const { return m_currentModifiers & GLFW_MOD_SHIFT; }
bool InputSystem::IsCtrlDown() const { return m_currentModifiers & GLFW_MOD_CONTROL; }
bool InputSystem::IsAltDown() const { return m_currentModifiers & GLFW_MOD_ALT; }

// Queries for mouse states -----------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
//bool InputSystem::IsMouseButtonDown(int button) const { return m_mouseDown[button]; }
//bool InputSystem::IsMouseButtonPressed(int button) const { return m_mousePressed[button]; }
//bool InputSystem::IsMouseButtonReleased(int button) const { return m_mouseReleased[button]; }

// Queries for mouse position ---------------------------------------------------------------------
// Note: Mouse position origin is from top left corner of the screen ------------------------------
float InputSystem::GetMousePosX() const { return m_mousePosX; }
float InputSystem::GetMousePosY() const { return m_mousePosY; }
float InputSystem::GetMouseDeltaPosX() const { return m_deltaPosX; }
float InputSystem::GetMouseDeltaPosY() const { return m_deltaPosY; }

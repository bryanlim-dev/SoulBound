#include "InputManager.h"

// Define static variables
std::array<bool, GLFW_KEY_LAST + 1> InputManager::m_keysDown;
std::array<bool, GLFW_KEY_LAST + 1> InputManager::m_keysPressed;
std::array<bool, GLFW_KEY_LAST + 1> InputManager::m_keysReleased;

std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> InputManager::m_mouseDown;
std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> InputManager::m_mousePressed;
std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> InputManager::m_mouseReleased;

int InputManager::m_currentModifiers = 0;
double InputManager::m_mousePosX = 0.0;
double InputManager::m_mousePosY = 0.0;
double InputManager::m_prevMousePosX = 0.0;
double InputManager::m_prevMousePosY = 0.0;
double InputManager::m_deltaPosX = 0.0;
double InputManager::m_deltaPosY = 0.0;

// Set up the respective callbacks with GLFW ------------------------------------------------------
// ------------------------------------------------------------------------------------------------
void InputManager::Init(GLFWwindow* window) {
	glfwSetKeyCallback(window, KeyCallback); // key button
	glfwSetMouseButtonCallback(window, MouseButtonCallback); // mouse button
	glfwSetCursorPosCallback(window, CursorPosCallback); // mouse position
}

// Update the mouse positions every update loop / frame -------------------------------------------
// ------------------------------------------------------------------------------------------------
void InputManager::Update() {
	// Compute mouse delta - the relative movement since the last frame
	m_deltaPosX = m_mousePosX - m_prevMousePosX;
	m_deltaPosY = m_mousePosY - m_prevMousePosY;
	m_prevMousePosX = m_mousePosX;
	m_prevMousePosY = m_mousePosY;
}

// Reset the key and mouse button states at the end of every update loop / frame ------------------
// ------------------------------------------------------------------------------------------------
void InputManager::LateUpdate() {
	// Reset states back to false
	m_keysPressed.fill(false);
	m_keysReleased.fill(false);
	m_mousePressed.fill(false);
	m_mouseReleased.fill(false);
}

// Key callback function --------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
void InputManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	(void)scancode; // not needed, unless implementing with custom keyboard layouts
	(void)window;

	if (key < 0 || key > GLFW_KEY_LAST)
		return;
	
	// Evaluate the respective key states
	if (action == GLFW_PRESS) {
		m_keysDown[key] = true;
		m_keysPressed[key] = true;
	} else if (action == GLFW_RELEASE) {
		m_keysDown[key] = false;
		m_keysReleased[key] = true;
	}

	// Update the modifier state
	m_currentModifiers = mods;
}

// Mouse callback function ------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
void InputManager::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	(void)window;
	
	if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST)
		return;
	
	// Evaluate the respective mouse states
	if (action == GLFW_PRESS) {
		m_mouseDown[button] = true;
		m_mousePressed[button] = true;
	} else if (action == GLFW_RELEASE) {
		m_mouseDown[button] = false;
		m_mouseReleased[button] = true;
	}

	// Update the modifier state
	m_currentModifiers = mods;
}

// Mouse position callback function ---------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
void InputManager::CursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
	(void)window;

	// Update the current mouse position
	m_mousePosX = xPos;
	m_mousePosY = yPos;
}

// Queries for key states -------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
bool InputManager::IsKeyDown(int key) { return m_keysDown[key]; }
bool InputManager::IsKeyPressed(int key) { return m_keysPressed[key]; }
bool InputManager::IsKeyReleased(int key) { return m_keysReleased[key]; }

// Queries for modifier key states ----------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
bool InputManager::IsShiftDown() { return m_currentModifiers & GLFW_MOD_SHIFT; }
bool InputManager::IsCtrlDown() { return m_currentModifiers & GLFW_MOD_CONTROL; }
bool InputManager::IsAltDown() { return m_currentModifiers & GLFW_MOD_ALT; }

// Queries for mouse states -----------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
bool InputManager::IsMouseButtonDown(int button) { return m_mouseDown[button]; }
bool InputManager::IsMouseButtonPressed(int button) { return m_mousePressed[button]; }
bool InputManager::IsMouseButtonReleased(int button) { return m_mouseReleased[button]; }

// Queries for mouse position ---------------------------------------------------------------------
// Note: Mouse position origin is from top left corner of the screen ------------------------------
double InputManager::GetMousePosX() { return m_mousePosX; }
double InputManager::GetMousePosY() { return m_mousePosY; }
double InputManager::GetMouseDeltaPosX() { return m_deltaPosX; }
double InputManager::GetMouseDeltaPosY() { return m_deltaPosY; }

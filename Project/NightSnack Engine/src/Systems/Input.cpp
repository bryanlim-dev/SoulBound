#include "Input.h"
#include "Graphics.h"

// Define static variables
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

	// Find current mouse position (World)
	float Hvp = static_cast<float>(coord.GetWindowHeight());
	float Wvp = static_cast<float>(coord.GetWindowWidth());
	Entity cam = coord.GetSystem<GraphicsSystem>()->FindCamera();
	m_mouseWorldPosX = (-Wvp / 2.f) + m_mousePosX + coord.GetComponent<Position>(cam).x;
	m_mouseWorldPosY = (Hvp / 2.f) - m_mousePosY + coord.GetComponent<Position>(cam).y;

	// Compute mouse delta - the relative movement since the last frame
	m_deltaPosX = m_mousePosX - m_prevMousePosX;
	m_deltaPosY = m_mousePosY - m_prevMousePosY;
	m_prevMousePosX = m_mousePosX;
	m_prevMousePosY = m_mousePosY;
}

// Translate the GLFW key value to its respective InputActions ------------------------------------
// ------------------------------------------------------------------------------------------------
InputEvent::InputAction InputSystem::TranslateGLFWKeyToAction(int glfwKey, int modifier) {
	switch (glfwKey) {
	// Single key presses
	case GLFW_KEY_W: return InputEvent::InputAction::SelectNext;
	case GLFW_KEY_A: return InputEvent::InputAction::ScaleDown;
	case GLFW_KEY_D: return InputEvent::InputAction::ScaleUp;
	case GLFW_KEY_R: return InputEvent::InputAction::Rotate;
	case GLFW_MOUSE_BUTTON_LEFT: return InputEvent::InputAction::ClickLMB;

	case GLFW_KEY_F: return InputEvent::InputAction::DynamicBallTest;
	case GLFW_KEY_G: return InputEvent::InputAction::GravityBallTest;

	// Entity keys;
	case GLFW_KEY_1: return InputEvent::InputAction::CreateSquare;
	case GLFW_KEY_2: return InputEvent::InputAction::CreateCircle;
	case GLFW_KEY_3: return InputEvent::InputAction::CreateBananaCircle;
	case GLFW_KEY_SPACE: return InputEvent::InputAction::DoubleEntities;
	case GLFW_KEY_BACKSPACE: return InputEvent::InputAction::HalfEntities;

	case GLFW_KEY_T: return InputEvent::InputAction::ToggleDebugDraw;

	// Modifier keys (CTRL+S etc)
	case GLFW_KEY_S: 
		return ((modifier & GLFW_MOD_CONTROL) > 0) ? InputEvent::InputAction::Save : InputEvent::InputAction::NumTotalInputAction;

	default: return InputEvent::InputAction::NumTotalInputAction;
	}
}

// Key callback function --------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
void InputSystem::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	(void)scancode; // not needed, unless implementing with custom keyboard layouts
	(void)window;

	InputEvent::InputAction mappedAction = TranslateGLFWKeyToAction(key, mods);
	if (mappedAction == InputEvent::InputAction::NumTotalInputAction)
		return;

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

	InputEvent::InputAction mappedAction = TranslateGLFWKeyToAction(button, mods);
	if (mappedAction == InputEvent::InputAction::NumTotalInputAction)
		return;

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

// Queries for mouse position ---------------------------------------------------------------------
// Note: Mouse position origin is from top left corner of the screen ------------------------------
float InputSystem::GetMousePosX() const { return m_mouseWorldPosX; }
float InputSystem::GetMousePosY() const { return m_mouseWorldPosY; }
float InputSystem::GetMouseDeltaPosX() const { return m_deltaPosX; }
float InputSystem::GetMouseDeltaPosY() const { return m_deltaPosY; }

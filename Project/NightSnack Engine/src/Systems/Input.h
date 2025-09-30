#ifndef INPUT_HPP
#define INPUT_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <array>
#include "ECS/ISystem.h"
#include "ECS/Coordinator.h"
#include "Systems/IEvent.h"

class InputSystem : public ISystem {

public:

	enum KeyboardKeys {
		W = 0,
		A,
		S,
		D,
		H,
		SPACE,
		ESC,
		NUM_TOTAL_KEYBOARD_KEY
	};

	enum MouseButtons {
		LMB = 0,
		RMB,
		NUM_TOTAL_MOUSE_BUTTON
	};

	void Init(GLFWwindow* window, EventDispatcher& dispatcher);

	void Update(float dt, Coordinator& coord) override;
	std::string GetName() const { return "InputSystem"; }
	void ResetInputStates();

	// Keyboard queries
	bool IsKeyDown(int key) const;
	bool IsKeyPressed(int key) const;
	bool IsKeyReleased(int key) const;

	// Keyboard modifier queries
	bool IsShiftDown() const;
	bool IsCtrlDown() const;
	bool IsAltDown() const;

	// Mouse queries
	bool IsMouseButtonDown(int button) const;
	bool IsMouseButtonPressed(int button) const;
	bool IsMouseButtonReleased(int button) const;

	// Accessor functions for mouse position
	float GetMousePosX() const;
	float GetMousePosY() const;
	float GetMouseDeltaPosX() const;
	float GetMouseDeltaPosY() const;

	Signature GetSignature() const override { return m_systemSignature; }

private:

	static InputEvent::InputAction TranslateGLFWKeyToAction(int glfwKey, int modifier);
	static KeyboardKeys TranslateGLFWKeyboardKey(int glfwKey);
	static MouseButtons TranslateGLFWMouseButton(int glfwButton);

	// Callbacks
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void CursorPosCallback(GLFWwindow* window, double xPos, double yPos);

	// State tracking
	static std::array<bool, static_cast<size_t>(NUM_TOTAL_KEYBOARD_KEY)> m_keysDown;		// Represents the keys that are being held down
	static std::array<bool, static_cast<size_t>(NUM_TOTAL_KEYBOARD_KEY)> m_keysPressed;		// Represents the keys that are pressed once
	static std::array<bool, static_cast<size_t>(NUM_TOTAL_KEYBOARD_KEY)> m_keysReleased;	// Represents the keys that are released

	static std::array<bool, static_cast<size_t>(NUM_TOTAL_MOUSE_BUTTON)> m_mouseDown;		// Represents the mouse that is being held down
	static std::array<bool, static_cast<size_t>(NUM_TOTAL_MOUSE_BUTTON)> m_mousePressed;	// Represents the mouse that is pressed once
	static std::array<bool, static_cast<size_t>(NUM_TOTAL_MOUSE_BUTTON)> m_mouseReleased;	// Represents the mouse that is released

	static int m_currentModifiers;							// Represents the bitfield of modifier keys

	static float m_mousePosX, m_mousePosY;					// Represents the current position in the current frame
	float m_mouseWorldPosX{}, m_mouseWorldPosY{};

	float m_prevMousePosX{ 0.f }, m_prevMousePosY{ 0.f };	// Represents the previous position in the previous frame
	float m_deltaPosX{ 0.f }, m_deltaPosY{ 0.f };			// Represents the relative movement since the last frame

	Signature m_systemSignature{};

	// System event dispatcher
	static EventDispatcher* m_dispatcher;
};


#endif // INPUT_HPP
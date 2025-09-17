#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

#include <GLFW/glfw3.h>
#include <array>

class InputManager {

public:
	
	static void Init(GLFWwindow* window);

	static void Update();
	static void LateUpdate();

	// Keyboard queries
	static bool IsKeyDown(int key); 
	static bool IsKeyPressed(int key);
	static bool IsKeyReleased(int key);

	// Keyboard modifier queries
	static bool IsShiftDown();
	static bool IsCtrlDown();
	static bool IsAltDown();

	// Mouse queries
	static bool IsMouseButtonDown(int button);
	static bool IsMouseButtonPressed(int button);
	static bool IsMouseButtonReleased(int button);

	static double GetMousePosX();
	static double GetMousePosY();
	static double GetMouseDeltaPosX();
	static double GetMouseDeltaPosY();

private:

	// Callbacks
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void CursorPosCallback(GLFWwindow* window, double xPos, double yPos);

	// State tracking
	static std::array<bool, GLFW_KEY_LAST + 1> m_keysDown;		// Represents the keys that are being held down
	static std::array<bool, GLFW_KEY_LAST + 1> m_keysPressed;		// Represents the keys that are pressed once
	static std::array<bool, GLFW_KEY_LAST + 1> m_keysReleased;	// Represents the keys that are released

	static std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> m_mouseDown;		// Represents the mouse that is being held down
	static std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> m_mousePressed;	// Represents the mouse that is pressed once
	static std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> m_mouseReleased;  // Represents the mouse that is released

	static int m_currentModifiers;					// Represents the bitfield of modifier keys
	static double m_mousePosX, m_mousePosY;			// Represents the current position in the current frame
	static double m_prevMousePosX, m_prevMousePosY;	// Represents the previous position in the previous frame
	static double m_deltaPosX, m_deltaPosY;			// Represents the relative movement since the last frame

};


#endif // INPUT_MANAGER_HPP
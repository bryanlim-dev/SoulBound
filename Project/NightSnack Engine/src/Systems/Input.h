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

	void Init(GLFWwindow* window, EventDispatcher& dispatcher);

	void Update(float dt, Coordinator& coord) override;
	std::string GetName() const { return "InputSystem"; }

	// Accessor functions for mouse position
	float GetMousePosX() const;
	float GetMousePosY() const;
	float GetMouseDeltaPosX() const;
	float GetMouseDeltaPosY() const;

	Signature GetSignature() const override { return m_systemSignature; }

private:

	static InputEvent::InputAction TranslateGLFWKeyToAction(int glfwKey, int modifier);
	static InputEvent::InputActionState TranslateGLFWKeyStateToAction(int glfwkeyState);

	// Callbacks
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void CursorPosCallback(GLFWwindow* window, double xPos, double yPos);

	static float m_mousePosX, m_mousePosY;					// Represents the current position in the current frame
	float m_mouseWorldPosX{}, m_mouseWorldPosY{};

	float m_prevMousePosX{ 0.f }, m_prevMousePosY{ 0.f };	// Represents the previous position in the previous frame
	float m_deltaPosX{ 0.f }, m_deltaPosY{ 0.f };			// Represents the relative movement since the last frame

	Signature m_systemSignature{};

	// System event dispatcher
	static EventDispatcher* m_dispatcher;
};


#endif // INPUT_HPP
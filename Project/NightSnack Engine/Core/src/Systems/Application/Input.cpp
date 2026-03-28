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

	// Controller statics
	std::array<bool, InputSystem::NUM_TOTAL_CONTROLLER_BUTTON> InputSystem::m_controllerDown    [InputSystem::MAX_CONTROLLERS]{};
	std::array<bool, InputSystem::NUM_TOTAL_CONTROLLER_BUTTON> InputSystem::m_controllerPressed [InputSystem::MAX_CONTROLLERS]{};
	std::array<bool, InputSystem::NUM_TOTAL_CONTROLLER_BUTTON> InputSystem::m_controllerReleased[InputSystem::MAX_CONTROLLERS]{};
	std::array<bool, InputSystem::NUM_TOTAL_CONTROLLER_AXIS>   InputSystem::m_controllerAxisActive  [InputSystem::MAX_CONTROLLERS]{};
	std::array<bool, InputSystem::NUM_TOTAL_CONTROLLER_AXIS>   InputSystem::m_controllerAxisPressed [InputSystem::MAX_CONTROLLERS]{};
	std::array<bool, InputSystem::NUM_TOTAL_CONTROLLER_AXIS>   InputSystem::m_controllerAxisReleased[InputSystem::MAX_CONTROLLERS]{};
	std::array<float, GLFW_GAMEPAD_AXIS_LAST + 1>              InputSystem::m_controllerRawAxes     [InputSystem::MAX_CONTROLLERS]{};
	bool InputSystem::m_controllerConnected[InputSystem::MAX_CONTROLLERS]{};

	void InputSystem::Init(GLFWwindow* window, EventDispatcher& dispatcher) {
		glfwSetKeyCallback(window, KeyCallback); // key button
		glfwSetMouseButtonCallback(window, MouseButtonCallback); // mouse button
		glfwSetCursorPosCallback(window, CursorPosCallback); // mouse position
		glfwSetJoystickCallback(JoystickCallback); // controller connect/disconnect

		// Detect any controllers already connected at startup
		// Use glfwJoystickPresent - glfwJoystickIsGamepad returns false for Switch Pro
		for (int i = 0; i <= GLFW_JOYSTICK_LAST; ++i)
		{
			m_controllerConnected[i] = glfwJoystickPresent(i) == GLFW_TRUE;
		}

		m_prevMouseWorldPosX = m_prevMouseWorldPosY = m_deltaWorldPosX = m_deltaWorldPosY = 0.f;

		// Store the event dispatcher
		m_dispatcher = &dispatcher;
	}

	void InputSystem::Update() {
		ResetInputStates();

		// Find current mouse position (World)
		float Hvp = static_cast<float>(m_coord.GetWindowHeight());
		float Wvp = static_cast<float>(m_coord.GetWindowWidth());
		Entity cam = m_coord.GetSystem<GraphicsSystem>()->FindCamera();

		auto& camTransform = m_coord.GetComponent<Transform>(cam);
		auto& camera = m_coord.GetComponent<Camera>(cam);
		float zoom = camera.zoom;
		float halfCamWidth = (camera.width * 0.5f) / zoom;
		float halfCamHeight = (camera.height * 0.5f) / zoom;

		// Convert screen-space mouse [0, Wvp] x [0, Hvp] to normalized [-1, 1] x [-1, 1]
		float normalizedX = (m_mousePosX / Wvp) * 2.0f - 1.0f;
		float normalizedY = 1.0f - (m_mousePosY / Hvp) * 2.0f;  // Flip Y axis

		// Scale to camera world space and add camera position
		m_mouseWorldPosX = (normalizedX * halfCamWidth) + camTransform.pos.x;
		m_mouseWorldPosY = (normalizedY * halfCamHeight) + camTransform.pos.y;

		// Compute mouse delta - the relative movement since the last frame
		m_deltaWorldPosX = m_mouseWorldPosX - m_prevMouseWorldPosX;
		m_deltaWorldPosY = m_mouseWorldPosY - m_prevMouseWorldPosY;
		m_prevMouseWorldPosX = m_mouseWorldPosX;
		m_prevMouseWorldPosY = m_mouseWorldPosY;

		// Poll all connected controllers
		for (int jid = 0; jid <= GLFW_JOYSTICK_LAST; ++jid)
		{
			if (!m_controllerConnected[jid]) continue;

			GLFWgamepadstate state{};
			if (!glfwGetGamepadState(jid, &state))
			{
				// Controller is connected but not in GLFW's gamepad DB
				// Fall back to raw joystick axes/buttons and remap manually
				int axisCount = 0, buttonCount = 0;
				const float* rawAxes    = glfwGetJoystickAxes   (jid, &axisCount);
				const unsigned char* rawButtons = glfwGetJoystickButtons(jid, &buttonCount);
				if (!rawAxes || !rawButtons) continue;

				if (axisCount >= 4)
				{
					state.axes[GLFW_GAMEPAD_AXIS_LEFT_X]  = rawAxes[0];
					state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]  = rawAxes[1];
					state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] = rawAxes[2];
					state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] = rawAxes[3];
				}
				// Map raw buttons to GLFW gamepad button slots
				const int rawToGLFW[][2] = {
					{ 0,  GLFW_GAMEPAD_BUTTON_CROSS    },
					{ 1,  GLFW_GAMEPAD_BUTTON_CIRCLE   },  // A (east)   -> Confirm
					{ 2,  GLFW_GAMEPAD_BUTTON_TRIANGLE },  // X (north)  -> Pause
					{ 3,  GLFW_GAMEPAD_BUTTON_SQUARE   },  // Y (west)   -> Possess
					{ 4,  GLFW_GAMEPAD_BUTTON_LEFT_BUMPER  },
					{ 5,  GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER },
					{ 8,  GLFW_GAMEPAD_BUTTON_BACK  },
					{ 9,  GLFW_GAMEPAD_BUTTON_START },
					{ 10, GLFW_GAMEPAD_BUTTON_LEFT_THUMB  },
					{ 11, GLFW_GAMEPAD_BUTTON_RIGHT_THUMB },
					{ 12, GLFW_GAMEPAD_BUTTON_DPAD_UP    },
					{ 13, GLFW_GAMEPAD_BUTTON_DPAD_DOWN  },
					{ 14, GLFW_GAMEPAD_BUTTON_DPAD_LEFT  },
					{ 15, GLFW_GAMEPAD_BUTTON_DPAD_RIGHT },
				};
				for (auto& [raw, mapped] : rawToGLFW)
				{
					if (raw < buttonCount)
						state.buttons[mapped] = rawButtons[raw];
				}
				// Raw buttons 6 (ZL) and 7 (ZR)
				if (6 < buttonCount) state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER]  = rawButtons[6] ? 1.f : -1.f;
				if (7 < buttonCount) state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] = rawButtons[7] ? 1.f : -1.f;

				// D-pad
				int hatCount = 0;
				const unsigned char* hats = glfwGetJoystickHats(jid, &hatCount);
				if (hats && hatCount > 0)
				{
					unsigned char hat = hats[0];
					state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP]    = (hat & GLFW_HAT_UP)    ? GLFW_PRESS : GLFW_RELEASE;
					state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] = (hat & GLFW_HAT_RIGHT) ? GLFW_PRESS : GLFW_RELEASE;
					state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN]  = (hat & GLFW_HAT_DOWN)  ? GLFW_PRESS : GLFW_RELEASE;
					state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT]  = (hat & GLFW_HAT_LEFT)  ? GLFW_PRESS : GLFW_RELEASE;
				}
			}

			// Buttons
			for (int b = 0; b < NUM_TOTAL_CONTROLLER_BUTTON; ++b)
			{
				bool nowDown = (state.buttons[b] == GLFW_PRESS);
				bool wasDown = m_controllerDown[jid][b];

				if (nowDown && !wasDown)
				{
					m_controllerDown    [jid][b] = true;
					m_controllerPressed [jid][b] = true;
					if (m_dispatcher)
					{
						auto action = TranslateControllerButtonToAction(b);
						if (action != InputEvent::InputAction::NumTotalInputAction)
							m_dispatcher->QueueEvent(std::make_unique<InputEvent>(action, InputEvent::InputActionState::Pressed));
					}
				}
				else if (!nowDown && wasDown)
				{
					m_controllerDown    [jid][b] = false;
					m_controllerReleased[jid][b] = true;
					if (m_dispatcher)
					{
						auto action = TranslateControllerButtonToAction(b);
						if (action != InputEvent::InputAction::NumTotalInputAction)
							m_dispatcher->QueueEvent(std::make_unique<InputEvent>(action, InputEvent::InputActionState::Released));
					}
				}
			}

			// Axes
			for (int a = 0; a <= GLFW_GAMEPAD_AXIS_LAST; ++a)
				m_controllerRawAxes[jid][a] = state.axes[a];

			// Map raw axes to directional booleans
			const float dz = AXIS_DEAD_ZONE;
			struct { int axisDir; float value; } axisMap[] = {
				{ CTRL_LSTICK_LEFT,  -state.axes[GLFW_GAMEPAD_AXIS_LEFT_X]  },
				{ CTRL_LSTICK_RIGHT,  state.axes[GLFW_GAMEPAD_AXIS_LEFT_X]  },
				{ CTRL_LSTICK_UP,    -state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]  },
				{ CTRL_LSTICK_DOWN,   state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]  },
				{ CTRL_RSTICK_LEFT,  -state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] },
				{ CTRL_RSTICK_RIGHT,  state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] },
				{ CTRL_RSTICK_UP,    -state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] },
				{ CTRL_RSTICK_DOWN,   state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] },
				{ CTRL_L_TRIGGER, (state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER]  + 1.f) * 0.5f },
				{ CTRL_R_TRIGGER, (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] + 1.f) * 0.5f },
			};

			for (auto& [axisDir, value] : axisMap)
			{
				bool nowActive = (value > dz);
				bool wasActive = m_controllerAxisActive[jid][axisDir];

				if (nowActive && !wasActive)
				{
					m_controllerAxisActive [jid][axisDir] = true;
					m_controllerAxisPressed[jid][axisDir] = true;
					if (m_dispatcher)
					{
						auto action = TranslateControllerAxisToAction(axisDir);
						if (action != InputEvent::InputAction::NumTotalInputAction)
							m_dispatcher->QueueEvent(std::make_unique<InputEvent>(action, InputEvent::InputActionState::Pressed));
					}
				}
				else if (nowActive && wasActive)
				{
					// Stick still held - Repeat every frame
					if (m_dispatcher)
					{
						auto action = TranslateControllerAxisToAction(axisDir);
						if (action != InputEvent::InputAction::NumTotalInputAction)
							m_dispatcher->QueueEvent(std::make_unique<InputEvent>(action, InputEvent::InputActionState::Repeat));
					}
				}
				else if (!nowActive && wasActive)
				{
					// Stick returned to centre - Released
					m_controllerAxisActive  [jid][axisDir] = false;
					m_controllerAxisReleased[jid][axisDir] = true;
					if (m_dispatcher)
					{
						auto action = TranslateControllerAxisToAction(axisDir);
						if (action != InputEvent::InputAction::NumTotalInputAction)
							m_dispatcher->QueueEvent(std::make_unique<InputEvent>(action, InputEvent::InputActionState::Released));
					}
				}
			}
		}

		// Process all queued events after input update and before the other systems
		m_dispatcher->ProcessEvents();
	}

	void InputSystem::ResetInputStates() {
		// Reset states back to false
		m_keysPressed.fill(false);
		m_keysReleased.fill(false);
		m_mousePressed.fill(false);
		m_mouseReleased.fill(false);
		for (int i = 0; i < MAX_CONTROLLERS; ++i)
		{
			m_controllerPressed [i].fill(false);
			m_controllerReleased[i].fill(false);
			m_controllerAxisPressed [i].fill(false);
			m_controllerAxisReleased[i].fill(false);
		}
	}

	InputSystem::KeyboardKeys InputSystem::TranslateGLFWKeyboardKey(int glfwKey) {
		switch (glfwKey) {
		case GLFW_KEY_W: return KeyboardKeys::W;
		case GLFW_KEY_A: return KeyboardKeys::A;
		case GLFW_KEY_S: return KeyboardKeys::S;
		case GLFW_KEY_D: return KeyboardKeys::D;
		case GLFW_KEY_H: return KeyboardKeys::H;
		case GLFW_KEY_J: return KeyboardKeys::J;
		case GLFW_KEY_SPACE: return KeyboardKeys::SPACE;
		case GLFW_KEY_ENTER: return KeyboardKeys::ENTER;
		case GLFW_KEY_RIGHT: return KeyboardKeys::RIGHT_ARROW;
		case GLFW_KEY_LEFT: return KeyboardKeys::LEFT_ARROW;
		case GLFW_KEY_UP: return KeyboardKeys::UP_ARROW;
		case GLFW_KEY_DOWN: return KeyboardKeys::DOWN_ARROW;
		case GLFW_MOD_SHIFT: return KeyboardKeys::SHIFT;
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
		case GLFW_KEY_SPACE: return InputEvent::InputAction::PlayerJumpAbility;

		case GLFW_KEY_W: return InputEvent::InputAction::PlayerUp;
		case GLFW_KEY_S: return InputEvent::InputAction::PlayerDown;

		case GLFW_KEY_J: return InputEvent::InputAction::PlayerPossess;
		//case GLFW_KEY_LEFT: return InputEvent::InputAction::PlayerMoveLeft;
		//case GLFW_KEY_RIGHT: return InputEvent::InputAction::PlayerMoveRight;
		//case GLFW_KEY_UP: return InputEvent::InputAction::PlayerJumpAbility;
		//case GLFW_KEY_DOWN: return InputEvent::InputAction::PlayerDown;
		//case GLFW_KEY_A: return InputEvent::InputAction::PlayerSpecialAbility;
		//case GLFW_KEY_S: return InputEvent::InputAction::PlayerPossess;
		//case GLFW_KEY_D: return InputEvent::InputAction::PlayerUnPossess;

		case GLFW_KEY_ENTER: return InputEvent::InputAction::SkipDialogueOrCameraPan;
		case GLFW_KEY_1: return InputEvent::InputAction::Ability1;
		case GLFW_KEY_2: return InputEvent::InputAction::Ability2;
		case GLFW_KEY_3: return InputEvent::InputAction::Ability3;

		case GLFW_KEY_0: return InputEvent::InputAction::LevelTutorial;
		case GLFW_KEY_9: return InputEvent::InputAction::Level1;
		case GLFW_KEY_8: return InputEvent::InputAction::Level2;
		case GLFW_KEY_7: return InputEvent::InputAction::Level3;

			// Window tests
		case GLFW_KEY_F1: return InputEvent::InputAction::DisplayFPS;
		case GLFW_KEY_F2: return InputEvent::InputAction::DefaultDialogue;
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
		case GLFW_KEY_C: // (CTRL+C)
			return ((modifier & GLFW_MOD_CONTROL) > 0) ? InputEvent::InputAction::Copy : InputEvent::InputAction::NumTotalInputAction;
		case GLFW_KEY_V: // (CTRL+C)
			return ((modifier & GLFW_MOD_CONTROL) > 0) ? InputEvent::InputAction::Paste : InputEvent::InputAction::NumTotalInputAction;
		case GLFW_KEY_P: // (CTRL+P)
			return ((modifier & GLFW_MOD_CONTROL) > 0) ? InputEvent::InputAction::PhysicsDebugMode : InputEvent::InputAction::PhysicsDebugStep;
		case GLFW_KEY_Z: // (CTRL+Z)
			return ((modifier & GLFW_MOD_CONTROL) > 0) ? InputEvent::InputAction::UndoStep : InputEvent::InputAction::NumTotalInputAction;
		case GLFW_KEY_Y: // (CTRL+Y)
			return ((modifier & GLFW_MOD_CONTROL) > 0) ? InputEvent::InputAction::RedoStep : InputEvent::InputAction::NumTotalInputAction;
		case GLFW_KEY_GRAVE_ACCENT: // (`+SHIFT)
			return ((modifier & GLFW_MOD_CONTROL) > 0) ? InputEvent::InputAction::ToggleCheat : InputEvent::InputAction::NumTotalInputAction;

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

	void InputSystem::JoystickCallback(int joystickId, int event)
	{
		if (event == GLFW_CONNECTED)
		{
			m_controllerConnected[joystickId] = glfwJoystickPresent(joystickId) == GLFW_TRUE;
			if (m_dispatcher)
				m_dispatcher->QueueEvent(std::make_unique<InputEvent>(
					InputEvent::InputAction::ControllerConnected,
					InputEvent::InputActionState::Pressed));
		}
		else if (event == GLFW_DISCONNECTED)
		{
			m_controllerConnected[joystickId] = false;

			// Fire Released for every active button/axis BEFORE clearing state,
			// so subscriber flags (e.g. PlayerController movingLeft) get cleared
			if (m_dispatcher)
			{
				for (int b = 0; b < NUM_TOTAL_CONTROLLER_BUTTON; ++b)
				{
					if (m_controllerDown[joystickId][b])
					{
						auto action = TranslateControllerButtonToAction(b);
						if (action != InputEvent::InputAction::NumTotalInputAction)
							m_dispatcher->QueueEvent(std::make_unique<InputEvent>(
								action, InputEvent::InputActionState::Released));
					}
				}
				for (int a = 0; a < NUM_TOTAL_CONTROLLER_AXIS; ++a)
				{
					if (m_controllerAxisActive[joystickId][a])
					{
						auto action = TranslateControllerAxisToAction(a);
						if (action != InputEvent::InputAction::NumTotalInputAction)
							m_dispatcher->QueueEvent(std::make_unique<InputEvent>(
								action, InputEvent::InputActionState::Released));
					}
				}
				// Notify game of disconnection
				m_dispatcher->QueueEvent(std::make_unique<InputEvent>(
					InputEvent::InputAction::ControllerDisconnected,
					InputEvent::InputActionState::Pressed));
			}

			// Clear all state after dispatching
			m_controllerDown[joystickId].fill(false);
			m_controllerPressed[joystickId].fill(false);
			m_controllerReleased[joystickId].fill(false);
			m_controllerAxisActive[joystickId].fill(false);
			m_controllerAxisPressed[joystickId].fill(false);
			m_controllerAxisReleased[joystickId].fill(false);
		}
	}

	InputEvent::InputAction InputSystem::TranslateControllerButtonToAction(int button)
	{
		switch (button)
		{
		case CTRL_FACE_DOWN:    return InputEvent::InputAction::PlayerJumpAbility;
		case CTRL_FACE_RIGHT:   return InputEvent::InputAction::SkipDialogueOrCameraPan;
		case CTRL_FACE_UP:      return InputEvent::InputAction::PauseGame;
		case CTRL_FACE_LEFT:    return InputEvent::InputAction::PlayerPossess;
		case CTRL_DPAD_UP:      return InputEvent::InputAction::PlayerUp;
		case CTRL_DPAD_DOWN:    return InputEvent::InputAction::PlayerDown;
		case CTRL_DPAD_LEFT:    return InputEvent::InputAction::PlayerMoveLeft;
		case CTRL_DPAD_RIGHT:   return InputEvent::InputAction::PlayerMoveRight;
		case CTRL_START:        return InputEvent::InputAction::PauseGame;
		default:                return InputEvent::InputAction::NumTotalInputAction;
		}
	}

	InputEvent::InputAction InputSystem::TranslateControllerAxisToAction(int axisDir)
	{
		// Both sticks map to movement/menu navigation so either stick works.
		switch (axisDir)
		{
		case CTRL_LSTICK_LEFT: [[fallthrough]];
		case CTRL_RSTICK_LEFT:  return InputEvent::InputAction::PlayerMoveLeft;
		case CTRL_LSTICK_RIGHT: [[fallthrough]];
		case CTRL_RSTICK_RIGHT: return InputEvent::InputAction::PlayerMoveRight;
		case CTRL_LSTICK_UP: [[fallthrough]];
		case CTRL_RSTICK_UP:    return InputEvent::InputAction::PlayerUp;
		case CTRL_LSTICK_DOWN: [[fallthrough]];
		case CTRL_RSTICK_DOWN:  return InputEvent::InputAction::PlayerDown;
		default:                return InputEvent::InputAction::NumTotalInputAction;
		}
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

	// Queries for controller -----------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------

	bool InputSystem::IsControllerConnected(int jid) const { return jid < MAX_CONTROLLERS && m_controllerConnected[jid]; }
	bool InputSystem::IsControllerButtonDown    (int b, int jid) const { return jid < MAX_CONTROLLERS && m_controllerDown    [jid][b]; }
	bool InputSystem::IsControllerButtonPressed (int b, int jid) const { return jid < MAX_CONTROLLERS && m_controllerPressed [jid][b]; }
	bool InputSystem::IsControllerButtonReleased(int b, int jid) const { return jid < MAX_CONTROLLERS && m_controllerReleased[jid][b]; }
	float InputSystem::GetControllerAxis(int axisIndex, int jid) const
	{
		if (jid >= MAX_CONTROLLERS || axisIndex > GLFW_GAMEPAD_AXIS_LAST) return 0.f;
		return m_controllerRawAxes[jid][axisIndex];
	}
	bool InputSystem::IsControllerAxisActive(int axisDir, int jid) const { return jid < MAX_CONTROLLERS && m_controllerAxisActive[jid][axisDir]; }

}	// NightSnack

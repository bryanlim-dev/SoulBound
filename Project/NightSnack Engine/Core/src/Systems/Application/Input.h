/******************************************************************************
 * File:        Input.h
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created Input System
 *
 * Description:
 * This file contains the declarations of the Input System.
 * The Input System gets its key and mouse inputs via GLFW callbacks.
 * It will then dispatch Input Events via the Event Dispatcher.
 * Other Systems in the ECS can check for the state of the specific keys via Getter functions.
 * 
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#ifndef INPUT_HPP
#define INPUT_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <array>
#include "Core/ECS/ISystem.h"
#include "Core/ECS/Coordinator.h"
#include "Systems/Application/IEvent.h"

namespace NightSnack {

	class InputSystem : public ISystem {

	public:

		// Specific keyboard keys to check for input state
		//When updating this update the one in Engine.cs
		enum KeyboardKeys {
			W = 0,
			A,
			S,
			D,
			H,
			SPACE,
			ESC,

			RIGHT_ARROW,
			LEFT_ARROW,
			UP_ARROW,

			NUM_TOTAL_KEYBOARD_KEY	// MUST BE THE LAST
		};

		// Specific mouse buttons to check for input state
		enum MouseButtons {
			LMB = 0,
			RMB,
			NUM_TOTAL_MOUSE_BUTTON
		};

		/************************************************************************/
		/*!
		\brief
		Constructor for the Input System.
		Calls Init().

		\param[in] Coord
		Reference to the ECS Coordinator

		\param[in] disp
		Reference to the Event Dispatcher
		*/
		/************************************************************************/
		InputSystem(Coordinator& coord, EventDispatcher& disp) : m_coord{ coord } {
			Init(coord.GetSystem<ApplicationSystem>()->GetNativeHandle(), disp);
		}

		/************************************************************************/
		/*!
		\brief
		Initialize function
		Sets the key, mouse, cursor GLFW function callbacks.
		Stores the event dispatcher.

		\param[in] window
		GLFWwindow pointer to the ApplicationSystem's window handle.
		Needed for the setting of GLFW function callbacks.

		\param[in] disp
		Reference to the Event Dispatcher.
		*/
		/************************************************************************/
		void Init(GLFWwindow* window, EventDispatcher& disp);

		/************************************************************************/
		/*!
		\brief
		Main update called by the core.
		Updating of mouse positions is done here.

		\param[in] dt
		Delta time

		\param[in] Coord
		Reference to the ECS Coordinator
		*/
		/************************************************************************/
		void Update() override;

		/************************************************************************/
		/*!
		\brief
		Resets all the key and mouse states back to false
		*/
		/************************************************************************/
		void ResetInputStates();

		// Keyboard queries
		/************************************************************************/
		/*!
		\brief
		Checks whether this specific key is being pressed down repeatedly or not

		\param[in] key
		The key whose int value is represented in enum KeyboardKeys

		\return
		True if this specific key is being pressed down repeatedly
		*/
		/************************************************************************/
		bool IsKeyDown(int key) const;

		/************************************************************************/
		/*!
		\brief
		Checks whether this specific key has been pressed or not

		\param[in] key
		The key whose int value is represented in enum KeyboardKeys

		\return
		True if this specific key has been pressed
		*/
		/************************************************************************/
		bool IsKeyPressed(int key) const;

		/************************************************************************/
		/*!
		\brief
		Checks whether this specific key has been released

		\param[in] key
		The key whose int value is represented in enum KeyboardKeys

		\return
		True if this specific key has been released
		*/
		/************************************************************************/
		bool IsKeyReleased(int key) const;

		// Keyboard modifier queries
		/************************************************************************/
		/*!
		\brief
		Checks whether the shift key is being pressed down repeatedly or not

		\return
		True if the shift key is being pressed down repeatedly
		*/
		/************************************************************************/
		bool IsShiftDown() const;

		/************************************************************************/
		/*!
		\brief
		Checks whether the control key is being pressed down repeatedly or not

		\return
		True if the control key is being pressed down repeatedly
		*/
		/************************************************************************/
		bool IsCtrlDown() const;

		/************************************************************************/
		/*!
		\brief
		Checks whether the alternate key is being pressed down repeatedly or not

		\return
		True if the alternate key is being pressed down repeatedly
		*/
		/************************************************************************/
		bool IsAltDown() const;

		// Mouse queries
		/************************************************************************/
		/*!
		\brief
		Checks whether this specific mouse button is being pressed down repeatedly or not

		\param[in] key
		The key whose int value is represented in enum MouseButtons

		\return
		True if this specific mouse button is being pressed down repeatedly
		*/
		/************************************************************************/
		bool IsMouseButtonDown(int button) const;

		/************************************************************************/
		/*!
		\brief
		Checks whether this specific mouse button has been pressed

		\param[in] key
		The key whose int value is represented in enum MouseButtons

		\return
		True if this specific mouse button has been pressed
		*/
		/************************************************************************/
		bool IsMouseButtonPressed(int button) const;

		/************************************************************************/
		/*!
		\brief
		Checks whether this specific mouse button has been released

		\param[in] key
		The key whose int value is represented in enum MouseButtons

		\return
		True if this specific mouse button has been released
		*/
		/************************************************************************/
		bool IsMouseButtonReleased(int button) const;

		// Accessor functions for mouse position
		/************************************************************************/
		/*!
		\brief
		Gets the screen position for mouse position x coordinate

		\return
		Screen position for mouse position x coordinate
		*/
		/************************************************************************/
		float GetMousePosX() const;

		/************************************************************************/
		/*!
		\brief
		Gets the screen position for mouse position y coordinate
		
		\return
		Screen position for mouse position y coordinate
		*/
		/************************************************************************/
		float GetMousePosY() const;
		
		/************************************************************************/
		/*!
		\brief
		Gets the world position for mouse position x coordinate

		\return
		World position for mouse position x coordinate
		*/
		/************************************************************************/
		float GetMouseWorldPosX() const;

		/************************************************************************/
		/*!
		\brief
		Gets the world position for mouse position y coordinate

		\return
		World position for mouse position y coordinate
		*/
		/************************************************************************/
		float GetMouseWorldPosY() const;

		/************************************************************************/
		/*!
		\brief
		Gets the relative mouse movement since the last frame, in world frame for x coordinate

		\return
		Relative mouse movement since the last frame, in world frame for x coordinate
		*/
		/************************************************************************/
		float GetMouseDeltaPosX() const;

		/************************************************************************/
		/*!
		\brief
		Gets the relative mouse movement since the last frame, in world frame for y coordinate

		\return
		Relative mouse movement since the last frame, in world frame for y coordinate
		*/
		/************************************************************************/
		float GetMouseDeltaPosY() const;

		/************************************************************************/
		/*!
		\brief
		Get the system's signature.
		Required by ISystem inheritance

		\return
		Signature (set of required components)
		*/
		/************************************************************************/
		Signature GetSignature() const override { return m_systemSignature; }

		/************************************************************************/
		/*!
		\brief
		Gets the name of this system, for debugging purposes
		Required by ISystem inheritance

		\return
		Name of the system
		*/
		/************************************************************************/
		std::string GetName() const { return "InputSystem"; }

	private:

		/************************************************************************/
		/*!
		\brief
		Translates a GLFW key into an enum from InputEvent::InputAction.
		This enum value is dispatched through an InputEvent using the Event Dispatcher

		\param[in] glfwKey
		The GLFW key to be converted

		\param[in] modifier
		The bitfield of modifier keys, to be used to check for combos like (CTRL+S etc)

		\return
		Enum value from InputEvent::InputAction representing what input action it is
		*/
		/************************************************************************/
		static InputEvent::InputAction TranslateGLFWKeyToAction(int glfwKey, int modifier);

		/************************************************************************/
		/*!
		\brief
		Translates a GLFW key state into an enum from InputEvent::InputActionState.
		This enum value is dispatched through an InputEvent using the Event Dispatcher

		\param[in] glfwKeyState
		The GLFW key state to be converted

		\return
		Enum value from InputEvent::InputActionState representing what input action state it is
		*/
		/************************************************************************/
		static InputEvent::InputActionState TranslateGLFWKeyStateToAction(int glfwkeyState);

		/************************************************************************/
		/*!
		\brief
		Translates a GLFW key into an enum from KeyboardKeys

		\param[in] glfwKey
		The GLFW key to be converted

		\return
		Enum value from KeyboardKeys
		*/
		/************************************************************************/
		static KeyboardKeys TranslateGLFWKeyboardKey(int glfwKey);

		/************************************************************************/
		/*!
		\brief
		Translates a GLFW key into an enum from MouseButtons

		\param[in] glfwButton
		The GLFW key to be converted

		\return
		Enum value from MouseButtons
		*/
		/************************************************************************/
		static MouseButtons TranslateGLFWMouseButton(int glfwButton);

		/************************************************************************/
		/*!
		\brief
		Key callback function which will be triggered by GLFW callbacks.
		Translate the key and its action accordingly.
		Dispatch an InputEvent via the Event Dispatcher.
		Update the state of the key accordingly, in the arrays in this Input System.

		\param[in] window
		GLFWwindow pointer, not to be used here

		\param[in] key
		The GLFW key that has been triggered

		\param[in] scancode
		Not to be used here, only useful for implementing custom keyboard layouts

		\param[in] action
		The state of the GLFW key that has been triggered

		\param[in] mods
		Bitfield of modifier keys
		*/
		/************************************************************************/
		static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

		/************************************************************************/
		/*!
		\brief
		Mouse callback function which will be triggered by GLFW callbacks.
		Translate the button and its action accordingly.
		Dispatch an InputEvent via the Event Dispatcher.
		Update the state of the button accordingly, in the arrays in this Input System.

		\param[in] window
		GLFWwindow pointer, not to be used here

		\param[in] button
		The GLFW button that has been triggered

		\param[in] action
		The state of the GLFW button that has been triggered

		\param[in] mods
		Bitfield of modifier keys
		*/
		/************************************************************************/
		static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

		/************************************************************************/
		/*!
		\brief
		Cursor callback function which will be triggered by GLFW callbacks.
		Updates the mouse position.

		\param[in] window
		GLFWwindow pointer, not to be used here

		\param[in] xPos
		Represents the current x mouse position

		\param[in] yPos
		Represents the current y mouse position
		*/
		/************************************************************************/
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
		float m_prevMouseWorldPosX{}, m_prevMouseWorldPosY{};	// Represents the previous position in the previous frame
		float m_deltaWorldPosX{}, m_deltaWorldPosY{};			// Represents the relative movement since the last frame

		Signature m_systemSignature{};

		// System event dispatcher
		static EventDispatcher* m_dispatcher;
		// Reference to ECS coordinator
		Coordinator& m_coord;
	};

}	// NightSnack

#endif // INPUT_HPP
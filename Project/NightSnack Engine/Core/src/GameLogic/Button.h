/******************************************************************************
 * File:        Button.h
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 100%) - Button class and GameUI system
 *
 *
 * Description:
 *	This file defines a Button class and a GameUI system for handling UI buttons.
 *	Takes care of button hover and click interactions, executing associated actions.
 *
 * Copyright   2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once
#include "Core/ECS/Coordinator.h"
#include "Systems/Application/IEvent.h"
#include "Systems/Application/Input.h"
#include "Systems/AudioSystem.h"
#include <string>
#include <unordered_map>
#include <iostream>

namespace NightSnack {

	class Button
	{

	public:
		/**
		* @brief default constructor
		*/
		Button() : m_action(ButtonAction::PLAYAUDIO), m_onClick(nullptr),
			m_onHover(nullptr), m_active(true) {}

		/**
		 * @brief constructor to initialize the button with a specific action and callbacks
		 * @param action action associated with the button
		 * @param onClick click event
		 * @param onHover hover event
		 */
		Button(ButtonAction action, void(*onClick)(), void(*onHover)())
			: m_action{ action }, m_onClick{ onClick }, m_onHover{ onHover }, m_active(true) {}

		/**
		* @brief activate the button by clicking
		*/
		void Activate() {
			if (m_onClick) (*m_onClick)();
		}

		/**
		* @brief trigger hover
		*/
		void Hover() {
			if (m_onHover) (*m_onHover)();
		}

		/**
		* @brief check if button is active
		* @return true if active, else false
		*/
		operator bool() { return m_active; }

		/**
		* @brief set the active state
		* @param active true if button is activated, else false
		*/
		void SetActive(bool active) { m_active = active; }

		/**
		* @brief retrieve the button action
		* @return the respective action
		*/
		ButtonAction GetAction() const { return m_action; }

	private:

		ButtonAction m_action;
		void(*m_onClick)();
		void(*m_onHover)();
		bool m_active;
	};

	/**
	* @brief struct to handle audio playing
	*/
	//struct PlayAudio {
	//	size_t soundID; // id of the sound to play

	//	/**
	//	* @brief play sound using AudioSystem
	//	* @param coord reference to coordinator
	//	*/
	//	void operator()(Coordinator& coord)
	//	{
	//		coord.GetSystem<AudioSystem>()->PlaySFX(soundID);
	//	}
	//};

	//inline void OnPlayClick() {
	//	//std::cout << "Play button clicked!\n";
	//}

	//inline void OnPlayHover() {
	//	//std::cout << "Play button hovered!\n";
	//}

	// GameUI system
	class GameUI : public ISystem {

	public:
		/**
		 * @brief constructor initializing the GameUI system
		 * @param coordinator reference to coordinator
		 * @param dispatcher reference to event dispatcher
		 */
		GameUI(Coordinator& coordinator, EventDispatcher& dispatcher)
			: m_coord(coordinator),
			m_dispatcher(&dispatcher),
			m_mouseWasPressed(false),
			mousePressed(false),
			mouseClickedThisFrame(false)
		{

			// set system signature
			m_systemSignature.set(GetComponentTypeID<ButtonInfo>());
			m_systemSignature.set(GetComponentTypeID<Transform>());
			m_systemSignature.set(GetComponentTypeID<Collider>());


			// Create button behaviour
			//Button playSound(ButtonAction::PLAYAUDIO, &OnPlayClick, &OnPlayHover);
			//m_buttons[ButtonAction::PLAYAUDIO] = playSound;
			// Store into m_buttons;

			// subscribe to dispatcher lambda to get input events for mouse interaction
			m_dispatcher->Subscribe<InputEvent>([this](const InputEvent& event) {

				// detect mouse pressed and LMB clicked
				if (event.pressed == InputEvent::InputActionState::Pressed &&
					event.inputAction == InputEvent::InputAction::ClickLMB) {
					mouseClickedThisFrame = true;
				}

				// handle stress test end key
				//if (event.inputAction == InputEvent::InputAction::EndStressTest)
				//{
				//	if (m_stress) {
				//		m_coord.GetSystem<EntityFactory>()->StressTestEnd();
				//		m_stress = false;
				//		m_layer = false;
				//	}
				//}
				});
		}

		/**
		* @brief updates and handle button interactions
		* @param steps number of simulation steps
		*/
		void Update(int steps) override
		{
			(void)steps;

			// get mouse x and y position
			float mouseX = m_coord.GetSystem<InputSystem>()->GetMousePosX();
			float mouseY = m_coord.GetSystem<InputSystem>()->GetMousePosY();

			bool mouseClicked = mouseClickedThisFrame;
			mouseClickedThisFrame = false;

			// get audio system
			auto audioSystem = m_coord.GetSystem<AudioSystem>().get();

			// iterate through all entities managed by the system
			for (Entity e : m_entities)
			{
				// skip entities missing required components
				if (!m_coord.HasComponent<ButtonInfo>(e) ||
					!m_coord.HasComponent<Transform>(e) ||
					!m_coord.HasComponent<Collider>(e))
					continue;

				auto& buttonInfo = m_coord.ModifyComponent<ButtonInfo>(e);
				const auto& transform = m_coord.GetComponent<Transform>(e);
				const auto& collider = m_coord.GetComponent<Collider>(e);

				//transform.pos = m_coord.GetWindowWidth

				// reset button states
				bool wasHovered = buttonInfo.isHovered;
				buttonInfo.isHovered = false;
				buttonInfo.wasClicked = false;

				// check if mouse is over button
				if (IsMouseOverButton(mouseX, mouseY, transform, collider))
				{
					buttonInfo.isHovered = true;

					// trigger the hover function on first hover
					if (!wasHovered)
					{
						// std::cout << "Button hovered\n";

						// call hover callback
						auto it = m_buttons.find(buttonInfo.action);
						if (it != m_buttons.end())
						{
							it->second.Hover();
						}
					}

					// handle button click
					if (mouseClicked)
					{
						buttonInfo.wasClicked = true;
						//std::cout << "Button clicked\n";

						// execute button action
						// in this case is play click button audio
						ExecuteButtonAction(buttonInfo.action, m_coord, audioSystem);

						// call click callback
						auto it = m_buttons.find(buttonInfo.action);
						if (it != m_buttons.end())
						{
							it->second.Activate();
						}
					}

					// change colour when hover for visual feedback
					if (m_coord.HasComponent<Color>(e))
					{
						auto& color = m_coord.ModifyComponent<Color>(e);
						if (buttonInfo.isHovered)
						{
							// lighten color when hovered
							color.r = 1.0f;
							color.g = 0.6f;
							color.b = 0.2f;
						}
						else
						{
							// normal colour
							color.r = 1.0f;
							color.g = 0.4f;
							color.b = 0.0f;
						}
					}
				}
				else
				{
					// reset to normal colour if not hovering
					if (m_coord.HasComponent<Color>(e))
					{
						auto& color = m_coord.ModifyComponent<Color>(e);
						color.r = 1.0f;
						color.g = 0.4f;
						color.b = 0.0f;
					}
				}
			}
		}

		/**
		 * @brief retrieve the system signature
		 * @return system signature
		 */
		Signature GetSignature() const override { return m_systemSignature; }

		/**
		 * @brief retrieve the system name
		 * @return system name
		 */
		std::string GetName() const { return "GameUI"; }

	private:
		std::unordered_map<ButtonAction, Button> m_buttons;

		Signature m_systemSignature;
		Coordinator& m_coord;
		EventDispatcher* m_dispatcher;

		bool m_mouseWasPressed;
		bool mousePressed;
		bool mouseClickedThisFrame;
		bool m_stress{ false };

		/**
		 * @brief checks if the mouse is over a button using AABB collision
		 * @param mouseX mouse x position
		 * @param mouseY mouse y position
		 * @param transform transform of the button
		 * @param collider collider of the component
		 * @return true if mouse is over the button, else false
		 */
		bool IsMouseOverButton(float mouseX, float mouseY, const Transform& transform,
			const Collider& collider)
		{
			// calculate button bounds (centered AABB)
			float halfWidth = collider.width * 0.5f;
			float halfHeight = collider.height * 0.5f;

			float left = transform.pos.x - halfWidth;
			float right = transform.pos.x + halfWidth;
			float bottom = transform.pos.y - halfHeight;
			float top = transform.pos.y + halfHeight;

			// check if mouse is inside bounds
			return (mouseX >= left && mouseX <= right && mouseY >= bottom && mouseY <= top);
		}

		/**
		 * @brief execute the respective button actions
		 * @param action action to execute
		 * @param coord reference to coordinator
		 * @param audioSystem pointer to audio system
		 */
		void ExecuteButtonAction(ButtonAction action, Coordinator& coord, AudioSystem* audioSystem)
		{
			(void)coord;

			switch (action)
			{
				// play audio
			case ButtonAction::PLAYAUDIO:
				// Play button click sound
				if (audioSystem)
				{
					const auto& audioKeys = audioSystem->GetAudioKeys();
					audioSystem->PlaySFX(audioKeys.buttonClick);
					//std::cout << "Playing click sound\n";
				}
				break;

			case ButtonAction::PLAYAUDIO2:
				// Play button click sound
				if (audioSystem)
				{
					const auto& audioKeys = audioSystem->GetAudioKeys();
					audioSystem->PlaySFX(audioKeys.playerJump);
					//std::cout << "Playing jump sound\n";
				}
				break;

			case ButtonAction::PLAYAUDIO3:
				// Play button click sound
				if (audioSystem)
				{
					const auto& audioKeys = audioSystem->GetAudioKeys();
					audioSystem->PlaySFX(audioKeys.playerHit);
					//std::cout << "Playing hit sound\n";
				}
				break;
			// play stress test
			case ButtonAction::PLAYSTRESSTEST:
				//m_coord.GetSystem<EntityFactory>()->m_stress = true;
				break;

			default:
				break;
			}
		}
	};

} // namespace NightSnack

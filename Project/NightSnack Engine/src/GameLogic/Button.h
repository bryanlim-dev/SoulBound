/******************************************************************************
 * File:        Button.h
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 100%)
 *
 *
 * Description:
 *
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
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

	class Button {

	public:
		// default construcrtor
		Button() : m_action(ButtonAction::PLAYAUDIO), m_onClick(nullptr),
			m_onHover(nullptr), m_active(true) {}

		Button(ButtonAction action, void(*onClick)(), void(*onHover)())
			: m_action{ action }, m_onClick{ onClick }, m_onHover{ onHover }, m_active(true) {}

		void Activate() {
			if (m_onClick) (*m_onClick)();
		}

		void Hover() {
			if (m_onHover) (*m_onHover)();
		}

		operator bool() { return m_active; }

		void SetActive(bool active) { m_active = active; }
		ButtonAction GetAction() const { return m_action; }

	private:
		ButtonAction m_action;
		void(*m_onClick)();
		void(*m_onHover)();
		bool m_active;
	};

	struct PlayAudio {
		size_t soundID;

		void operator()(Coordinator& coord) {
			coord.GetSystem<AudioSystem>()->PlaySFX(soundID);
		}
	};

	inline void OnPlayClick() {
		//std::cout << "Play button clicked!\n";
	}

	inline void OnPlayHover() {
		//std::cout << "Play button hovered!\n";
	}

	class GameUI : public ISystem {

	public:
		GameUI(Coordinator& coordinator, EventDispatcher& dispatcher)
			: m_coord(coordinator),
			m_dispatcher(&dispatcher),
			m_mouseWasPressed(false),
			mousePressed(false),
			mouseClickedThisFrame(false)
		{
	
			// Set System Signature;
			m_systemSignature.set(GetComponentTypeID<ButtonInfo>());
			m_systemSignature.set(GetComponentTypeID<Transform>());
			m_systemSignature.set(GetComponentTypeID<Collider>());


			// Create button behaviour
			Button playSound(ButtonAction::PLAYAUDIO, &OnPlayClick, &OnPlayHover);
			m_buttons[ButtonAction::PLAYAUDIO] = playSound;

			// Store into m_buttons;

			// Dispatcher lambda
			m_dispatcher->Subscribe<InputEvent>([this](const InputEvent& event) {


				if (event.pressed == InputEvent::InputActionState::Pressed &&
					event.inputAction == InputEvent::InputAction::ClickLMB) {
					mouseClickedThisFrame = true;
				}

				});
		}

		void Update(int steps) override
		{
			(void)steps;

			float mouseX = m_coord.GetSystem<InputSystem>()->GetMousePosX();
			float mouseY = m_coord.GetSystem<InputSystem>()->GetMousePosY();


			bool mouseClicked = mouseClickedThisFrame;
			mouseClickedThisFrame = false;

			auto audioSystem = m_coord.GetSystem<AudioSystem>().get();

			for (Entity e : m_entities)
			{

				if (!m_coord.HasComponent<ButtonInfo>(e) ||
					!m_coord.HasComponent<Transform>(e) ||
					!m_coord.HasComponent<Collider>(e))
					continue;

				auto& buttonInfo = m_coord.ModifyComponent<ButtonInfo>(e);
				const auto& transform = m_coord.GetComponent<Transform>(e);
				const auto& collider = m_coord.GetComponent<Collider>(e);

				/*std::cout << "  - Button type: " << buttonInfo.type << "\n";
				std::cout << "  - Transform: pos(" << transform.pos.x << ", " << transform.pos.y
					<< ") scale(" << transform.scale.x << ", " << transform.scale.y << ")\n";
				std::cout << "  - Collider: " << collider.width << "x" << collider.height << "\n";*/

				// Reset states
				bool wasHovered = buttonInfo.isHovered;
				buttonInfo.isHovered = false;
				buttonInfo.wasClicked = false;

				// STEP 5: Check if mouse is over button (AABB collision)
				if (IsMouseOverButton(mouseX, mouseY, transform, collider)) 
				{
					buttonInfo.isHovered = true;

					// HOVER CALLBACK - Call only on first hover
					if (!wasHovered) {
						std::cout << "Button hovered\n";

						// Call hover callback
						auto it = m_buttons.find(buttonInfo.action);
						if (it != m_buttons.end()) {
							it->second.Hover();
						}
					}

					// CLICK DETECTION - Mouse clicked while hovering
					if (mouseClicked) {
						buttonInfo.wasClicked = true;
						std::cout << "Button clicked\n";

						// EXECUTE BUTTON ACTION
						ExecuteButtonAction(buttonInfo.action, m_coord, audioSystem);

						// Call click callback
						auto it = m_buttons.find(buttonInfo.action);
						if (it != m_buttons.end())
						{
							it->second.Activate();
						}
					}

					// VISUAL FEEDBACK - Change color when hovered
					if (m_coord.HasComponent<Color>(e))
					{
						auto& color = m_coord.ModifyComponent<Color>(e);
						if (buttonInfo.isHovered)
						{
							// Lighten color when hovered
							color.r = 1.0f;
							color.g = 0.6f;
							color.b = 0.2f;
						}
						else {
							// Normal color
							color.r = 1.0f;
							color.g = 0.4f;
							color.b = 0.0f;
						}
					}
				}
				else 
				{
					// Not hovering - reset to normal color
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

		Signature GetSignature() const override { return m_systemSignature; }
		std::string GetName() const { return "GameUI"; }

	private:
		std::unordered_map<ButtonAction, Button> m_buttons;

		Signature m_systemSignature;

		Coordinator& m_coord;
		EventDispatcher* m_dispatcher;

		bool m_mouseWasPressed;
		bool mousePressed;
		bool mouseClickedThisFrame;

		bool IsMouseOverButton(float mouseX, float mouseY, const Transform& transform,
			const Collider& collider) {
			// Calculate button bounds (centered AABB)
			float halfWidth = collider.width * 0.5f;
			float halfHeight = collider.height * 0.5f;

			float left = transform.pos.x - halfWidth;
			float right = transform.pos.x + halfWidth;
			float bottom = transform.pos.y - halfHeight;
			float top = transform.pos.y + halfHeight;

			// Check if mouse is inside bounds
			return (mouseX >= left && mouseX <= right && mouseY >= bottom && mouseY <= top);
		}

		void ExecuteButtonAction(ButtonAction action, Coordinator& coord, AudioSystem* audioSystem)
		{
			switch (action)
			{
			case ButtonAction::PLAYAUDIO:
				// Play button click sound
				if (audioSystem)
				{
					const auto& audioKeys = audioSystem->GetAudioKeys();
					audioSystem->PlaySFX(audioKeys.buttonClick);
					std::cout << "Playing button sound\n";
				}
				break;

			default:
				break;
			}
		}
	};

} // namespace NightSnack

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

    void OnPlayClick() {
        std::cout << "Play button clicked!\n";
    }

    void OnPlayHover() {
        std::cout << "Play button hovered!\n";
    }

    class GameUI : public ISystem {

        GameUI(Coordinator& coordinator, EventDispatcher* dispatcher) 
            : m_coord (coordinator), 
            m_dispatcher (dispatcher)
        {

            // Set System Signature;
            m_systemSignature.set(GetComponentTypeID<ButtonInfo>());
            m_systemSignature.set(GetComponentTypeID<Transform>());
            m_systemSignature.set(GetComponentTypeID<Collider>());

            // Create all buttons;
            Button playAudioButton(ButtonAction::PLAYAUDIO, &OnPlayClick, &OnPlayHover);
            m_buttons[ButtonAction::PLAYAUDIO] = playAudioButton;

            // Store into m_buttons;
        }

        void Update(int steps) override {
            (void)steps;

            float x = m_coord.GetSystem<InputSystem>()->GetMousePosX();
            float y = m_coord.GetSystem<InputSystem>()->GetMousePosY();
            
            //bool mouseClicked = InputSystem

            for (Entity e : m_entities) {
                Collider const& col = m_coord.GetComponent<Collider>(e);

                // Check for collision with mouse pos and col;

                // if (collision) -> OnHover();

                // if (collision && mouseclick) -> OnAction();
                // bi = m_coord.GetComponent<ButtonInfo>(e);
                // m_buttons[bi.action](m_coord);
            }
        }

        Signature GetSignature() const override { return m_systemSignature; }
        std::string GetName() const override { return "GameUI"; }

    private:
        std::unordered_map<ButtonAction, Button> m_buttons;

        Signature m_systemSignature;

        Coordinator& m_coord;
        EventDispatcher* m_dispatcher;
    };

} // namespace NightSnack

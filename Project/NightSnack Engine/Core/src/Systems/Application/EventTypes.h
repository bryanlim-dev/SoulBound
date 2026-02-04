/******************************************************************************
 * File:        EventTypes.h
 * Project:     NightSnack
 * Author(s):   Lim Wei Jie (Primary, 100%) - Created event types for Input, 
 *              Collision and Audio
 *
 * Description: This file contains the declarations of various event types 
 *              used in the engine.
 *
 * Copyright � 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

namespace NightSnack {

    // Types of events
    enum class EventType
    {
        Input,
        Collision,
        Audio,
        Graphics
    };

    // Input event (keyboard/mouse)
    class InputEvent : public IEvent
    {
    public:

        enum class InputAction {

            // Player movement
            PlayerMoveLeft,     // LEFT ARROW
            PlayerMoveRight,    // RIGHT ARROW
            PlayerJump,         // UP ARROW
            PlayerPossess,          // S KEY
            PlayerSpecialAbility,   // A KEY
            PlayerUnPossess,        // D KEY

            Ability1,
            Ability2,
            Ability3,

            ChangeAnimation,    // X

            // Debug tests
            ToggleDebugDraw,    // O
            PhysicsDebugStep,   // P

            // Window events
			DisplayFPS,         // F1
            ToggleFullscreen,   // F4
            PauseGame,          // Escape
            RestartGame,

            // Mouse events
            ClickLMB,           // LMB

            // Modifier events
            Copy,               // CTRL + C
            Paste,              // CTRL + V

            PhysicsDebugMode,   // CTRL + P
			UndoStep,           // CTRL + Z
			ToggleUIEditor,     // Backslash ( \ )

            NumTotalInputAction
        };

        enum class InputActionState {
            Pressed = 0,
            Repeat,
            Released,
            NumTotalInputActionState
        };

        InputAction inputAction;
        InputActionState pressed;

        InputEvent(InputAction a, InputActionState p) : inputAction(a), pressed(p) {}
        EventType GetType() const override { return EventType::Input; }
    };

    // Collision event
    class CollisionEvent : public IEvent
    {
    public:
        int entityA;
        int entityB;

        CollisionEvent(int a, int b) : entityA(a), entityB(b) {}
        EventType GetType() const override { return EventType::Collision; }
    };

    // Audio event (play/stop sound)
    class AudioEvent : public IEvent
    {
    public:
        enum class AudioAction { Play, Stop, Pause };
        AudioAction action;
        int soundId;

        AudioEvent(AudioAction act, int id)
            : action(act), soundId(id) {
        }
        EventType GetType() const override { return EventType::Audio; }
    };

    // Graphics event
    class GraphicsEvent : public IEvent
    {
    public:

        enum class GraphicsAction {
            CreateTexture = 0
        };

        GraphicsAction graphicsAction;

        GraphicsEvent(GraphicsAction a) : graphicsAction(a) {}
        EventType GetType() const override { return EventType::Graphics; }
    };

}   // NightSnack

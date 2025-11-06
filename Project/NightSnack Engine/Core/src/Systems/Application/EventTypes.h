/******************************************************************************
 * File:        EventTypes.h
 * Project:     NightSnack
 * Author(s):   Lim Wei Jie (Primary, 100%) - Created event types for Input, 
 *              Collision and Audio
 *
 * Description: This file contains the declarations of various event types 
 *              used in the engine.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

namespace NightSnack {

    // Types of events
    enum class EventType
    {
        Input,
        Collision,
        Audio
    };

    // Input event (keyboard/mouse)
    class InputEvent : public IEvent
    {
    public:

        enum class InputAction {
            // Manipulation of a single textured object
            ScaleUp,            // D
            ScaleDown,          // A
            Rotate,             // R

            // Physics tests
            DynamicBallTest,    // F
            GravityBallTest,    // G

            // Creation of entities
            CreateSquare,       // 1
            CreateCircle,       // 2
            CreateBananaCircle, // 3 -> texture entity;
            StressTest,         // Space
            LayerTest,          // L
            EndStressTest,      // Backspace
            InstanceTest,       // O

            // Player movement
            PlayerMoveLeft,     // Player controller with Arrow keys;
            PlayerMoveRight,
            PlayerJump,
            ChangePlayer,       // Down arrow;

            // Debug tests
            //ToggleSystemTimers, // TAB
            ToggleDebugDraw,    // T
            ToggleInstancedRendering, // I
            CrashTest,          // C
            PhysicsDebugStep,   // P

            // Mouse events
            ClickLMB,           // LMB

            // Modifier events
            Save,               // CTRL + S
            PhysicsDebugMode,   // CTRL + P
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

}   // NightSnack

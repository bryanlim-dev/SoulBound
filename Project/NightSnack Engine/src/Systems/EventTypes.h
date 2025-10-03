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
        ScaleUp,            // D
        ScaleDown,          // A
        Rotate,             // R
        DynamicBallTest,    // F
        GravityBallTest,    // G
        ClickLMB,           // LMB
        Save,               // CTRL + S

        CreateSquare,       // 1
        CreateCircle,       // 2
        CreateBananaCircle, // 3 -> texture entity;
        DoubleEntities,     // Space
        HalfEntities,       // Backspace

        PlayerMoveLeft,     // Player controller with Arrow keys;
        PlayerMoveRight,
        PlayerJump,
        ChangePlayer,       // Down arrow;

        ToggleSystemTimers, // Y
        ToggleDebugDraw,    // T
        PhysicsDebugMode,   // CTRL + P
        PhysicsDebugStep,   // P
        CrashTest,          // C

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
        : action(act), soundId(id) {}
    EventType GetType() const override { return EventType::Audio; }
};

#pragma once

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
        SelectNext, // W
        ScaleUp, // D
        ScaleDown, // A
        Rotate, // R
        ApplyForce, // SPACE
        ClickLMB, // LMB
        Save, // CTRL + S
        NumTotalInputAction,

        CreateSquare, // 1
        CreateCircle, // 2 -> has physics;
        CreateBananaCircle, // 3 -> texture entity;
<<<<<<< Updated upstream
        DoubleEntities,
        HalfEntities
=======
        DoubleEntities, // Space
        HalfEntities, // Backspace

        PlayerMoveLeft, // Player controller with Arrow keys;
        PlayerMoveRight,
        PlayerJump,
        ChangePlayer    // Down arrow;

    };

    enum class InputActionState {
        Pressed = 0,
        Repeat,
        Released
>>>>>>> Stashed changes
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

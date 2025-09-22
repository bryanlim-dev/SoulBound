#pragma once

enum class EventType
{
    Application,
    Input,
    Collision,
    Movement,
    Graphics
};

// Application/Window event
class AppWindowEvent : public IEvent
{
public:
    enum class Action 
    {
        // Application actions
        Start, Quit, Resize, Pause, Resume
    };
    Action action;

    // Only used for resize action
    int width = 0;
    int height = 0;

    // Constructor for application actions
    AppWindowEvent(Action act) : action(act) {}

    // Constructor for window actions
    AppWindowEvent(Action act, int w, int h)
        : action(act), width(w), height(h) {}

    EventType GetType() const override { return EventType::Application; }
};

// Input event (keyboard/mouse)
class InputEvent : public IEvent
{
public:
    int  key;
    bool pressed;

    InputEvent(int k, bool p) : key(k), pressed(p) {}
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

// Movement event
class MovementEvent : public IEvent
{
public:
    int   entityId;
    float x, y, z;

    MovementEvent(int id, float nx, float ny, float nz)
        : entityId(id), x(nx), y(ny), z(nz) {}
    EventType GetType() const override { return EventType::Movement; }
};

// Graphics event
class GraphicsEvent : public IEvent
{
public:
    enum class GraphicsAction { Render, LoadTexture, UnloadTexture };
    GraphicsAction action;
    int resourceId;

    GraphicsEvent(GraphicsAction act, int resId)
        : action(act), resourceId(resId) {}
    EventType GetType() const override { return EventType::Graphics; }
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
    EventType GetType() const override { return EventType::Application; }
};

/******************************************************************************
 * File:        Component.h
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan (Primary) - Created basic components | Defined json 
 *              overloads for reading components
 *				Goh Bei Lai (Secondary, 15%) - Created Graphics components (RenderLayer, Renderable, Camera)
 *              Bryan Lim Jun Jie (Secondary, 5%) - Created Button components (ButtonType, ButtonAction, ButtonInfo)
 *              Clement Ang Yang Ying (Secondary, 5%) - Create States component
 *
 * Description:
 * This file defines all components for this project, it also defines a specific
 * json overload for each component to read from a json file.
 *
 * Copyright � 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <json.hpp>
#include "MathLibrary/Math.h"
#include <variant>
namespace NightSnack {

    constexpr float FixedDT = 1.f / 60.f;

    using json = nlohmann::json;
    class StateBase; //Forward decleration

    // ================================================================================================
    // COMPONENTS
    // ================================================================================================
    // When updating this update the one in Engine.cs
    enum class LayerType : std::uint8_t {
        _ENGINE,
        BACKGROUND,
        GAMELAYER,
        FOREGROUND,
        GAMEUI,
        _ENGINE2
    };
    struct Transform {
        Vec3 pos;
        float rot{};
        Vec2 scale;

        Vec3 UIbasePos{ 0.f, 0.f, 0.f };
        Vec2 UIbaseScale{ 0.f, 0.f };

        LayerType layer;
        bool prefabModified{ false };
        bool isActive{ true };
    };

    struct Force {

        // Direction force
        Vec2 direction;
        float magnitude;

        float lifetime;
        bool isActive;
    };

    struct RigidBody {
        Force force;
        Vec2 vel;
        Vec2 accel;

        float invMass = 1.f;
        float invInertia = 1.f;

        float restitution{};
        float gravitymult;

        bool useGravity;
        bool useRotate;
    };

    //Types of enums supported for scripting
    enum struct MonoValueType
    {
        MONO_INT,
        MONO_BOOL,
        MONO_FLOAT,
        MONO_STRING,
        MONO_VECTOR2,
        MONO_VECTOR3,
        MONO_SCRIPT,
        MONO_UNSUPPORTED
    };


    using ScriptValue = std::variant<
        int,
        float,
        bool,
        std::string,
        Vec2,
        Vec3,
        std::vector<uint8_t>
    >;


    //Used to track what value has changed
    struct ScriptModifiedValue
    {
        std::string scriptName;
        std::string variableName;
        MonoValueType valueType;
        ScriptValue value;
    };

    struct Script {
        std::list<std::string> names;

        //Modidied data within script 
        std::multimap<std::string, ScriptModifiedValue> modifiedValuesMap;
    };

    //When updating this update the one in Engine.cs
    enum class ShapeType { AABB, Circle };
    enum class ColliderType {
        Solid,   // Blocks movement
        Trigger,  // Detects overlaps only
        Ground
    };
    struct Collider {
        ShapeType shape{ ShapeType::AABB };     // AABB or Circle
        ColliderType type{ ColliderType::Solid };   // Solid or Trigger
        bool isStatic{ true };       // true = immovable (walls), false = dynamic (player)

        // Shape data
        float width{ 0.f };   // AABB width
        float height{ 0.f };  // AABB height
        float radius{ 0.f };  // Circle radius

        // Collision state
        bool isGrounded{ false };
        bool colliding{ false };
    };

    struct Sprite {
        std::string textureName;
    };

    struct Color {
        float r, g, b, a;
    };

    struct PlayerController {
        float moveSpeed;
        float jumpSpeed;
        // maybe can add abilities etc...
    };


    //A component to hold data for states
    struct States
    {
        std::map<std::string, std::shared_ptr<StateBase>> stateMap;
        std::shared_ptr<StateBase> currState = nullptr;
        //Is this the first time running any state?
        //(Used to detect and call Enter on first state)
        bool isFirstState = true; 
    };

    // ================================================================================================
    // GRAPHICS COMPONENTS
    // ================================================================================================

    // All renderable entities will have this component
    // Each renderable will have a Mesh, Material, Shader, Texture tied to them
    struct Renderable {
        std::string meshName{ "square" };
        std::string materialName{ "ColorMat" };
        bool isWireframe{ false };
        bool wasModified{ true };
    };

    // An entity will have 1 Animator, comprising of many animations
    struct Animator {
        std::string currentAnimationClip{};
        std::string spriteSheetName{};
        int currentFrameIndex{ 0 };
        float currentTime{ 0.f };
        float playbackSpeed{ 1.f };
        bool isPlaying{ true };
        bool isLooping{ true };
        bool lastFacingRight{ false };
        // The active region of the texture to sample from
        Vec2 uvTopLeft{ 0.f, 0.f };
        Vec2 uvBottomRight{ 1.f, 1.f }; 
    };

    struct Font {
        std::string fontType;
        std::string text;
        float fontSize{ 1.f };
        Vec3 textColor{ 1.f, 1.f, 1.f };
        Vec3 offsetPosition{ 0.f, 0.f, 0.f };
    };

    // Only camera objects will have this component
    struct Camera {
        bool isMainCam{ true };
        float zoom{ 1.f };
        float width{ 1600.f };
        float height{ 900.f };

        float minX, maxX;
        float minY, maxY;

        Vec2 offset;
    };

	// Audio component
    struct Audio {
        std::string audioKey;
        float volume = 1.0f;
        bool loop = false;
        bool playOnStart = false;
        //bool is3D = false;
        bool isPlaying = false;
        //bool stop = false;
    };

    enum ButtonType {
        CLICKER,
        SLIDER,

        INVALID_BUTTON
    };
    enum ButtonAction {
        PLAYAUDIO,
        PLAYAUDIO2,
        PLAYAUDIO3,

        LOADLEVEL,
        OPENMENU,
        CLOSEMENU,
        NOACTION,
        EXIT,
    };
    struct ButtonInfo {
        ButtonAction action;
        ButtonType type;
        std::string target;
    };

    // Add more Components here if necessary...
    
}   // NightSnack

#endif // COMPONENT_HPP

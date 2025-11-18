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
namespace NightSnack {

    constexpr float FixedDT = 0.016f;

    using json = nlohmann::json;
    class StateBase; //Forward decleration

    // ================================================================================================
    // COMPONENTS
    // ================================================================================================
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
        float rot;
        Vec2 scale;

        LayerType layer;
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

        float invMass;
        float invInertia;

        float restitution;
        float gravitymult;

        bool useGravity;
        bool useRotate; 
    };

    struct Script {
        std::string name;
    };

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
        float width{ 1.0f };   // AABB width
        float height{ 1.0f };  // AABB height
        float radius{ 0.5f };  // Circle radius

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
        std::string meshName{};
        std::string materialName{};
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
    };

    // Only camera objects will have this component
    struct Camera {
        bool isMainCam{ true };
        float zoom{ 1.f };
        float width{ 1600.f };
        float height{ 900.f };
    };

    enum ButtonType {
        GAMEMENU,
        SETTINGS,
        EXIT
    };
    enum ButtonAction {
        PLAYAUDIO,
        PLAYAUDIO2,
        PLAYAUDIO3,
        PLAYSTRESSTEST,
        //ENDSTRESSTEST,
        CHANGELEVELS
    };
    struct ButtonInfo {
        ButtonAction action;
        ButtonType type;

        bool isHovered{ false };
        bool wasClicked{ false };

        //Vec2 screenPos{};
    };

    // Add more Components here if necessary...
    // Ensure to add a Json overload below for your component to allow for Archetype Loading...

    // ================================================================================================
    // COMPONENT JSON OVERLOADS
    // ================================================================================================
    // POSITION / ROTATION / SCALE / TRANSFORM ====================
    inline void from_json(const json& j, Vec3& p) {
        j.at("x").get_to(p.x);
        j.at("y").get_to(p.y);
        j.at("z").get_to(p.z);
    }

    inline void from_json(const json& j, Vec2& s) {
        j.at("x").get_to(s.x);
        j.at("y").get_to(s.y);
    }

    inline void from_json(json const& j, Transform& t) {
        auto pos = j.at("Position");
        auto scl = j.at("Scale");
        t.pos = { pos.value("x", 0.0f), pos.value("y", 0.0f), pos.value("z", 0.0f) };
        t.rot = j.value("Rotation", 0.0f);
        t.scale = { scl.value("x", 1.0f), scl.value("y", 1.0f) };

        // entity layer
        std::string l = j.value("Layer", "");
        if (l == "_Engine") { t.layer = LayerType::_ENGINE; }
        else if (l == "Background") { t.layer = LayerType::BACKGROUND; }
        else if (l == "Foreground") { t.layer = LayerType::FOREGROUND; }
        else if (l == "GameUI") { t.layer = LayerType::GAMEUI; }
        else if (l == "_Engine2") { t.layer = LayerType::_ENGINE2; }
        else {
            t.layer = LayerType::GAMELAYER;
        }
    }
    
    inline void from_json(const json& j, PlayerController& p) {
        j.at("moveSpeed").get_to(p.moveSpeed);
        j.at("jumpSpeed").get_to(p.jumpSpeed);
    }

    // PHYSICS ====================================================
    inline void from_json(const json& j, RigidBody& r) {
        float mass{};
        j.at("mass").get_to(mass);
        r.invMass = 1.f / mass;

        float inertia{ mass };
        if (j.contains("inertia")) {
            j.at("inertia").get_to(inertia);
        }
        r.invInertia = 1.f / inertia;

        r.restitution = 0.5f;
        r.force = Force{};
        r.accel = Vec2{};
        r.vel = Vec2{};
        r.useGravity = false;
        j.at("useGravity").get_to(r.useGravity);
    }

    // GRAPHICS ===================================================
    inline void from_json(const json& j, Sprite& s) {
        j.at("textureName").get_to(s.textureName);
    }

    inline void from_json(const json& j, Color& c) {
        j.at("r").get_to(c.r);
        j.at("g").get_to(c.g);
        j.at("b").get_to(c.b);
        j.at("a").get_to(c.a);
    }

    // Renderable is tricky since Mesh* and Material* are runtime resources.
    // Might instead serialize names/ids and resolve them later; Store meshes in an unorderedmap with std::string keys;
    inline void from_json(const json& j, Renderable& r) {

        j.at("meshName").get_to(r.meshName);
        j.at("materialName").get_to(r.materialName);

        if (j.contains("isWireframe"))
            j.at("isWireframe").get_to(r.isWireframe);
        else
            r.isWireframe = false;

        r.wasModified = true;

    }

    // Font =======================================================
    inline void from_json(const json& j, Font& f) {
        j.at("fontType").get_to(f.fontType);
        j.at("text").get_to(f.text);
        j.at("fontSize").get_to(f.fontSize);
        auto clr = j.at("textColor");
        f.textColor = { clr.value("r", 0.0f), clr.value("g", 0.0f), clr.value("b", 0.0f) };
    }

    // Animator ===================================================
    inline void from_json(const json& j, Animator& a) {
        j.at("spriteSheetName").get_to(a.spriteSheetName);
        j.at("playbackSpeed").get_to(a.playbackSpeed);
        j.at("isPlaying").get_to(a.isPlaying);
        j.at("isLooping").get_to(a.isLooping);
    }

    // Camera =====================================================
    inline void from_json(const json& j, Camera& c) {
        j.at("isMainCam").get_to(c.isMainCam);
        j.at("zoom").get_to(c.zoom);
        j.at("width").get_to(c.width);
        j.at("height").get_to(c.height);
    }

    inline void from_json(const json& j, ButtonInfo& b) {
        if (j["action"] == "PlayAudio") {
            b.action = ButtonAction::PLAYAUDIO;
        }

        if (j["action"] == "PlayAudio2") {
            b.action = ButtonAction::PLAYAUDIO2;
        }

        if (j["action"] == "PlayAudio3") {
            b.action = ButtonAction::PLAYAUDIO3;
        }

        if (j["action"] == "StressTest") {
            b.action = ButtonAction::PLAYSTRESSTEST;
        }

        /*if (j["action"] == "EndStressTest") {
            b.action = ButtonAction::ENDSTRESSTEST;
        }*/

        if (j["action"] == "ChangeLevel") {
            b.action = ButtonAction::CHANGELEVELS;
        }

        if (j["type"] == "GameMenu") {
            b.type = ButtonType::GAMEMENU;
        }
    }

    // COLLISION ==================================================
    inline void from_json(const json& j, Collider& c) {
        std::string shapeStr, typeStr;
        j.at("shape").get_to(shapeStr);
        j.at("type").get_to(typeStr);

        if (shapeStr == "AABB") c.shape = ShapeType::AABB;
        else if (shapeStr == "Circle") c.shape = ShapeType::Circle;

        if (typeStr == "Solid") c.type = ColliderType::Solid;
        if (typeStr == "Ground") c.type = ColliderType::Ground;
        else if (typeStr == "Trigger") c.type = ColliderType::Trigger;

        j.at("isStatic").get_to(c.isStatic);

        if (c.shape == ShapeType::AABB) {
            j.at("width").get_to(c.width);
            j.at("height").get_to(c.height);
            c.radius = 0.f;
        }
        else if (c.shape == ShapeType::Circle) {
            j.at("radius").get_to(c.radius);
            c.width = 0.f;
            c.height = 0.f;
        }

        // Optional runtime-only flags (not usually in prefab JSON)
        c.isGrounded = j.value("isGrounded", false);
        c.colliding = j.value("colliding", false);
    }
    // STATES ==================================================
    //States from_json function is within StateBase.h
    inline void from_json(const json& j, Script& s) {
        j.at("name").get_to(s.name);
    }

}   // NightSnack

#endif // COMPONENT_HPP

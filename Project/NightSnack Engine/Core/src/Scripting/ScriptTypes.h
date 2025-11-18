/******************************************************************************
 * File:        ScriptTypes.h
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan
 *
 * Description:
 * This file defines simple data structures that can cross the C++/C# boundary.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include "../ImportExport.h"
#include <cstdint>

namespace NightSnack::Scripting {

    // Entity IDs;
	using Entity = std::uint32_t;

    struct CORE_API vec2 {
        float x, y;

        vec2(float x1=0, float y1=0) 
        : x{x1}, y{y1} {}

        //NightSnack::Vec2 ToNative() {
        //    return NightSnack::Vec2(x, y);
        //}
        //static vec2 FromNative(NightSnack::Vec2 const& v) {
        //    return vec2(v.x, v.y);
        //}
    };

    struct CORE_API vec3 {
        float x, y, z;

        vec3(float x1=0, float y1=0, float z1=0) 
        : x{x1}, y{y1}, z{z1} {}

        //NightSnack::Vec3 ToNative() {
        //    return NightSnack::Vec3(x, y, z);
        //}
        //static vec3 FromNative(NightSnack::Vec3 const& v) {
        //    return vec3(v.x, v.y, v.z);
        //}
    };

    struct CORE_API Trans {
        vec3 pos{};
        float rot{};
        vec2 scale{};
        NightSnack::LayerType layer = NightSnack::LayerType::_ENGINE;
        //NightSnack::Transform ToNative() {
        //    NightSnack::Transform t;
        //    t.pos = pos.ToNative();
        //    t.rot = rot;
        //    t.scale = scale.ToNative();
        //    return t;
        //}
        //static Trans FromNative(const NightSnack::Transform& t) {
        //    Trans ret;
        //    ret.pos = vec3::FromNative(t.pos);
        //    ret.rot = t.rot;
        //    ret.scale = vec2::FromNative(t.scale);
        //    return ret;
        //}
    };

    struct CORE_API Forc {
        // Direction force
        vec2 direction{};
        float magnitude{};

        float lifetime{};
        bool isActive = false;
    };

    struct CORE_API Collide {
        NightSnack::ShapeType shape{ NightSnack::ShapeType::AABB };     // AABB or Circle
        NightSnack::ColliderType type{ NightSnack::ColliderType::Solid };   // Solid or Trigger
        bool isStatic{ true };       // true = immovable (walls), false = dynamic (player)

        // Shape data
        float width{ 1.0f };   // AABB width
        float height{ 1.0f };  // AABB height
        float radius{ 0.5f };  // Circle radius

        // Collision state
        bool isGrounded{ false };
        bool colliding{ false };
    };


    // Handle to a game object (Similar to Entity ID)
    struct CORE_API GameObjectHandle {
        uint32_t id;

        GameObjectHandle(uint32_t id = 0) : id(id) {}
        bool IsValid() const { return id != 0; }
    };

} // namespace NightSnack::Scripting

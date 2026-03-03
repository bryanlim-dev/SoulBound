/******************************************************************************
 * File:        UndoSystem.h
 * Project:     NightSnack
 * Author(s):	Wei Jie Lim (Primary, 100%)
 *
 * Description:
 * This file contains the definition of UndoSystem which handles the
 * undo functionality for the editor.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include <variant>
#include <stack>
#include <deque>
#include <optional>
#include "Core/ECS/Coordinator.h"
#include "Core/ECS/Component.h"

namespace NightSnack {

	// Snapshots for different undoable actions
    struct TransformSnapshot
    {
		// Required data to restore
        Entity entity;
        Transform transform;
    };

    struct PhysicsSettingsSnapshot
    {
        float gravity;
        float drag;
        float friction;
	};

    struct PlayerControllerSnapshot
    {
        Entity entity;
        PlayerController playercontroller;
	};

    struct CameraSnapshot
    {
        Entity entity;
        Camera camera;
	};

    struct RigidBodySnapshot
    {
        Entity entity;
        RigidBody rigidbody;
    };

    struct ColliderSnapshot
    {
        Entity entity;
        Collider collider;
    };

    struct RenderableSnapshot
    {
        Entity entity;
        Renderable renderable;
    };

    struct FontSnapshot
    {
        Entity entity;
        Font font;
	};

    struct ComponentAddSnapshot
    {
        Entity entity;
        std::string componentName;
    };

    struct ComponentRemoveSnapshot
    {
        Entity entity;
        std::string componentName;
		std::optional<PlayerController> playercontroller;
        std::optional<RigidBody> rigidbody;
        std::optional<Collider> collider;
        std::optional<Renderable> renderable;
		std::optional<Script> script;
		std::optional<Animator> animator;
		std::optional<Font> font;
		std::optional<Character> character;
		std::optional<Collectible> collectible;
    };

    struct EntityCreateSnapshot
    {
        Entity entity;
    };

    struct EntityDeleteSnapshot
    {
        Entity entity;
        std::string prefabName;
        Transform transform;
    };

    using UndoAction = std::variant<
        TransformSnapshot,
        EntityCreateSnapshot,
        EntityDeleteSnapshot,
        PhysicsSettingsSnapshot,
		PlayerControllerSnapshot,
        CameraSnapshot,
        RigidBodySnapshot,
        ColliderSnapshot,
        RenderableSnapshot,
		FontSnapshot,
        ComponentAddSnapshot,
        ComponentRemoveSnapshot
    >;

    class UndoSystem {
    public:
		// Instance access
        static UndoSystem& Instance()
        {
            static UndoSystem instance;
            return instance;
        }

		// Push new action onto the undo stack
        void Push(const UndoAction& action)
        {
            if (undoStack.size() >= maxSteps) undoStack.pop_front();
            undoStack.push_back(action);
        }

		// Pop action from the undo stack
        std::optional<UndoAction> Pop()
        {
            if (undoStack.empty()) { return std::nullopt; }
            auto action = undoStack.back();
            undoStack.pop_back();
            return action;
        }

		// Clear the undo stack
        void Clear() { undoStack.clear(); }

		// Perform the undo operation
        void PerformUndo(Coordinator& coord, EntityFactory& factory)
        {
			// Get the last action, return if none
            auto action = Pop();
            if (!action) { return; }

            std::visit([&](auto&& act) {
                using T = std::decay_t<decltype(act)>;
                if constexpr (std::is_same_v<T, TransformSnapshot>)
                {
                    if (coord.HasComponent<Transform>(act.entity))
                        coord.ModifyComponent<Transform>(act.entity) = act.transform;
                }
                else if constexpr (std::is_same_v<T, PlayerControllerSnapshot>)
                {
                    if (coord.HasComponent<PlayerController>(act.entity))
                        coord.ModifyComponent<PlayerController>(act.entity) = act.playercontroller;
				}
                else if constexpr (std::is_same_v<T, CameraSnapshot>)
                {
                    if (coord.HasComponent<Camera>(act.entity))
                        coord.ModifyComponent<Camera>(act.entity) = act.camera;
                }
                else if constexpr (std::is_same_v<T, RigidBodySnapshot>)
                {
                    if (coord.HasComponent<RigidBody>(act.entity))
                        coord.ModifyComponent<RigidBody>(act.entity) = act.rigidbody;
                }
                else if constexpr (std::is_same_v<T, ColliderSnapshot>)
                {
                    if (coord.HasComponent<Collider>(act.entity))
                        coord.ModifyComponent<Collider>(act.entity) = act.collider;
                }
                else if constexpr (std::is_same_v<T, RenderableSnapshot>)
                {
                    if (coord.HasComponent<Renderable>(act.entity))
                        coord.ModifyComponent<Renderable>(act.entity) = act.renderable;
                }
                else if constexpr (std::is_same_v<T, FontSnapshot>)
                {
                    if (coord.HasComponent<Font>(act.entity))
                        coord.ModifyComponent<Font>(act.entity) = act.font;
				}
                else if constexpr (std::is_same_v<T, ComponentAddSnapshot>)
                {
					if (act.componentName == "PlayerController") coord.RemoveComponent<PlayerController>(act.entity);
                    else if (act.componentName == "RigidBody")   coord.RemoveComponent<RigidBody>(act.entity);
                    else if (act.componentName == "Collider")    coord.RemoveComponent<Collider>(act.entity);
                    else if (act.componentName == "Renderable")  coord.RemoveComponent<Renderable>(act.entity);
					else if (act.componentName == "Scripting")   coord.RemoveComponent<Script>(act.entity);
					else if (act.componentName == "Animator")    coord.RemoveComponent<Animator>(act.entity);
					else if (act.componentName == "Font")        coord.RemoveComponent<Font>(act.entity);
                }
                else if constexpr (std::is_same_v<T, ComponentRemoveSnapshot>)
                {
                    if (act.componentName == "PlayerController" && act.playercontroller)
						coord.AddComponent<PlayerController>(act.entity, *act.playercontroller);
                    else if (act.componentName == "RigidBody" && act.rigidbody)
                        coord.AddComponent<RigidBody>(act.entity, *act.rigidbody);
                    else if (act.componentName == "Collider" && act.collider)
                        coord.AddComponent<Collider>(act.entity, *act.collider);
                    else if (act.componentName == "Renderable" && act.renderable)
                        coord.AddComponent<Renderable>(act.entity, *act.renderable);
					else if (act.componentName == "Scripting" && act.script)
						coord.AddComponent<Script>(act.entity, *act.script);
					else if (act.componentName == "Animator" && act.animator)
						coord.AddComponent<Animator>(act.entity, *act.animator);
					else if (act.componentName == "Font" && act.font)
						coord.AddComponent<Font>(act.entity, *act.font);
                }
                else if constexpr (std::is_same_v<T, PhysicsSettingsSnapshot>)
                {
                    auto ps = coord.GetSystem<PhysicsSystem>();
                    if (ps) {
                        ps->SetGravity(act.gravity);
                        ps->SetDrag(act.drag);
                        ps->SetFriction(act.friction);
                    }
                }
                else if constexpr (std::is_same_v<T, EntityCreateSnapshot>)
                {
					factory.RemoveEntity(act.entity);
                }
                else if constexpr (std::is_same_v<T, EntityDeleteSnapshot>)
                {
                    RemapEntityId(act.entity, factory.Create(act.prefabName, act.transform));
                }
                }, *action);
        }

        void RemapEntityId(Entity oldId, Entity newId)
        {
            for (auto& action : undoStack)
            {
                std::visit([&](auto&& act) {
                    using T = std::decay_t<decltype(act)>;
                    if constexpr (std::is_same_v<T, TransformSnapshot>)
                    {
                        if (act.entity == oldId) { act.entity = newId; }
                    }
                    else if constexpr (std::is_same_v<T, EntityCreateSnapshot>)
                    {
                        if (act.entity == oldId) { act.entity = newId; }
                    }
                    else if constexpr (std::is_same_v<T, EntityDeleteSnapshot>)
                    {
                        if (act.entity == oldId) { act.entity = newId; }
                    }
                    }, action);
            }
        }

    private:
        static constexpr size_t maxSteps = 30;
        std::deque<UndoAction> undoStack;

        UndoSystem() = default;
        
        UndoSystem(const UndoSystem&) = delete;
        
        UndoSystem& operator=(const UndoSystem&) = delete;
    };

}	// NightSnack

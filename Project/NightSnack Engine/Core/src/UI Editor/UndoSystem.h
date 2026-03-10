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

    struct ButtonSnapshot
    {
        Entity entity;
        ButtonInfo button;
	};

    struct AudioSnapshot
    {
        Entity entity;
        Audio audio;
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
		std::optional<ButtonInfo> button;
		std::optional<Audio> audio;
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
		ButtonSnapshot,
		AudioSnapshot,
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
            redoStack.clear();
        }

		// Pop action from the undo stack
        std::optional<UndoAction> Pop()
        {
            if (undoStack.empty()) { return std::nullopt; }
            auto action = undoStack.back();
            undoStack.pop_back();
            if (redoStack.size() >= maxSteps) redoStack.pop_front();
            redoStack.push_back(action);
            return action;
        }

		// Clear the undo stack
        void Clear() { undoStack.clear(); }

        // Clear the redo stack
		void ClearRedo() { redoStack.clear(); }

		// Perform the undo operation
        void PerformUndo(Coordinator& coord, EntityFactory& factory)
        {
			// Get the last action, return if none
            if (undoStack.empty()) return;
            auto action = undoStack.back();
            undoStack.pop_back();

            std::visit([&](auto&& act) {
                using T = std::decay_t<decltype(act)>;
                if constexpr (std::is_same_v<T, TransformSnapshot>)
                {
                    if (coord.HasComponent<Transform>(act.entity))
                    {
                        // Capture current state for redo
                        TransformSnapshot redoSnap{ act.entity, coord.GetComponent<Transform>(act.entity) };
                        if (redoStack.size() >= maxSteps) redoStack.pop_front();
                        redoStack.push_back(redoSnap);

                        // Apply undo snapshot
                        coord.ModifyComponent<Transform>(act.entity) = act.transform;
                    }
                }
                else if constexpr (std::is_same_v<T, PlayerControllerSnapshot>)
                {
                    if (coord.HasComponent<PlayerController>(act.entity))
                    {
                        PlayerControllerSnapshot redoSnap{ act.entity, coord.GetComponent<PlayerController>(act.entity) };
                        if (redoStack.size() >= maxSteps) redoStack.pop_front();
                        redoStack.push_back(redoSnap);

                        coord.ModifyComponent<PlayerController>(act.entity) = act.playercontroller;
                    }
				}
                else if constexpr (std::is_same_v<T, CameraSnapshot>)
                {
                    if (coord.HasComponent<Camera>(act.entity))
                    {
                        CameraSnapshot redoSnap{ act.entity, coord.GetComponent<Camera>(act.entity) };
                        if (redoStack.size() >= maxSteps) redoStack.pop_front();
                        redoStack.push_back(redoSnap);

                        coord.ModifyComponent<Camera>(act.entity) = act.camera;
                    }
                }
                else if constexpr (std::is_same_v<T, RigidBodySnapshot>)
                {
                    if (coord.HasComponent<RigidBody>(act.entity))
                    {
                        RigidBodySnapshot redoSnap{ act.entity, coord.GetComponent<RigidBody>(act.entity) };
                        if (redoStack.size() >= maxSteps) redoStack.pop_front();
						redoStack.push_back(redoSnap);

						coord.ModifyComponent<RigidBody>(act.entity) = act.rigidbody;
                    }
                }
                else if constexpr (std::is_same_v<T, ColliderSnapshot>)
                {
                    if (coord.HasComponent<Collider>(act.entity))
                    {
                        ColliderSnapshot redoSnap{ act.entity, coord.GetComponent<Collider>(act.entity) };
                        if (redoStack.size() >= maxSteps) redoStack.pop_front();
						redoStack.push_back(redoSnap);

						coord.ModifyComponent<Collider>(act.entity) = act.collider;
                    }
                }
                else if constexpr (std::is_same_v<T, RenderableSnapshot>)
                {
                    if (coord.HasComponent<Renderable>(act.entity))
                    {
                        RenderableSnapshot redoSnap{ act.entity, coord.GetComponent<Renderable>(act.entity) };
						if (redoStack.size() >= maxSteps) redoStack.pop_front();
						redoStack.push_back(redoSnap);

						coord.ModifyComponent<Renderable>(act.entity) = act.renderable;
                    }
                }
                else if constexpr (std::is_same_v<T, FontSnapshot>)
                {
                    if (coord.HasComponent<Font>(act.entity))
                    {
                        FontSnapshot redoSnap{ act.entity, coord.GetComponent<Font>(act.entity) };
						if (redoStack.size() >= maxSteps) redoStack.pop_front();
						redoStack.push_back(redoSnap);

						coord.ModifyComponent<Font>(act.entity) = act.font;
                    }
                }
                else if constexpr (std::is_same_v<T, ButtonSnapshot>)
                {
                    if (coord.HasComponent<ButtonInfo>(act.entity))
                    {
						ButtonSnapshot redoSnap{ act.entity, coord.GetComponent<ButtonInfo>(act.entity) };
						if (redoStack.size() >= maxSteps) redoStack.pop_front();
						redoStack.push_back(redoSnap);

						coord.ModifyComponent<ButtonInfo>(act.entity) = act.button;
                    }
                }
                else if constexpr (std::is_same_v<T, AudioSnapshot>)
                {
                    if (coord.HasComponent<Audio>(act.entity))
                    {
						AudioSnapshot redoSnap{ act.entity, coord.GetComponent<Audio>(act.entity) };
						if (redoStack.size() >= maxSteps) redoStack.pop_front();
						redoStack.push_back(redoSnap);

						coord.ModifyComponent<Audio>(act.entity) = act.audio;
                    }
                }
                else if constexpr (std::is_same_v<T, ComponentAddSnapshot>)
                {
                    if (act.componentName == "PlayerController" && coord.HasComponent<PlayerController>(act.entity))
                    {
                        PlayerControllerSnapshot redoSnap{ act.entity, coord.GetComponent<PlayerController>(act.entity) };
                        if (redoStack.size() >= maxSteps) redoStack.pop_front();
                        redoStack.push_back(redoSnap);
                        coord.RemoveComponent<PlayerController>(act.entity);
                    }
                    else if (act.componentName == "RigidBody" && coord.HasComponent<RigidBody>(act.entity))
                    {
                        RigidBodySnapshot redoSnap{ act.entity, coord.GetComponent<RigidBody>(act.entity) };
                        if (redoStack.size() >= maxSteps) redoStack.pop_front();
                        redoStack.push_back(redoSnap);
                        coord.RemoveComponent<RigidBody>(act.entity);
                    }
                    else if (act.componentName == "Collider")
                    {
						ColliderSnapshot redoSnap{ act.entity, coord.GetComponent<Collider>(act.entity) };
						if (redoStack.size() >= maxSteps) redoStack.pop_front();
                        redoStack.push_back(redoSnap);

						coord.RemoveComponent<Collider>(act.entity);
                    }
                    else if (act.componentName == "Renderable")
                    {
						RenderableSnapshot redoSnap{ act.entity, coord.GetComponent<Renderable>(act.entity) };
						if (redoStack.size() >= maxSteps) redoStack.pop_front();
						redoStack.push_back(redoSnap);

                        coord.RemoveComponent<Renderable>(act.entity);
                    }
					else if (act.componentName == "Scripting")
					{
                        // Doing later
						coord.RemoveComponent<Script>(act.entity);
					}
					else if (act.componentName == "Animator")
					{
						// Doing later
						coord.RemoveComponent<Animator>(act.entity);
					}
					else if (act.componentName == "Font")
					{
						FontSnapshot redoSnap{ act.entity, coord.GetComponent<Font>(act.entity) };
						if (redoStack.size() >= maxSteps) redoStack.pop_front();
						redoStack.push_back(redoSnap);

						coord.RemoveComponent<Font>(act.entity);
					}
                    else if (act.componentName == "ButtonInfo")
					{   
						ButtonSnapshot redoSnap{ act.entity, coord.GetComponent<ButtonInfo>(act.entity) };
						if (redoStack.size() >= maxSteps) redoStack.pop_front();
						redoStack.push_back(redoSnap);

						coord.RemoveComponent<ButtonInfo>(act.entity);
					}
					else if (act.componentName == "Audio")
					{   
						AudioSnapshot redoSnap{ act.entity, coord.GetComponent<Audio>(act.entity) };
						if (redoStack.size() >= maxSteps) redoStack.pop_front();
						redoStack.push_back(redoSnap);

						coord.RemoveComponent<Audio>(act.entity);
					}
                }
                else if constexpr (std::is_same_v<T, ComponentRemoveSnapshot>)
                {
                    if (act.componentName == "PlayerController" && act.playercontroller)
                    {
                        if (coord.HasComponent<PlayerController>(act.entity))
                        {
                            PlayerControllerSnapshot redoSnap{ act.entity, coord.GetComponent<PlayerController>(act.entity) };
                            if (redoStack.size() >= maxSteps) redoStack.pop_front();
                            redoStack.push_back(redoSnap);
                        }
                        coord.AddComponent<PlayerController>(act.entity, *act.playercontroller);
                    }
                    else if (act.componentName == "RigidBody" && act.rigidbody)
                    {
                        if (coord.HasComponent<RigidBody>(act.entity))
                        {
                            RigidBodySnapshot redoSnap{ act.entity, coord.GetComponent<RigidBody>(act.entity) };
                            if (redoStack.size() >= maxSteps) redoStack.pop_front();
                            redoStack.push_back(redoSnap);
                        }
                        coord.AddComponent<RigidBody>(act.entity, *act.rigidbody);
                    }
                    else if (act.componentName == "Collider" && act.collider)
                    {
                        if (coord.HasComponent<Collider>(act.entity))
                        {
                            ColliderSnapshot redoSnap{ act.entity, coord.GetComponent<Collider>(act.entity) };
                            if (redoStack.size() >= maxSteps) redoStack.pop_front();
                            redoStack.push_back(redoSnap);
                        }
						coord.AddComponent<Collider>(act.entity, *act.collider);
                    }
                    else if (act.componentName == "Renderable" && act.renderable)
                    {
                        if (coord.HasComponent<Renderable>(act.entity))
                        {
                            RenderableSnapshot redoSnap{ act.entity, coord.GetComponent<Renderable>(act.entity) };
                            if (redoStack.size() >= maxSteps) redoStack.pop_front();
                            redoStack.push_back(redoSnap);
                        }
						coord.AddComponent<Renderable>(act.entity, *act.renderable);
                    }
                    else if (act.componentName == "Scripting" && act.script)
                    {
                        // Later
                        coord.AddComponent<Script>(act.entity, *act.script);
                    }
                    else if (act.componentName == "Animator" && act.animator)
                    {
                        // Later
                        coord.AddComponent<Animator>(act.entity, *act.animator);
                    }
                    else if (act.componentName == "Font" && act.font)
                    {
                        if (coord.HasComponent<Font>(act.entity))
                        {
                            FontSnapshot redoSnap{ act.entity, coord.GetComponent<Font>(act.entity) };
                            if (redoStack.size() >= maxSteps) redoStack.pop_front();
                            redoStack.push_back(redoSnap);
                        }
                        coord.AddComponent<Font>(act.entity, *act.font);
                    }
                    else if (act.componentName == "ButtonInfo" && act.button)
                    {
                        if (coord.HasComponent<ButtonInfo>(act.entity))
                        {
                            ButtonSnapshot redoSnap{ act.entity, coord.GetComponent<ButtonInfo>(act.entity) };
                            if (redoStack.size() >= maxSteps) redoStack.pop_front();
                            redoStack.push_back(redoSnap);
                        }
                        coord.AddComponent<ButtonInfo>(act.entity, *act.button);
                    }
                    else if (act.componentName == "Audio" && act.audio)
                    {
                        if (coord.HasComponent<Audio>(act.entity))
                        {
                            AudioSnapshot redoSnap{ act.entity, coord.GetComponent<Audio>(act.entity) };
                            if (redoStack.size() >= maxSteps) redoStack.pop_front();
                            redoStack.push_back(redoSnap);
                        }
                        coord.AddComponent<Audio>(act.entity, *act.audio);
                    }
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
                }, action);

                if (redoStack.size() >= maxSteps) redoStack.pop_front();
                redoStack.push_back(action);
        }

        // Redo operation
        void PerformRedo(Coordinator& coord, EntityFactory& factory)
        {
            (void)factory;
            if (redoStack.empty()) return;
            auto action = redoStack.back();
            redoStack.pop_back();

            // Redo the action
            std::visit([&](auto&& act)
            {
                using T = std::decay_t<decltype(act)>;

                if constexpr (std::is_same_v<T, TransformSnapshot>)
                {
                    if (coord.HasComponent<Transform>(act.entity))
                    {
                        // Capture current state for undo
                        TransformSnapshot undoSnap{ act.entity, coord.GetComponent<Transform>(act.entity) };
                        if (undoStack.size() >= maxSteps) undoStack.pop_front();
                        undoStack.push_back(undoSnap);

                        // Apply redo snapshot
                        coord.ModifyComponent<Transform>(act.entity) = act.transform;
                    }
                }
                if constexpr (std::is_same_v<T, PlayerControllerSnapshot>)
                {
                    if (coord.HasComponent<PlayerController>(act.entity))
                    {
                        PlayerControllerSnapshot undoSnap{ act.entity, coord.GetComponent<PlayerController>(act.entity) };
                        if (undoStack.size() >= maxSteps) undoStack.pop_front();
                        undoStack.push_back(undoSnap);

						coord.ModifyComponent<PlayerController>(act.entity) = act.playercontroller;
                    }
                }
                else if constexpr (std::is_same_v<T, CameraSnapshot>)
                {
                    if (coord.HasComponent<Camera>(act.entity))
                    {
                        CameraSnapshot undoSnap{ act.entity, coord.GetComponent<Camera>(act.entity) };
                        if (undoStack.size() >= maxSteps) undoStack.pop_front();
						undoStack.push_back(undoSnap);

						coord.ModifyComponent<Camera>(act.entity) = act.camera;
                    }
                }
                else if constexpr (std::is_same_v<T, RigidBodySnapshot>)
                {
                    if (coord.HasComponent<RigidBody>(act.entity))
                    {
                        RigidBodySnapshot undoSnap{ act.entity, coord.GetComponent<RigidBody>(act.entity) };
						if (undoStack.size() >= maxSteps) undoStack.pop_front();
						undoStack.push_back(undoSnap);

						coord.ModifyComponent<RigidBody>(act.entity) = act.rigidbody;
                    }
                }
                else if constexpr (std::is_same_v<T, ColliderSnapshot>)
                {
                    if (coord.HasComponent<Collider>(act.entity))
                    {
						ColliderSnapshot undoSnap{ act.entity, coord.GetComponent<Collider>(act.entity) };
						if (undoStack.size() >= maxSteps) undoStack.pop_front();
						undoStack.push_back(undoSnap);

						coord.ModifyComponent<Collider>(act.entity) = act.collider;
                    }
                }
                else if constexpr (std::is_same_v<T, RenderableSnapshot>)
                {
                    if (coord.HasComponent<Renderable>(act.entity))
                    {
						RenderableSnapshot undoSnap{ act.entity, coord.GetComponent<Renderable>(act.entity) };
						if (undoStack.size() >= maxSteps) undoStack.pop_front();
						undoStack.push_back(undoSnap);

						coord.ModifyComponent<Renderable>(act.entity) = act.renderable;
                    }
                }
                else if constexpr (std::is_same_v<T, FontSnapshot>)
                {
                    if (coord.HasComponent<Font>(act.entity))
                    {
						FontSnapshot undoSnap{ act.entity, coord.GetComponent<Font>(act.entity) };
						if (undoStack.size() >= maxSteps) undoStack.pop_front();
						undoStack.push_back(undoSnap);

						coord.ModifyComponent<Font>(act.entity) = act.font;
                    }
                }
                else if constexpr (std::is_same_v<T, ButtonSnapshot>)
                {
                    if (coord.HasComponent<ButtonInfo>(act.entity))
                    {
						ButtonSnapshot undoSnap{ act.entity, coord.GetComponent<ButtonInfo>(act.entity) };
						if (undoStack.size() >= maxSteps) undoStack.pop_front();
						undoStack.push_back(undoSnap);

						coord.ModifyComponent<ButtonInfo>(act.entity) = act.button;
                    }
                }
                else if constexpr (std::is_same_v<T, AudioSnapshot>)
                {
                    if (coord.HasComponent<Audio>(act.entity))
                    {
						AudioSnapshot undoSnap{ act.entity, coord.GetComponent<Audio>(act.entity) };
						if (undoStack.size() >= maxSteps) undoStack.pop_front();
						undoStack.push_back(undoSnap);

						coord.ModifyComponent<Audio>(act.entity) = act.audio;
                    }
                }
                else if constexpr (std::is_same_v<T, ComponentRemoveSnapshot>)
                {
                    if (act.componentName == "PlayerController" && coord.HasComponent<PlayerController>(act.entity))
                    {
                        PlayerControllerSnapshot undoSnap{ act.entity, coord.GetComponent<PlayerController>(act.entity) };
                        if (undoStack.size() >= maxSteps) undoStack.pop_front();
                        undoStack.push_back(undoSnap);

                        coord.RemoveComponent<PlayerController>(act.entity);
                    }
                    else if (act.componentName == "RigidBody" && coord.HasComponent<RigidBody>(act.entity))
                    {
                        RigidBodySnapshot undoSnap{ act.entity, coord.GetComponent<RigidBody>(act.entity) };
                        if (undoStack.size() >= maxSteps) undoStack.pop_front();
                        undoStack.push_back(undoSnap);

                        coord.RemoveComponent<RigidBody>(act.entity);
                    }
                    else if (act.componentName == "Collider" && coord.HasComponent<Collider>(act.entity))
					{
						ColliderSnapshot undoSnap{ act.entity, coord.GetComponent<Collider>(act.entity) };
						if (undoStack.size() >= maxSteps) undoStack.pop_front();
						undoStack.push_back(undoSnap);

                        coord.RemoveComponent<Collider>(act.entity);
                    }
                    else if (act.componentName == "Renderable" && coord.HasComponent<Renderable>(act.entity))
                    {
                        RenderableSnapshot undoSnap{ act.entity, coord.GetComponent<Renderable>(act.entity) };
                        if (undoStack.size() >= maxSteps) undoStack.pop_front();
                        undoStack.push_back(undoSnap);

                        coord.RemoveComponent<Renderable>(act.entity);
                    }
                    else if (act.componentName == "Scripting" && coord.HasComponent<Script>(act.entity))
                    {
                        coord.RemoveComponent<Script>(act.entity);
                    }
                    else if (act.componentName == "Animator" && coord.HasComponent<Animator>(act.entity))
                    {
                        coord.RemoveComponent<Animator>(act.entity);
                    }
                    else if (act.componentName == "Font" && coord.HasComponent<Font>(act.entity))
                    {
                        FontSnapshot undoSnap{ act.entity, coord.GetComponent<Font>(act.entity) };
                        if (undoStack.size() >= maxSteps) undoStack.pop_front();
                        undoStack.push_back(undoSnap);

                        coord.RemoveComponent<Font>(act.entity);
                    }
                    else if (act.componentName == "ButtonInfo" && coord.HasComponent<ButtonInfo>(act.entity))
                    {
                        ButtonSnapshot undoSnap{ act.entity, coord.GetComponent<ButtonInfo>(act.entity) };
                        if (undoStack.size() >= maxSteps) undoStack.pop_front();
                        undoStack.push_back(undoSnap);

                        coord.RemoveComponent<ButtonInfo>(act.entity);
					}
                    else if (act.componentName == "Audio" && coord.HasComponent<Audio>(act.entity))
                    {
                        AudioSnapshot undoSnap{ act.entity, coord.GetComponent<Audio>(act.entity) };
                        if (undoStack.size() >= maxSteps) undoStack.pop_front();
                        undoStack.push_back(undoSnap);

                        coord.RemoveComponent<Audio>(act.entity);
                    }
                }
            }, action);

            // Pushed to undoStack so it can be undone again
            if (undoStack.size() >= maxSteps) undoStack.pop_front();
            undoStack.push_back(action);
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
        std::deque<UndoAction> redoStack;

        UndoSystem() = default;
        
        UndoSystem(const UndoSystem&) = delete;
        
        UndoSystem& operator=(const UndoSystem&) = delete;
    };

}	// NightSnack

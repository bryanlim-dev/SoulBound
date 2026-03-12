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
#include "GameLogic/SceneManager.h"

namespace NightSnack {

    // Generic snapshot for any component type
    template<typename TComp>
    struct ComponentSnapshot
    {
        Entity  entity;
        TComp   component;
    };

    // Component aliases
    using TransformSnapshot         = ComponentSnapshot<Transform>;
    using PlayerControllerSnapshot  = ComponentSnapshot<PlayerController>;
    using CameraSnapshot            = ComponentSnapshot<Camera>;
    using RigidBodySnapshot         = ComponentSnapshot<RigidBody>;
    using ColliderSnapshot          = ComponentSnapshot<Collider>;
    using RenderableSnapshot        = ComponentSnapshot<Renderable>;
    using FontSnapshot              = ComponentSnapshot<Font>;
    using ButtonSnapshot            = ComponentSnapshot<ButtonInfo>;
    using AudioSnapshot             = ComponentSnapshot<Audio>;

    struct PhysicsSettingsSnapshot
    {
        float gravity;
        float drag;
        float friction;
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
        std::optional<RigidBody>        rigidbody;
        std::optional<Collider>         collider;
        std::optional<Renderable>       renderer;
		std::optional<Script>           script;
		std::optional<Animator>         animator;
		std::optional<Font>             font;
		std::optional<ButtonInfo>       button;
		std::optional<Audio>            audio;
		std::optional<Character>        character;
		std::optional<Collectible>      collectible;
    };

    struct EntityCreateSnapshot
    {
        Entity entity;
    };

    struct EntityDeleteSnapshot
    {
        Entity      entity;
        std::string prefabName;
        Transform   transform;
        std::optional<Collider> collider;
    };

    struct CamTriggerCreateSnapshot
    {
        Entity entity;
    };

    struct CamTriggerDeleteSnapshot
    {
        Entity    entity;
        Vec3      pos;
        Vec2      scale;
        json      bounds;
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
        ComponentRemoveSnapshot,
        CamTriggerCreateSnapshot,
        CamTriggerDeleteSnapshot
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

                if constexpr (IsComponentSnapshot<T>::value)
                {
                    using Comp = typename IsComponentSnapshot<T>::ComponentType;
                    if (coord.HasComponent<Comp>(act.entity))
                    {
                        PushToStack(redoStack, T{ act.entity, coord.GetComponent<Comp>(act.entity) });
                        coord.ModifyComponent<Comp>(act.entity) = act.component;
                    }
                }
                else if constexpr (std::is_same_v<T, ComponentAddSnapshot>)
                {
                    UndoComponentAdd(coord, act.entity, act.componentName, redoStack);
                }
                else if constexpr (std::is_same_v<T, ComponentRemoveSnapshot>)
                {
                    UndoComponentRemove(coord, act, redoStack);
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
                    // Capture prefab for redo
                    std::string prefabName = factory.GetEntityPrefabName(act.entity);
                    Transform   transform{};
                    if (coord.HasComponent<Transform>(act.entity))
                        transform = coord.GetComponent<Transform>(act.entity);

                    std::optional<Collider> snapCollider;
                    if (coord.HasComponent<Collider>(act.entity))
                        snapCollider = coord.GetComponent<Collider>(act.entity);
                    
                    factory.RemoveEntity(act.entity);

					// Redo stack for entity deletion
                    PushToStack(redoStack, EntityDeleteSnapshot{ act.entity, prefabName, transform, snapCollider });
                }
                else if constexpr (std::is_same_v<T, EntityDeleteSnapshot>)
                {
                    // Create entity and capture new ID
                    Entity newEntity = act.collider
                        ? factory.CreateCollider(*act.collider, act.transform)
                        : factory.Create(act.prefabName, act.transform);
                    
                    RemapEntityId(act.entity, newEntity);

					// Redo stack for entity creation
                    PushToStack(redoStack, EntityCreateSnapshot{ newEntity });
                }
                else if constexpr (std::is_same_v<T, CamTriggerCreateSnapshot>)
                {
                    // Undo a CamTrigger Create
                    auto sceneManager = coord.GetSystem<NightSnack::Scene::SceneManager>();
                    if (!sceneManager) return;

                    Vec3 pos{};
                    Vec2 scale{};
                    json bounds{};

                    if (coord.HasComponent<Transform>(act.entity))
                    {
                        auto& t = coord.GetComponent<Transform>(act.entity);
                        pos = t.pos;
                        scale = Vec2{ t.scale.x, t.scale.y };
                    }
                    for (auto& [e, b] : sceneManager->GetCameraKeyframes())
                    {
                        if (e == act.entity) { bounds = b; break; }
                    }
                        
                    sceneManager->DeleteCamTrigger(act.entity);
                    PushToStack(redoStack, CamTriggerDeleteSnapshot{ act.entity, pos, scale, bounds });
                }
                else if constexpr (std::is_same_v<T, CamTriggerDeleteSnapshot>)
                {
                    // Undo a CamTrigger Delete
                    auto sceneManager = coord.GetSystem<NightSnack::Scene::SceneManager>();
                    if (!sceneManager) return;

                    sceneManager->CreateCamTrigger(act.pos, act.scale, act.bounds);
                    auto keyframes = sceneManager->GetCameraKeyframes();

                    if (!keyframes.empty())
                    {
                        Entity newEntity = keyframes.back().first;
                        RemapEntityId(act.entity, newEntity);
                        PushToStack(redoStack, CamTriggerCreateSnapshot{ newEntity });
                    }
                }
            }, action);
        }

        // Redo operation
        void PerformRedo(Coordinator& coord, EntityFactory& factory)
        {
            (void)factory;
            if (redoStack.empty()) return;
            auto action = redoStack.back();
            redoStack.pop_back();

            // Redo the action
            std::visit([&](auto&& act) {
                using T = std::decay_t<decltype(act)>;

                if constexpr (IsComponentSnapshot<T>::value)
                {
                    using Comp = typename IsComponentSnapshot<T>::ComponentType;
                    if (coord.HasComponent<Comp>(act.entity))
                    {
                        PushToStack(undoStack, T{ act.entity, coord.GetComponent<Comp>(act.entity) });
                        coord.ModifyComponent<Comp>(act.entity) = act.component;
                    }
                }
                else if constexpr (std::is_same_v<T, ComponentAddSnapshot>)
                {
                    UndoComponentAdd(coord, act.entity, act.componentName, undoStack);
                }
                else if constexpr (std::is_same_v<T, ComponentRemoveSnapshot>)
                {
                    UndoComponentRemove(coord, act, undoStack);
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
					// Capture prefab for undo
                    std::string prefabName = factory.GetEntityPrefabName(act.entity);
                    Transform   transform{};
                    if (coord.HasComponent<Transform>(act.entity))
                        transform = coord.GetComponent<Transform>(act.entity);

                    std::optional<Collider> snapCollider;
                    if (coord.HasComponent<Collider>(act.entity))
                        snapCollider = coord.GetComponent<Collider>(act.entity);

                    factory.RemoveEntity(act.entity);

					// Undo stack for entity deletion
                    PushToStack(undoStack, EntityDeleteSnapshot{ act.entity, prefabName, transform, snapCollider });
                }
                else if constexpr (std::is_same_v<T, EntityDeleteSnapshot>)
                {
					// Create entity and capture new ID
                    Entity newEntity = act.collider
                        ? factory.CreateCollider(*act.collider, act.transform)
                        : factory.Create(act.prefabName, act.transform);

                    RemapEntityId(act.entity, newEntity);

					// Undo stack for entity creation
                    PushToStack(undoStack, EntityCreateSnapshot{ newEntity });
                }
                else if constexpr (std::is_same_v<T, CamTriggerCreateSnapshot>)
                {
                    // Undo a CamTrigger Create
                    auto sceneManager = coord.GetSystem<NightSnack::Scene::SceneManager>();
                    if (!sceneManager) return;

                    Vec3 pos{};
                    Vec2 scale{};
                    json bounds{};

                    if (coord.HasComponent<Transform>(act.entity))
                    {
                        auto& t = coord.GetComponent<Transform>(act.entity);
                        pos = t.pos;
                        scale = Vec2{ t.scale.x, t.scale.y };
                    }
                    for (auto& [e, b] : sceneManager->GetCameraKeyframes())
                    {
                        if (e == act.entity) { bounds = b; break; }
                    }
                    
                    sceneManager->DeleteCamTrigger(act.entity);
                    PushToStack(undoStack, CamTriggerDeleteSnapshot{ act.entity, pos, scale, bounds });
                }
                else if constexpr (std::is_same_v<T, CamTriggerDeleteSnapshot>)
                {
                    // Undo a CamTrigger Delete
                    auto sceneManager = coord.GetSystem<NightSnack::Scene::SceneManager>();
                    if (!sceneManager) return;

                    sceneManager->CreateCamTrigger(act.pos, act.scale, act.bounds);
                    auto keyframes = sceneManager->GetCameraKeyframes();

                    if (!keyframes.empty())
                    {
                        Entity newEntity = keyframes.back().first;
                        RemapEntityId(act.entity, newEntity);
                        PushToStack(undoStack, CamTriggerCreateSnapshot{ newEntity });
                    }
                }
            }, action);
        }

        void RemapEntityId(Entity oldId, Entity newId)
        {
            for (auto& action : undoStack)
            {
                std::visit([&](auto&& act) {
                    using T = std::decay_t<decltype(act)>;
                    if constexpr (std::is_same_v<T, TransformSnapshot> ||
                        std::is_same_v<T, EntityCreateSnapshot>        ||
                        std::is_same_v<T, EntityDeleteSnapshot>)
                    {
                        if (act.entity == oldId) act.entity = newId;
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

		// Helper trait to identify component snapshots
        template<typename T>
        struct IsComponentSnapshot : std::false_type {};

        template<typename TComp>
        struct IsComponentSnapshot<ComponentSnapshot<TComp>> : std::true_type
        {
            using ComponentType = TComp;
        };

		// Push action onto stack with max size limit
        void PushToStack(std::deque<UndoAction>& stack, UndoAction action)
        {
            if (stack.size() >= maxSteps) stack.pop_front();
            stack.push_back(std::move(action));
        }

		// Handle undo for component addition by removing the component and 
        // saving its state if it existed
        void UndoComponentAdd(Coordinator& coord, Entity entity,
            const std::string& name, std::deque<UndoAction>& targetStack)
        {
            auto removeAndSave = [&]<typename TComp>(auto getField)
            {
                if (!coord.HasComponent<TComp>(entity)) return;

                ComponentRemoveSnapshot snap{ entity, name };
                getField(snap) = coord.GetComponent<TComp>(entity);
                PushToStack(targetStack, snap);

                coord.RemoveComponent<TComp>(entity);
            };

            if (name == "PlayerController")       removeAndSave.template operator() <PlayerController> ([](auto& s) -> auto& { return s.playercontroller; });
            else if (name == "RigidBody")         removeAndSave.template operator() <RigidBody> ([](auto& s) -> auto& { return s.rigidbody; });
            else if (name == "Collider")          removeAndSave.template operator() <Collider> ([](auto& s) -> auto& { return s.collider; });
            else if (name == "Renderable")        removeAndSave.template operator() <Renderable> ([](auto& s) -> auto& { return s.renderer; });
            else if (name == "Font")              removeAndSave.template operator() <Font> ([](auto& s) -> auto& { return s.font; });
            else if (name == "Button")            removeAndSave.template operator() <ButtonInfo> ([](auto& s) -> auto& { return s.button; });
            else if (name == "Audio")             removeAndSave.template operator() <Audio> ([](auto& s) -> auto& { return s.audio; });
            else if (name == "Character")         removeAndSave.template operator() <Character> ([](auto& s) -> auto& { return s.character; });
            else if (name == "Collectible")       removeAndSave.template operator() <Collectible> ([](auto& s) -> auto& { return s.collectible; });
            else if (name == "Scripting")   { if (coord.HasComponent<Script>(entity)) { coord.RemoveComponent<Script>(entity); PushToStack(targetStack, ComponentAddSnapshot{ entity, name }); } }
            else if (name == "Animator")    { if (coord.HasComponent<Animator>(entity)) { coord.RemoveComponent<Animator>(entity); PushToStack(targetStack, ComponentAddSnapshot{ entity, name }); } }
        }

		// Handle undo for component removal by re-adding the component with 
        // its previous state if it existed, and saving current state if it exists
        void UndoComponentRemove(Coordinator& coord, const ComponentRemoveSnapshot& act,
            std::deque<UndoAction>& targetStack)
        {
            auto addAndPush = [&]<typename TComp>(const std::optional<TComp>&saved)
            {
                if (!saved) return;
                coord.AddComponent<TComp>(act.entity, *saved);
                PushToStack(targetStack, ComponentAddSnapshot{ act.entity, act.componentName });
            };

            const auto& n = act.componentName;
            if (n == "PlayerController")       addAndPush.template operator() <PlayerController> (act.playercontroller);
            else if (n == "RigidBody")         addAndPush.template operator() <RigidBody> (act.rigidbody);
            else if (n == "Collider")          addAndPush.template operator() <Collider> (act.collider);
            else if (n == "Renderer")          addAndPush.template operator() <Renderable> (act.renderer);
            else if (n == "Font")              addAndPush.template operator() <Font> (act.font);
            else if (n == "Button")            addAndPush.template operator() <ButtonInfo> (act.button);
            else if (n == "Audio")             addAndPush.template operator() <Audio> (act.audio);
            else if (n == "Character")         addAndPush.template operator() <Character> (act.character);
            else if (n == "Collectible")       addAndPush.template operator() <Collectible> (act.collectible);
            else if (n == "Scripting" && act.script) { coord.AddComponent<Script>(act.entity, *act.script);   PushToStack(targetStack, ComponentAddSnapshot{ act.entity, n }); }
            else if (n == "Animator" && act.animator) { coord.AddComponent<Animator>(act.entity, *act.animator); PushToStack(targetStack, ComponentAddSnapshot{ act.entity, n }); }
        }

    };

}	// NightSnack

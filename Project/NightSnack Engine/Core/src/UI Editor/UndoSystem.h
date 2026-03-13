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

	// Snapshot for Physics Settings (not tied to an entity)
    struct PhysicsSettingsSnapshot
    {
        float gravity;
        float drag;
        float friction;
	};

	// Snapshots for adding/removing components
    struct ComponentAddSnapshot
    {
        Entity entity;
        std::string componentName;
    };

	// Snapshot for removing a component
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

	// Snapshots for entity creation/deletion
    struct EntityCreateSnapshot
    {
        Entity entity;
    };

	// Snapshot for components of a deleted entity
    struct EntityDeleteSnapshot
    {
        Entity      entity;
        std::string prefabName;
        Transform   transform;
        std::optional<PlayerController> playercontroller;
        std::optional<Camera>           camera;
        std::optional<RigidBody>        rigidbody;
        std::optional<Collider>         collider;
        std::optional<Renderable>       renderer;
        std::optional<Script>           script;
        std::optional<Animator>         animator;
        std::optional<Audio>            audio;
        std::optional<Font>             font;
        std::optional<ButtonInfo>       button;
        std::optional<Character>        character;
        std::optional<Collectible>      collectible;
    };

	// Snapshots for camera trigger creation/deletion
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

	// Variant type to hold any undo action
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

        /************************************************************************/
        /*!
        \brief
            Updates the UndoSystem.

        \param coord
            - Coordinator reference to access components and entities

        \param disp
            - EventDispatcher reference to subscribe to events
        */
        /************************************************************************/
        void Update(Coordinator& coord, EventDispatcher& disp);

        /************************************************************************/
        /*!
        \brief
		    Singleton access method for UndoSystem instance.
        */
        /************************************************************************/
        static UndoSystem& Instance()
        {
            static UndoSystem instance;
            return instance;
        }

        /************************************************************************/
        /*!
        \brief
            Pushes a new action onto the undo stack.

        \param action
			- The action to be added to the undo stack.
        */
        /************************************************************************/
        void Push(const UndoAction& action)
        {
            if (undoStack.size() >= maxSteps) undoStack.pop_front();
            undoStack.push_back(action);
            redoStack.clear();
        }

        /************************************************************************/
        /*!
        \brief
            Pops an action from the undo stack.
        */
        /************************************************************************/
        std::optional<UndoAction> Pop()
        {
            if (undoStack.empty()) { return std::nullopt; }
            auto action = undoStack.back();
            undoStack.pop_back();
            if (redoStack.size() >= maxSteps) redoStack.pop_front();
            redoStack.push_back(action);
            return action;
        }

        /************************************************************************/
        /*!
        \brief
		    Clears the undo & redo stack.
        */
        /************************************************************************/
        void Clear() { undoStack.clear(); }
        void ClearRedo() { redoStack.clear(); }

        /************************************************************************/
        /*!
        \brief
            Records an entity deletion for undo purposes.

		\param coord
		    - Coordinator reference to access components and entities
		
        \param factory
            - EntityFactory reference to create/remove entities as needed for undo operations.

		\param e
            - The entity to be deleted.
        */
        /************************************************************************/
        void PushEntityDelete(Coordinator& coord, EntityFactory& factory, Entity e)
        {
            Push(CaptureEntity(coord, factory, e));
        }

        /************************************************************************/
        /*!
        \brief
			Performs the undo operation. This will revert the last action 
            taken in the editor, such as modifying a component, 
            adding/removing a component, creating/deleting an entity, 
            or creating/deleting a camera trigger.

        \param coord
			- Coordinator reference to access components and entities

        \param factory
			- EntityFactory reference to create/remove entities as needed for undo operations.
        */
        /************************************************************************/
        void PerformUndo(Coordinator& coord, EntityFactory& factory)
        {
			// Get the last action, return if none
            if (undoStack.empty()) return;
            auto action = undoStack.back();
            undoStack.pop_back();

            std::visit([&](auto&& act) {
                using T = std::decay_t<decltype(act)>;

				// For component modifications, capture current state for redo and revert to old state
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
                    // Capture all components before removing
                    auto snap = CaptureEntity(coord, factory, act.entity);
                    factory.RemoveEntity(act.entity);
                    // Redo stack for entity deletion
                    PushToStack(redoStack, snap);
                }
                else if constexpr (std::is_same_v<T, EntityDeleteSnapshot>)
                {
                    // Create base entity, restore all captured components
                    Entity newEntity = factory.Create(act.prefabName, act.transform);
                    RestoreEntity(coord, newEntity, act);
                    if (coord.HasComponent<Transform>(newEntity))
                        coord.ModifyComponent<Transform>(newEntity) = act.transform;
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

        /************************************************************************/
        /*!
        \brief
            Performs a redo operation by reapplying the last undone action.

		\param coord
		    - Coordinator reference to access components and entities

        \param factory
			- EntityFactory reference to create/remove entities as needed for redo operations.
        */
        /************************************************************************/
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
                    // Capture all components before removing
                    auto snap = CaptureEntity(coord, factory, act.entity);
                    factory.RemoveEntity(act.entity);
                    // Undo stack for entity deletion
                    PushToStack(undoStack, snap);
                }
                else if constexpr (std::is_same_v<T, EntityDeleteSnapshot>)
                {
                    // Create base entity, restore all captured components
                    Entity newEntity = factory.Create(act.prefabName, act.transform);
                    RestoreEntity(coord, newEntity, act);
                    if (coord.HasComponent<Transform>(newEntity))
                        coord.ModifyComponent<Transform>(newEntity) = act.transform;
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

        /************************************************************************/
        /*!
        \brief
            Remaps the entity ID in the undo stack.

        \param oldId
			- The old entity ID to be replaced.

		\param newId
		    - The new entity ID to replace with.
        */
        /************************************************************************/
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

        /************************************************************************/
        /*!
        \brief
			Pushes a new action onto the specified stack.

		\param stack
		    - The stack to push the action onto (undo or redo).

		\param action
            - The action to be added to the stack.
        */
        /************************************************************************/
        // Captures all components from an entity into an EntityDeleteSnapshot
        EntityDeleteSnapshot CaptureEntity(Coordinator& coord, EntityFactory& factory, Entity e)
        {
            EntityDeleteSnapshot snap;
            snap.entity    = e;
            snap.prefabName = factory.GetEntityPrefabName(e);
            if (coord.HasComponent<Transform>        (e)) snap.transform        = coord.GetComponent<Transform>        (e);
            if (coord.HasComponent<PlayerController> (e)) snap.playercontroller = coord.GetComponent<PlayerController> (e);
            if (coord.HasComponent<Camera>           (e)) snap.camera           = coord.GetComponent<Camera>           (e);
            if (coord.HasComponent<RigidBody>        (e)) snap.rigidbody        = coord.GetComponent<RigidBody>        (e);
            if (coord.HasComponent<Collider>         (e)) snap.collider         = coord.GetComponent<Collider>         (e);
            if (coord.HasComponent<Renderable>       (e)) snap.renderer         = coord.GetComponent<Renderable>       (e);
            if (coord.HasComponent<Script>           (e)) snap.script           = coord.GetComponent<Script>           (e);
            if (coord.HasComponent<Animator>         (e)) snap.animator         = coord.GetComponent<Animator>         (e);
            if (coord.HasComponent<Audio>            (e)) snap.audio            = coord.GetComponent<Audio>            (e);
            if (coord.HasComponent<Font>             (e)) snap.font             = coord.GetComponent<Font>             (e);
            if (coord.HasComponent<ButtonInfo>       (e)) snap.button           = coord.GetComponent<ButtonInfo>       (e);
            if (coord.HasComponent<Character>        (e)) snap.character        = coord.GetComponent<Character>        (e);
            if (coord.HasComponent<Collectible>      (e)) snap.collectible      = coord.GetComponent<Collectible>      (e);
            return snap;
        }

        // Restores all components from an EntityDeleteSnapshot onto a freshly created entity
        void RestoreEntity(Coordinator& coord, Entity newEntity, const EntityDeleteSnapshot& snap)
        {
            if (snap.playercontroller) coord.AddComponent<PlayerController>(newEntity, *snap.playercontroller);
            if (snap.camera)           coord.AddComponent<Camera>          (newEntity, *snap.camera);
            if (snap.rigidbody)        coord.AddComponent<RigidBody>       (newEntity, *snap.rigidbody);
            if (snap.collider)         coord.AddComponent<Collider>        (newEntity, *snap.collider);
            if (snap.renderer)         coord.AddComponent<Renderable>      (newEntity, *snap.renderer);
            if (snap.script)           coord.AddComponent<Script>          (newEntity, *snap.script);
            if (snap.animator)         coord.AddComponent<Animator>        (newEntity, *snap.animator);
            if (snap.audio)            coord.AddComponent<Audio>           (newEntity, *snap.audio);
            if (snap.font)             coord.AddComponent<Font>            (newEntity, *snap.font);
            if (snap.button)           coord.AddComponent<ButtonInfo>      (newEntity, *snap.button);
            if (snap.character)        coord.AddComponent<Character>       (newEntity, *snap.character);
            if (snap.collectible)      coord.AddComponent<Collectible>     (newEntity, *snap.collectible);
        }

        void PushToStack(std::deque<UndoAction>& stack, UndoAction action)
        {
            if (stack.size() >= maxSteps) stack.pop_front();
            stack.push_back(std::move(action));
        }

        /************************************************************************/
        /*!
        \brief
            Undoes the addition of a component by removing it 
            and restoring its previous state if it existed.

		\param coord
		    - Coordinator reference to access components and entities

        \param entity
			- The entity from which the component was added.

		\param name
			- The name of the component to be removed.

        \param targetStack
			- The stack to push the undo action onto (undo or redo).
        */
        /************************************************************************/
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

        /************************************************************************/
        /*!
        \brief
            Undoes a component removal action by re-adding the component 
            with its previous state.

		\param coord
			- Coordinator reference to access components and entities.

        \param act
			- The action to be undone.

        \param targetStack
			- The stack to push the redo action onto.
        */
        /************************************************************************/
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

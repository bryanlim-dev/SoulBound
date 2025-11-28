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

    using UndoAction = std::variant
        <TransformSnapshot, 
        EntityCreateSnapshot, 
        EntityDeleteSnapshot>;

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
        static constexpr size_t maxSteps = 20;
        std::deque<UndoAction> undoStack;

        UndoSystem() = default;
        
        UndoSystem(const UndoSystem&) = delete;
        
        UndoSystem& operator=(const UndoSystem&) = delete;
    };

}	// NightSnack

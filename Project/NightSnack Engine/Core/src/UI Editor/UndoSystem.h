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

    struct TransformSnapshot
    {
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
        // Other components...
    };

    using UndoAction = std::variant
        <TransformSnapshot, 
        EntityCreateSnapshot, 
        EntityDeleteSnapshot>;

    class UndoSystem {
    public:
        static UndoSystem& Instance()
        {
            static UndoSystem instance;
            return instance;
        }

        void Push(const UndoAction& action)
        {
            if (undoStack.size() >= maxSteps) undoStack.pop_front();
            undoStack.push_back(action);
        }

        std::optional<UndoAction> Pop()
        {
            if (undoStack.empty()) return std::nullopt;
            auto action = undoStack.back();
            undoStack.pop_back();
            return action;
        }

        void Clear() { undoStack.clear(); }

        void PerformUndo(Coordinator& coord, EntityFactory& factory)
        {
            auto action = Pop();
            if (!action) return;

            std::visit([&](auto&& act) {
                using T = std::decay_t<decltype(act)>;
                if constexpr (std::is_same_v<T, TransformSnapshot>)
                {
                    if (coord.HasComponent<Transform>(act.entity))
                        coord.ModifyComponent<Transform>(act.entity) = act.transform;
                }
                else if constexpr (std::is_same_v<T, EntityCreateSnapshot>)
                {
                    coord.DestroyEntity(act.entity);
                }
                else if constexpr (std::is_same_v<T, EntityDeleteSnapshot>)
                {
                    factory.Create(act.prefabName, act.transform);
                }
                }, *action);
        }

    private:
        static constexpr size_t maxSteps = 20;
        std::deque<UndoAction> undoStack;

        UndoSystem() = default;
        
        UndoSystem(const UndoSystem&) = delete;
        
        UndoSystem& operator=(const UndoSystem&) = delete;
    };

}	// NightSnack

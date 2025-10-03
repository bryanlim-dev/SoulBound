/******************************************************************************
 * File:        IEvent.h
 * Project:     NightSnack
 * Author(s):
 *
 * Description:
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include <typeindex>

enum class EventType;

class IEvent
{
public:
	virtual ~IEvent() = default;
	virtual EventType GetType() const = 0;
};

class EventDispatcher 
{
public:
    template<typename EventType>
    void Subscribe(std::function<void(const EventType&)> listener) 
    {
        auto& listeners = m_listeners[std::type_index(typeid(EventType))];
        listeners.push_back([listener](const void* event) 
        {
            listener(*static_cast<const EventType*>(event));
        });
    }

    template<typename EventType>
    void Dispatch(const EventType& event) 
    {
        auto it = m_listeners.find(std::type_index(typeid(EventType)));
        if (it != m_listeners.end())
        {
            for (auto& listener : it->second) 
            {
                listener(&event);
            }
        }
    }

private:
    std::unordered_map<std::type_index, std::vector<std::function<void(const void*)>>> m_listeners;
};

#include "Systems/EventTypes.h"

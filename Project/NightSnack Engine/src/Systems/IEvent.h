/******************************************************************************
 * File:        IEvent.h
 * Project:     NightSnack
 * Author(s):   Lim Wei Jie (Primary, 100%) - Created event interface with 
 *              dispatch and subscription system
 *
 * Description: This file contains the definition of an event interface (IEvent)
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include <typeindex>

namespace NightSnack {

    // Forward declaration of EventType enum
    enum class EventType;

    /************************************************************************/
    /*!
    \brief
    Interface for all events in the system. Includes a virtual destructor
    and a method to get the event type.
    */
    /************************************************************************/
    class IEvent
    {
    public:
        virtual ~IEvent() = default;
        virtual EventType GetType() const = 0;
    };

    class EventDispatcher
    {
    public:
        /************************************************************************/
        /*!
        \brief
        Subscribes a listener function to a specific event type.
        When an event of that type is dispatched, the listener will be called.

        \param[in] listener
        The function to be called when the event is received.
        */
        /************************************************************************/
        template<typename EventType>
        void Subscribe(std::function<void(const EventType&)> listener)
        {
            auto& listeners = m_listeners[std::type_index(typeid(EventType))];
            listeners.push_back([listener](const void* event)
                {
                    listener(*static_cast<const EventType*>(event));
                });
        }

        /************************************************************************/
        /*!
        \brief
        Dispatches an event to all subscribed listeners of that event type.

        \param[in] event
        The event instance to be dispatched.
        */
        /************************************************************************/
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

        // unordered_map of event type to vector of listener functions
    private:
        std::unordered_map<std::type_index, std::vector<std::function<void(const void*)>>> m_listeners;
    };

}   // NightSnack

// placed here to avoid circular dependency
#include "Systems/EventTypes.h"

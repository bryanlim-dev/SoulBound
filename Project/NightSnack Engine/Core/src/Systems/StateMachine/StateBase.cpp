/******************************************************************************
 * File:        StateBase.cpp
 * Project:     NightSnack
 * Author(s):	Clement Ang Yang Ying (Primary, 100%) - Created class and logic
 *
 * Description:
 * This file contains the definition of the class StateBase.
 * It is the class that holds data on a state for the state machine such as
 * which states to transition to and their requirements, update, eneter, exit
 * and their state components.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
#include "StateBase.h"
#include <Systems/PlayerController.h>
#include <fstream>

namespace NightSnack
{

    void StateBase::Enter(Entity entityID, Coordinator& coord)
    {
        m_isExit = false;
        //std::cout << "ID: " << entityID << ", State Enter: " << m_stateName << std::endl;

        if (m_enterFunction)
            m_enterFunction(entityID, coord);

        //Structure bindings to iterate
        for (auto& [name, component] : m_stateComponentMap)
        {
            component->OnEnter(entityID, coord);
        }
    }

    void StateBase::Update(float dt, Entity entityID, Coordinator& coord)
    {
        //Call update function
        if (m_updateFunction)
            m_updateFunction(dt, entityID, coord);

        //Structure bindings to iterate
        for (auto& [name, component] : m_stateComponentMap)
        {
            component->OnUpdate(dt, entityID, coord);
        }

        //Check thru transition list
        for (auto& stateFuncPair : m_transtitionStatesList)
        {
            if (stateFuncPair.second && stateFuncPair.second(entityID, coord))
            {
                m_nextState = stateFuncPair.first;
                m_isExit = true;
                return;
            }
        }
    }

    void StateBase::Exit(Entity entityID, Coordinator& coord)
    {
        m_isExit = false;
        //std::cout << "ID: " << entityID << ", State Exit: " << m_stateName << std::endl;

        if (m_exitFunction)
            m_exitFunction(entityID, coord);

        //Structure bindings to iterate
        for (auto& [name, component] : m_stateComponentMap)
        {
            component->OnExit(entityID, coord);
        }

        if (!m_nextState.expired())
        {
            m_nextState.reset();
        }
    }

    States CreateStateFromArchetypeJson(std::string stateArchetype, std::string stateArchetypeFilePath)
    {
        //Read json files
        std::ifstream file(stateArchetypeFilePath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open archetypes states file: " + stateArchetypeFilePath);
        }

        json statesArchetypeJson;
        try {
            file >> statesArchetypeJson;
        }
        catch (json::parse_error& e) {
            throw std::runtime_error("JSON parse error at byte " + std::to_string(e.byte) + ": " + e.what());
        }

        States states;

        //States have 2 part (components & transitions)

        //For the archetype in the string
        //Loop through all states within the list
        for (auto& [stateName, stateJson] : statesArchetypeJson[stateArchetype]["States"].items())
        {
            std::shared_ptr<StateBase> statePtr = std::make_shared<StateBase>(stateName);

            //Loop through the all params in the list and create them
            for (auto& [componentType, paramJsonInfo] : stateJson["behaviourTypes"].items())
            {
                std::shared_ptr<StateBehaviour> component = CreateStateBehaviour(componentType, paramJsonInfo);
                statePtr->InsertStateComponent(componentType, component);
            }

            //Insert into map
            states.stateMap.insert({ stateName, statePtr });

        } //End of base state create loop

        //Now add in all transitions
        for (auto& [stateName, stateJson] : statesArchetypeJson[stateArchetype]["States"].items())
        {
            std::shared_ptr<StateBase> statePtr = states.stateMap[stateName];

            //Loop through the all params in the list and create them
            for (auto& [paramType, paramJsonInfo] : stateJson["transitions"].items())
            {
                std::string transitionTarget, transitionFunc; 
                paramJsonInfo.at("target").get_to(transitionTarget);
                paramJsonInfo.at("condition").get_to(transitionFunc);

                //Check if actually in map
                if (states.stateMap.find(transitionTarget) != states.stateMap.end())
                {
                    //Valid in map
                    statePtr->InsertStateTransition(states.stateMap[transitionTarget], GetTransitionFunction(transitionFunc));
                }
                else
                    std::cout << "INVALID Transition add attempted! State: " << stateName << " to " << transitionTarget << std::endl;
            }

            //Insert into map
            states.stateMap.insert({ stateName, statePtr });

        } //End of transition state create loop

        //Input first state
        std::string firstState;
        statesArchetypeJson[stateArchetype].at("InitialState").get_to(firstState);

        if (states.stateMap.find(firstState) != states.stateMap.end())
        {
            //Valid in map
            states.currState = states.stateMap[firstState];
        }
        else
            std::cout << "INVALID first state! State: " << firstState << std::endl;

        return states;
    }


}
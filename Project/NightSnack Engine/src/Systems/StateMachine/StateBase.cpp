#include "StateBase.h"
#include <Systems/PlayerController.h>
#include <fstream>

namespace NightSnack
{


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
            for (auto& [componentType, paramJsonInfo] : stateJson["componentTypes"].items())
            {
                std::shared_ptr<StateComponent> component = CreateStateComponent(componentType, paramJsonInfo);
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
#include "StateBase.h"
#include <Systems/PlayerController.h>
#include <fstream>

namespace NightSnack
{
    inline void from_json(const json& j, States& c) {
        std::string stateMachineStr;
        j.at("StateMachine").get_to(stateMachineStr); //State Machine Archytype

        c = CreateStateFromArchetypeJson(stateMachineStr, "assets/GameData/Archetypes/AIStatesArchetypes.json");
    }

    States&& CreateStateFromArchetypeJson(std::string stateArchetype, std::string stateArchetypeFilePath)
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
        for (auto& [stateName, stateJson] : statesArchetypeJson[stateArchetype].items())
        {
            std::shared_ptr<StateBase> statePtr = std::make_shared<StateBase>(stateName);

            //Loop through the all params in the list and create them
            for (auto& [paramType, paramJsonInfo] : stateJson["componentTypes"].items())
            {
                std::shared_ptr<StateComponent> component = CreateStateComponent(paramType, paramJsonInfo);
                statePtr->InsertStateComponent(paramType, component);
            }

            //Insert into map
            states.stateMap.insert({ stateName, statePtr });

        } //End of base state create loop

        //Now add in all transitions


        return std::move(states);
    }

}
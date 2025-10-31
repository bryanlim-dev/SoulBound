#pragma once

#ifndef STATEBASE_H
#define STATEBASE_H

#include <memory>
#include <string>
#include <functional>
#include <iostream>

#include <Core/ECS/Coordinator.h>
#include "StateComponents.h"

namespace NightSnack
{
	//Defined in StateComponets
	//using TransitionFunction = std::function<bool(Entity, Coordinator&)>;

	class StateBase
	{
	protected:
		std::list<std::pair<std::weak_ptr<StateBase>, TransitionFunction>> m_transtitionStatesList;
		
		//A map of components for states and thier names
		//Refactor tracker: 8
		std::unordered_map<std::string, std::shared_ptr<StateComponent>> m_stateComponentMap;

		//Function returns nothing an takes in a EntityID and float (dt)
		std::function<void (Entity, Coordinator&)> m_enterFunction = nullptr;
		std::function<void (float, Entity)> m_updateFunction = nullptr;
		std::function<void (Entity, Coordinator&)> m_exitFunction = nullptr;

		std::weak_ptr<StateBase> m_nextState;

		std::string m_stateKey; //Can double as the state name

		bool m_isExit = false;

	public:
		StateBase(std::string keyName) : m_stateKey(keyName) {};

		//StateBase(Entity id, std::string stateKeyName, Coordinator& coord,
		//	std::function<void(float, Entity)> updateFunction = [](float, Entity) { /* Default is empty by Design*/ })
		//	:m_entityID(id), m_stateKey(stateKeyName), m_coord(coord){
		//	m_updateFunction = updateFunction;
		//	//std::cout << "State adresss in constructor: " << this << "\n";
		//}

		virtual ~StateBase() {
			if (!m_nextState.expired())
			{
				m_nextState.reset();
			}

			m_enterFunction = nullptr;
			m_updateFunction = nullptr;
			m_exitFunction = nullptr;

			m_transtitionStatesList.clear();
			m_stateComponentMap.clear();
		}

		void SetEnterFunction(std::function<void(float, Coordinator&)> enterFunc)
		{
			m_enterFunction = enterFunc;
		}

		void SetExitFunction(std::function<void(float, Coordinator&)> exitFunc)
		{
			m_exitFunction = exitFunc;
		}

		void InsertStateTransition(std::weak_ptr<StateBase> state, TransitionFunction transition)
		{
			m_transtitionStatesList.emplace_back(state, transition);
		}

		void SetNextState(std::shared_ptr<StateBase> newNextState) {
			m_nextState = newNextState;
		}

		virtual void Enter(Entity entityID, Coordinator& coord) {
			m_isExit = false; 
			std::cout << "ID: " << entityID << ", State Enter: " << m_stateKey << std::endl;

			if (m_enterFunction)
				m_enterFunction(entityID, coord);

			//Structure bindings to iterate
			for (auto& [name, component] : m_stateComponentMap)
			{
				component->OnEnter(entityID, coord);
			}
		}

		virtual void Update(float dt, Entity entityID, Coordinator& coord) {
			//Call update function
			if (m_updateFunction)
				m_updateFunction(dt, entityID);

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

		virtual void Exit(Entity entityID, Coordinator& coord) {
			m_isExit = false; 
			std::cout << "ID: " << entityID << ", State Exit: " << m_stateKey << std::endl;

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


		std::string StateKey() const
		{
			return m_stateKey;
		}

		std::weak_ptr<StateBase> NextState() const
		{
			return m_nextState;
		}

		bool IsExit() const
		{
			return m_isExit;
		}

		//State components
		void InsertStateComponent(std::string const& name, std::shared_ptr<StateComponent> component)
		{
			m_stateComponentMap.insert(std::make_pair(name, component));
		}

		//This only dcheck if its exists not the type
		bool HasStateComponent(std::string const& stateName)
		{
			if (m_stateComponentMap.find(stateName) != m_stateComponentMap.end())
				return true;

			return false;
		}

		std::shared_ptr<StateComponent> GetStateComponent(std::string stateName)
		{
			//Find if it is in map
			auto it = m_stateComponentMap.find(stateName);
			if (it == m_stateComponentMap.end()) //It is not in map nullptr return
				return nullptr;

			//Cast it to a shared ptr, as this is how it is stored
			return std::shared_ptr<StateComponent>(it->second);
		}
	};

	States CreateStateFromArchetypeJson(std::string stateArchetype, std::string stateArchetypeFilePath);

	inline void from_json(const json& j, States& c) {
		std::string stateMachineStr;
		j.at("StateMachine").get_to(stateMachineStr); //State Machine Archytype

		c = CreateStateFromArchetypeJson(stateMachineStr, "assets/GameData/Archetypes/AIStatesArchetypes.json");
	}


} //End of namesapce

#endif 
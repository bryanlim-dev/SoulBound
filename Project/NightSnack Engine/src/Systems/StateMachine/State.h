#pragma once

#ifndef State
#define State

#include <memory>
#include <string>
#include <functional>
#include <iostream>
#include <Core/ECS/Coordinator.h>
#include "StateComponents.h"

namespace NightSnack
{

	class StateBase
	{
	protected:
		std::list<std::pair<std::shared_ptr<StateBase>, std::function<bool(Entity, Coordinator&)>>> m_transtitionStatesList;
		
		//Using any allows more data types to be store, just check if it can be converted before using
		//Components are shared ptrs to allow for casting
		//Maybe instead of making it too static i made it too dynamic this time?
		//Refactor tracker: 8
		std::unordered_map<std::string, std::shared_ptr<StateComponent>> m_stateComponentMap;

		//Function returns nothing an takes in a EntityID and float (dt)
		std::function<void (Entity, Coordinator&)> m_enterFunction;
		std::function<void (float, Entity)> m_updateFunction;
		std::function<void (Entity, Coordinator&)> m_exitFunction;

		std::shared_ptr<StateBase> m_nextState = nullptr;
		Entity m_entityID;
		Coordinator& m_coord;

		std::string m_stateKey; //Can double as the state name

		bool m_isExit = false;

	public:
		StateBase() = delete;

		StateBase(Entity id, std::string stateKeyName, Coordinator& coord,
			std::function<void(float, Entity)> updateFunction = [](float, Entity) { /* Default is empty by Design*/ })
			:m_entityID(id), m_stateKey(stateKeyName), m_coord(coord){
			m_updateFunction = updateFunction;
			//std::cout << "State adresss in constructor: " << this << "\n";
		}

		virtual ~StateBase() {
			if (m_nextState)
			{
				m_nextState = nullptr;
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

		void InsertStateTransition(std::shared_ptr<StateBase> state, std::function<bool(Entity, Coordinator&)>transition)
		{
			m_transtitionStatesList.emplace_back(state, transition);
		}

		void SetNextState(std::shared_ptr<StateBase> newNextState) {
			m_nextState = newNextState;
		}

		virtual void Enter() {
			m_isExit = false; 
			std::cout << "ID: " << m_entityID << ", State Enter: " << m_stateKey << std::endl;

			if (m_enterFunction)
				m_enterFunction(m_entityID, m_coord);

			//Structure bindings to iterate
			for (auto& [name, component] : m_stateComponentMap)
			{
				component->OnEnter(m_entityID, m_coord);
			}
		}

		virtual void Update(float dt) {
			//Call update function
			m_updateFunction(dt, m_entityID);

			//Structure bindings to iterate
			for (auto& [name, component] : m_stateComponentMap)
			{
				component->OnUpdate(dt, m_entityID, m_coord);
			}

			//Check thru transition list
			for (auto& stateFuncPair : m_transtitionStatesList)
			{
				if (stateFuncPair.second && stateFuncPair.second(m_entityID, m_coord))
				{
					m_nextState = stateFuncPair.first;
					m_isExit = true;
					return;
				}
			}
		}

		virtual void Exit() { 
			m_isExit = false; 
			std::cout << "ID: " << m_entityID << ", State Exit: " << m_stateKey << std::endl;

			if (m_exitFunction)
				m_exitFunction(m_entityID, m_coord);

			//Structure bindings to iterate
			for (auto& [name, component] : m_stateComponentMap)
			{
				component->OnExit(m_entityID, m_coord);
			}
		}


		std::string StateKey() const
		{
			return m_stateKey;
		}

		std::shared_ptr<StateBase> NextState() const
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
} //End of namesapce

#endif 
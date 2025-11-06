/******************************************************************************
 * File:        StateBase.h
 * Project:     NightSnack
 * Author(s):	Clement Ang Yang Ying (Primary, 100%) - Created class and logic
 *
 * Description:
 * This file contains the deleclration and some definitions of the class StateBase.
 * It is the class that holds data on a state for the state machine such as
 * which states to transition to and their requirements, update, eneter, exit
 * and their state components.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
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

		std::function<void (Entity, Coordinator&)> m_enterFunction = nullptr;
		std::function<void (float, Entity, Coordinator&)> m_updateFunction = nullptr;
		std::function<void (Entity, Coordinator&)> m_exitFunction = nullptr;

		std::weak_ptr<StateBase> m_nextState;

		std::string m_stateName; //Can double as the state name

		bool m_isExit = false;

	public:
		/************************************************************************/
		/*!
		\brief
		Constructor for StateBase class

		\param[in] stateName
		Name of the state
		*/
		/************************************************************************/
		StateBase(std::string stateName) : m_stateName(stateName) {};

		//StateBase(Entity id, std::string stateKeyName, Coordinator& coord,
		//	std::function<void(float, Entity)> updateFunction = [](float, Entity) { /* Default is empty by Design*/ })
		//	:m_entityID(id), m_stateName(stateKeyName), m_coord(coord){
		//	m_updateFunction = updateFunction;
		//	//std::cout << "State adresss in constructor: " << this << "\n";
		//}

		/************************************************************************/
		/*!
		\brief
		Destructor of the StateBase Class
		*/
		/************************************************************************/
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

		/************************************************************************/
		/*!
		\brief
		Sets a function that will activate when entering state

		\param[in] enterFunc
		std::function that return nothing and takes in a Entity and
		the Coordinator refrence
		*/
		/************************************************************************/
		void SetEnterFunction(std::function<void(Entity, Coordinator&)> enterFunc)
		{
			m_enterFunction = enterFunc;
		}

		/************************************************************************/
		/*!
		\brief
		Sets a function that will activate when updating the state

		\param[in] enterFunc
		std::function that return nothing and takes in a float (deltaTime, dt),
		Entity and the Coordinator refrence
		*/
		/************************************************************************/
		void SetUpdateFunction(std::function<void(float, Entity, Coordinator&)> updateFunc)
		{
			m_updateFunction = updateFunc;
		}

		/************************************************************************/
		/*!
		\brief
		Sets a function that will activate when exiting state

		\param[in] enterFunc
		std::function that return nothing and takes in a Entity and
		the Coordinator refrence
		*/
		/************************************************************************/
		void SetExitFunction(std::function<void(Entity, Coordinator&)> exitFunc)
		{
			m_exitFunction = exitFunc;
		}

		/************************************************************************/
		/*!
		\brief
		Inserts a state to transition to and the trigger condition required to
		transition to it.

		\param[in] state
		A weak ptr to the state to transition to

		\param[in] transition
		The transition function to check, it returns a bool and takes in the Entity 
		and coordinator refrence
		*/
		/************************************************************************/
		void InsertStateTransition(std::weak_ptr<StateBase> state, TransitionFunction transition)
		{
			m_transtitionStatesList.emplace_back(state, transition);
		}

		/************************************************************************/
		/*!
		\brief
		Sets the next state that will be transitioned to

		\param[in] state
		A shared ptr to the state to transition to

		*/
		/************************************************************************/
		void SetNextState(std::shared_ptr<StateBase> newNextState) {
			m_nextState = newNextState;
		}

		/************************************************************************/
		/*!
		\brief
		Enter function that will run when the state is activated

		\param[in] entityID
		Entity id of the entity that this state belongs to

		\param[in] coord
		Coordinator refrence to access data from
		*/
		/************************************************************************/
		virtual void Enter(Entity entityID, Coordinator& coord);

		/************************************************************************/
		/*!
		\brief
		Update function that will run when the state is updated

		\param[in] dt
		Delta time of the time to update this function

		\param[in] entityID
		Entity id of the entity that this state belongs to

		\param[in] coord
		Coordinator refrence to access data from
		*/
		/************************************************************************/
		virtual void Update(float dt, Entity entityID, Coordinator& coord);

		/************************************************************************/
		/*!
		\brief
		Exit function that will run when the state is activated

		\param[in] entityID
		Entity id of the entity that this state belongs to

		\param[in] coord
		Coordinator refrence to access data from
		*/
		/************************************************************************/
		virtual void Exit(Entity entityID, Coordinator& coord);


		/************************************************************************/
		/*!
		\brief
		Get the name of the state

		\return
		Returns the name of the state as a string
		*/
		/************************************************************************/
		std::string StateName() const
		{
			return m_stateName;
		}

		/************************************************************************/
		/*!
		\brief
		Get a weak ptr to the next state

		\return
		Returns a weak ptr to the next state to transition to
		*/
		/************************************************************************/
		std::weak_ptr<StateBase> NextState() const
		{
			return m_nextState;
		}

		/************************************************************************/
		/*!
		\brief
		Get if the state transition has triggered

		\return
		Returns a bool if the state is ready to transition
		*/
		/************************************************************************/
		bool IsExit() const
		{
			return m_isExit;
		}

		/************************************************************************/
		/*!
		\brief
		Adds a state component to the state

		\param[in] name
		Name of the state component

		\param[in] component
		shared ptr of the state component to add
		*/
		/************************************************************************/
		void InsertStateComponent(std::string const& name, std::shared_ptr<StateComponent> component)
		{
			m_stateComponentMap.insert(std::make_pair(name, component));
		}

		/************************************************************************/
		/*!
		\brief
		Checks if the state component exists (via name only not the type)

		\param[in] stateName
		Name of the state component

		\return
		bool of if the state component exists
		*/
		/************************************************************************/
		bool HasStateComponent(std::string const& stateName)
		{
			if (m_stateComponentMap.find(stateName) != m_stateComponentMap.end())
				return true;

			return false;
		}

		/************************************************************************/
		/*!
		\brief
		Returns a shared ptr to a state componenet

		\param[in] stateName
		Name of the state component to return

		\return
		shared ptr of the state componenet
		*/
		/************************************************************************/
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

	/************************************************************************/
	/*!
	\brief
	Function to create the entity component States from a json input

	\param[in] stateArchetype
	Name of the state archetype to access

	\param[in] stateArchetypeFilePath
	File path of the state archetype json to access 

	\return
	Entity componenet States with the state initaliized
	*/
	/************************************************************************/
	States CreateStateFromArchetypeJson(std::string stateArchetype, std::string stateArchetypeFilePath);

	/************************************************************************/
	/*!
	\brief
	inline function to read a json and returns the component States via paramater 

	\param[in] j
	json data to read from

	\param[out] c
	States refrence to return the initalized component from json
	*/
	/************************************************************************/
	inline void from_json(const json& j, States& c) {
		std::string stateMachineStr;
		j.at("StateMachine").get_to(stateMachineStr); //State Machine Archytype

		c = CreateStateFromArchetypeJson(stateMachineStr, "../assets/GameData/Archetypes/AIStatesArchetypes.json");
	}


} //End of namesapce

#endif 
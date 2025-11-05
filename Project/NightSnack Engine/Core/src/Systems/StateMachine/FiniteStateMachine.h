/******************************************************************************
 * File:        FiniteStateMachine.cpp
 * Project:     NightSnack
 * Author(s):	Clement Ang Yang Ying (Primary, 100%) - Created class and logic
 *
 * Description:
 * This file contains the deceleration of the system class FiniteStateMachine.
 * It is NOT owned by a single entity and is instead a system that takes in
 * the component States from entities and trigger
 * the state functions (Upadte/Eneter/Exit/Switchhing,etc.)
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
#pragma once
#include <map>
#include "StateBase.h"
//#include "../../Core/ECS/Component.h"
//#include "../../Core/ECS/Coordinator.h"

namespace NightSnack
{
	//Finite State Machine dosnt belong to a single entity.
	//It takes in the component struct States and a entity ID,
	//then uses the executes the logic for each state within it.
	class FiniteStateMachine : public ISystem
	{
	private:
		Signature m_systemSignature{};
		Coordinator& m_coord;

	public:
		/************************************************************************/
		/*!
		\brief
		Default Constructor for FiniteStateMachine

		\param coord
		Refrence to the coordinator
		*/
		/************************************************************************/
		FiniteStateMachine(Coordinator& coord);

		/************************************************************************/
		/*!
		\brief
		Destructor for FiniteStateMachine
		*/
		/************************************************************************/
		~FiniteStateMachine();

		/************************************************************************/
		/*!
		\brief
		Overriden Update function for FiniteStateMachine
		Loops thorugh each entity with a States component and update their states

		\param steps
		interger of how many times to peform the update
		*/
		/************************************************************************/
		void Update(int steps) override;


		/************************************************************************/
		/*!
		\brief
		Returns name of system (overridden from ISystem)
		Required by ISystem base class;

		\return
		Returns name of system (FiniteStateMachine)
		*/
		/************************************************************************/
		std::string GetName() const override;
		/************************************************************************/
		/*!
		\brief
		Returns signature of system (overridden from ISystem)
		Required by ISystem base class;

		\return
		Returns name of system (FiniteStateMachine)
		*/
		/************************************************************************/
		Signature GetSignature() const override;

		/************************************************************************/
		/*!
		\brief
		Creates a States component with debug states within and adds it to the entity

		\param entityID
		ID of the entity to add debug states to

		\param coord
		Coordinator to access the entity from
		*/
		/************************************************************************/
		static void CreateDebugTestStates(Entity entityId, Coordinator& coord) ;

	private:
		/************************************************************************/
		/*!
		\brief
		Function to update the states of an entity
		Will check for state switch and switch/enter/exit accordingly

		\param dt
		Delta time to update by

		\param id
		id of the entity to udpate

		\param coord
		Refrence to States component of the entity to update

		*/
		/************************************************************************/
		void UpdateEntityState(float dt, Entity id, States& states);
	};
}
/******************************************************************************
 * File:        StateComponents.h
 * Project:     NightSnack
 * Author(s):	Clement Ang Yang Ying (Primary, 100%) - Created class and logic
 *
 * Description:
 * This file contains the deleclration for the state component class and state transition functions
 * It is the class that holds data on a state as componets asa well as update eneter
 * and exit functions
 * This file also contatins the transitions functions to singal that a transition
 * to another state is avaliable
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
#pragma once
#include "../../Core/ECS/Component.h"
#include ".././../MathLibrary/Vector.h"
#include "../../Core/ECS/Entity.h"
#include <functional>

namespace NightSnack
{
	//Foward delectration of coordinator 
	class Coordinator;

	using TransitionFunction = std::function<bool(Entity, Coordinator&)>;

	//A namespace of checks for conditions and transitions
	namespace StateFunctionChecker
	{
		enum STATE_FUNCTIONS
		{
			STATE_FUNCTION_COUNTDOWN,
			
			STATE_FUNCTION_PLAYER_LEFTSIDE,
			STATE_FUNCTION_PLAYER_RIGHTSIDE,

			STATE_FUNCTION_ENTITY_COLLIDING,
			STATE_FUNCTION_ENTITY_ISGROUNDED

		};

		/************************************************************************/
		/*!
		\brief
		Function that checks if the countdown component of the entity is finished

		\param[in] ownerID
		id of the entity that owns the state
		
		\param[in] coord
		Refrence to the coordinator to get data from

		\return
		bool of if the countdown component is finished
		*/
		/************************************************************************/
		bool EntityCountdownComponentFinish(Entity ownerID, Coordinator& coord);

		/************************************************************************/
		/*!
		\brief
		Function that checks if the player is to the left of the entity

		\param[in] ownerID
		id of the entity that owns the state

		\param[in] coord
		Refrence to the coordinator to get data from

		\return
		bool of if the player enitty is to the left of the entity
		*/
		/************************************************************************/
		bool CheckPlayerIsLeftSide(Entity ownerID, Coordinator& coord);
		/************************************************************************/
		/*!
		\brief
		Function that checks if the player is to the right of the entity

		\param[in] ownerID
		id of the entity that owns the state

		\param[in] coord
		Refrence to the coordinator to get data from

		\return
		bool of if the player enitty is to the right of the entity
		*/
		/************************************************************************/
		bool CheckPlayerIsRightSide(Entity ownerID, Coordinator& coord);

		/************************************************************************/
		/*!
		\brief
		Function that checks if the entity's collider is colliding

		\param[in] ownerID
		id of the entity that owns the state

		\param[in] coord
		Refrence to the coordinator to get data from

		\return
		bool of if entity's collider is colliding
		*/
		/************************************************************************/
		bool EntityIsColliding(Entity ownerID, Coordinator& coord);
		/************************************************************************/
		/*!
		\brief
		Function that checks if the entity's collider is grounded

		\param[in] ownerID
		id of the entity that owns the state

		\param[in] coord
		Refrence to the coordinator to get data from

		\return
		bool of if entity's collider is grounded
		*/
		/************************************************************************/
		bool EntityIsGrounded(Entity ownerID, Coordinator& coord);
	};

	enum STATE_COMPONENT_TYPE
	{
		STATE_COMPONENT_BASE, //Base class no logic or vars
		STATE_COMPONENT_COOLDOWN, 
		STATE_COMPONENT_MOVEMENT, 
	};




	//This is a class instead of a template becuase it needs to udpate and reset its values on enter/exit
	class StateComponent
	{
	public:
		/************************************************************************/
		/*!
		\brief
		Default constructor of the StateComponent class
		*/
		/************************************************************************/
		StateComponent() {};
		/************************************************************************/
		/*!
		\brief
		Virtual destructor of the StateComponent class
		*/
		/************************************************************************/
		virtual ~StateComponent() { 
			//std::cout << "Component Deletd" << std::endl; 
		};

		/************************************************************************/
		/*!
		\brief
		Virtual function that trigger on entering state

		\param[in] idOwner
		id of the owner of the state

		\param[in] coord
		Cordinator refrence
		*/
		/************************************************************************/
		virtual void OnEnter(Entity idOwner, Coordinator& coord) { (void)idOwner; (void)coord; };
		/************************************************************************/
		/*!
		\brief
		Virtual function that trigger on state update

		\param[in] dt
		Deltas time to update by

		\param[in] idOwner
		id of the owner of the state

		\param[in] coord
		Cordinator refrence
		*/
		/************************************************************************/
		virtual void OnUpdate(float dt, Entity idOwner, Coordinator& coord) { (void)idOwner; (void)coord; };
		/************************************************************************/
		/*!
		\brief
		Function that trigger on exiting state

		\param[in] idOwner
		id of the owner of the state

		\param[in] coord
		Cordinator refrence
		*/
		/************************************************************************/
		virtual void OnExit(Entity idOwner, Coordinator& coord) { (void)idOwner; (void)coord; };
		
		//bool TransitionCheck(Entity idOwner, Coordinator& coord) { (void)idOwner; (void)coord; return false; };
	};

	//This class is to store various types (int, bool, structs, etc)
	template<typename T>
	class StateComponentVariable
	{
	private:
		T m_variable;

	public:
		/************************************************************************/
		/*!
		\brief
		Constructor of the StateComponentVariable class

		\param[in] var
		teamplate type T of variable to store
		*/
		/************************************************************************/
		StateComponentVariable(T var);
		/************************************************************************/
		/*!
		\brief
		Destructor of the StateComponentVariable class
		*/
		/************************************************************************/
		~StateComponentVariable();

		/************************************************************************/
		/*!
		\brief
		Obtains the value stored in class

		\return
		Returns the value stored in class
		*/
		/************************************************************************/
		T GetVariable() const;

		/************************************************************************/
		/*!
		\brief
		Obtains the value refrence stored in class

		\return
		Returns the value refrence stored in class
		*/
		/************************************************************************/
		T& GetVariable();

		/************************************************************************/
		/*!
		\brief
		Sets the value stored in the class
		
		\param[in] newVar
		New value to store
		*/
		/************************************************************************/
		void SetVariable(T newVar);
	};

	class StateComponentCountdown : public StateComponent 
	{
	private:
		float m_maxTime;
		float m_currTime;

	public:
		/************************************************************************/
		/*!
		\brief
		Default constructor of the StateComponentCountdown class

		\param[in] maxTime
		Max time of the counter
		*/
		/************************************************************************/
		StateComponentCountdown(float maxTime);
		/************************************************************************/
		/*!
		\brief
		Destructor of the StateComponentCountdown class
		*/
		/************************************************************************/
		~StateComponentCountdown();

		/************************************************************************/
		/*!
		\brief
		Function that trigger on entering state
		Sets the current timer to the max time

		\param[in] idOwner
		id of the owner of the state

		\param[in] coord
		Cordinator refrence
		*/
		/************************************************************************/
		void OnEnter(Entity idOwner, Coordinator& coord) override;
		/************************************************************************/
		/*!
		\brief
		Function that trigger on state update
		Updates timer

		\param[in] dt
		Deltas time to update by

		\param[in] idOwner
		id of the owner of the state

		\param[in] coord
		Cordinator refrence
		*/
		/************************************************************************/
		void OnUpdate(float dt, Entity idOwner, Coordinator& coord) override;
		/************************************************************************/
		/*!
		\brief
		Function that trigger on exiting state

		\param[in] idOwner
		id of the owner of the state

		\param[in] coord
		Cordinator refrence
		*/
		/************************************************************************/
		void OnExit(Entity idOwner, Coordinator& coord) override;

		/************************************************************************/
		/*!
		\brief
		Obtains the current time of the counter

		\return
		Returns the float of the currernt time
		*/
		/************************************************************************/
		float GetCurrTime() const;
	};

	class StateComponentMovement : public StateComponent
	{
	private:
		Vec2 m_spd = Vec2();

	public:
		/************************************************************************/
		/*!
		\brief
		Constructor of the StateComponentMovement class

		\param[in] maxTime
		Max time of the counter
		*/
		/************************************************************************/
		StateComponentMovement(Vec2 spd);
		/************************************************************************/
		/*!
		\brief
		Default constructor of the StateComponentMovement class
		*/
		/************************************************************************/
		StateComponentMovement();


		/************************************************************************/
		/*!
		\brief
		Function that trigger on entering state
		Sets entity speed to stored speed

		\param[in] idOwner
		id of the owner of the state

		\param[in] coor
		Cordinator refrence
		*/
		/************************************************************************/
		void OnEnter(Entity idOwner, Coordinator& coord) override;
		/************************************************************************/
		/*!
		\brief
		Function that trigger on state udpate
		Sets the entity speed to the current stored speed

		\param[in] idOwner
		id of the owner of the state

		\param[in] coor
		Cordinator refrence
		*/
		/************************************************************************/
		void OnUpdate(float dt, Entity idOwner, Coordinator& coord) override;
		/************************************************************************/
		/*!
		\brief
		Function that trigger on exiting state

		\param[in] idOwner
		id of the owner of the state

		\param[in] coor
		Cordinator refrence
		*/
		/************************************************************************/
		void OnExit(Entity idOwner, Coordinator& coord) override;

		/************************************************************************/
		/*!
		\brief
		Gets the speed stored in the class
		
		\return
		Returns Vec2 of the speed stored
		*/
		/************************************************************************/
		Vec2 GetSpd() const;

	};

	//Sets a sudden impulse force to the entity
	class StateComponentImpulse : public StateComponent
	{
	private:
		Vec2 m_impluse = Vec2();

	public:
		/************************************************************************/
		/*!
		\brief
		Default constructor of the StateComponentImpulse class

		\param[in] impulse
		Vec2 of the impulse to set on enter state
		*/
		/************************************************************************/
		StateComponentImpulse(Vec2 impulse);
		/************************************************************************/
		/*!
		\brief
		Default constructor of the StateComponentImpulse class
		*/
		/************************************************************************/
		StateComponentImpulse();

		/************************************************************************/
		/*!
		\brief
		Function that trigger on entering state
		Adds the speed to the entity to the stored impulse

		\param[in] idOwner
		id of the owner of the state

		\param[in] coor
		Cordinator refrence
		*/
		/************************************************************************/
		void OnEnter(Entity idOwner, Coordinator& coord) override;
	};

	/************************************************************************/
	/*!
	\brief
	Creates a shared ptr of a StateComponent based on the name

	\param[in] componentName
	Name of the component to create

	\param[in] paramJson
	Refrence of json to get the component data from

	\return
	Shared ptr of the component created as the ptr of the base class StateComponent
	*/
	/************************************************************************/
	std::shared_ptr<StateComponent> CreateStateComponent(std::string componentName, json& paramJson);


	/************************************************************************/
	/*!
	\brief
	Gets the state tarnsition from the inpuuted name

	\param[in] transitonName
	Name of the transition to return

	\return
	std::function<bool(Entity, Coordinator&)> of the transition obtained 
	*/
	/************************************************************************/
	TransitionFunction GetTransitionFunction(std::string transitonName);

}
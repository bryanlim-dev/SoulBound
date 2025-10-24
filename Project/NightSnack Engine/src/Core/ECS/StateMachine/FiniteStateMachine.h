#pragma once
#include "State.h"
#include "../Component.h"
#include <map>
#include <Core/ECS/Coordinator.h>

namespace NightSnack
{
	//Finite State Machine dosnt belong to a single entity.
	//It takes in the component struct States and a entity ID,
	//then uses the executes the logic for each state within it.
	class FiniteStateMachine : public ISystem
	{
	private:
		std::list<Entity> m_entityList;

		Signature m_systemSignature{};

	public:
		FiniteStateMachine();
		~FiniteStateMachine();

		void Update(float dt, Coordinator& coord) override;

		void AddEntity(Entity id);
		void RemoveEntity(Entity id);

		static States CreateDebugTestStates(Entity entityId) ;


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

	private:
		void UpdateEntityState(float dt, Coordinator& coord, Entity id, const States& states);
	};
}
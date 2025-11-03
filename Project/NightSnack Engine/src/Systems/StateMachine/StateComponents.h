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

			STATE_FUNCTION_ENTITY_COLLIDING

		};

		bool EntityCountdownComponentFinish(Entity ownerID, Coordinator& coord);


		bool CheckPlayerIsLeftSide(Entity ownerID, Coordinator& coord);
		bool CheckPlayerIsRightSide(Entity ownerID, Coordinator& coord);

		bool EntityIsColliding(Entity ownerID, Coordinator& coord);
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
		StateComponent() {};
		virtual ~StateComponent() { std::cout << "Component Deletd" << std::endl; };

		virtual void OnEnter(Entity idOwner, Coordinator& coord) { (void)idOwner; (void)coord; };
		virtual void OnUpdate(float dt, Entity idOwner, Coordinator& coord) { (void)idOwner; (void)coord; };
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
		StateComponentVariable(T var);
		~StateComponentVariable();

		T GetVariable() const;
		T& GetVariable();
		void SetVariable(T newVar);
	};

	class StateComponentCountdown : public StateComponent 
	{
	private:
		float m_maxTime;
		float m_currTime;

	public:
		StateComponentCountdown(float maxTime);
		~StateComponentCountdown();

		void OnEnter(Entity idOwner, Coordinator& coord) override;
		void OnUpdate(float dt, Entity idOwner, Coordinator& coord) override;
		void OnExit(Entity idOwner, Coordinator& coord) override;

		float GetCurrTime() const;
	};

	class StateComponentMovement : public StateComponent
	{
	private:
		Vec2 m_spd = Vec2();

	public:
		StateComponentMovement(Vec2 spd);
		StateComponentMovement();

		void OnEnter(Entity idOwner, Coordinator& coord) override;
		void OnUpdate(float dt, Entity idOwner, Coordinator& coord) override;
		void OnExit(Entity idOwner, Coordinator& coord) override;

		Vec2 GetSpd() const;

	};


	std::shared_ptr<StateComponent> CreateStateComponent(std::string componentName, json& paramJson);
	TransitionFunction GetTransitionFunction(std::string transitonName);

}
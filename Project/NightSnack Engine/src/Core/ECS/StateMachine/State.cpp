#include "State.h"


namespace NightSnack
{
	StateCountDown::StateCountDown(Entity id, std::string stateKeyName, float maxTime)
		:StateBase(id, stateKeyName), m_maxTimer(maxTime), m_currTimer(maxTime)
	{
	}

	void StateCountDown::Enter()
	{
		StateBase::Enter();

		m_currTimer = m_maxTimer;
	}

	void StateCountDown::Update(float dt) 
	{
		StateBase::Update(dt);

		if (!m_isExit)
		{
			m_currTimer -= dt;
			//std::cout << "Counter: " << m_currTimer << std::endl;
			if (m_currTimer <= 0)
			{
				m_isExit = true;
			}
		}
	}

	void StateCountDown::Exit()
	{

	}
}
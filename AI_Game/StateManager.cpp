#include "StateManager.h"
#include "State.h"


StateManager::StateManager()
{
}


StateManager::~StateManager()
{
		m_currentState = nullptr;
}

void StateManager::Update(float _deltaTime)
{
		if (m_currentState)
		{
				m_currentState->Update(_deltaTime);
		}
}

void StateManager::SetState(State * _state)
{
		if (m_currentState)
		{
				delete m_currentState;
		}
		m_currentState = _state;
}

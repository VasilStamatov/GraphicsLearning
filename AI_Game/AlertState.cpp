#include "AlertState.h"
#include "Zombie.h"
#include "PatrolState.h"
#include "ChaseState.h"

AlertState::AlertState(Zombie* _ownerZombie) : ZombieState(_ownerZombie)
{
		m_timer.Start();
}


AlertState::~AlertState()
{
}

void AlertState::Update(float _deltaTime)
{
		if (!CheckForPlayer())
		{
				//player went outside of sight
				m_zombie->m_stateManager.SetState(new PatrolState(m_zombie));
		}
		else if (m_timer.Seconds() >= 3.0f)
		{
				m_zombie->m_stateManager.SetState(new ChaseState(m_zombie));
		}
}
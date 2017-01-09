#include "SmartPatrolState.h"

#include "SmartChaseState.h"
#include "RetreatState.h"
#include "Zombie.h"

SmartPatrolState::SmartPatrolState(Zombie* _ownerZombie) : PatrolState(_ownerZombie)
{
}


SmartPatrolState::~SmartPatrolState()
{
}

void SmartPatrolState::Update(float _deltaTime)
{
		if (!m_zombie->m_pathToTake.empty())
		{
				FollowPath(_deltaTime);
		}
		else
		{
				if (!m_requestedPath)
				{
						FindPath();
				}
		}

		if (m_zombie->m_health <= 50.0f)
		{
				//chase player
				if (!m_requestedPath)
				{
						//only switch states if there is no path requested (or the callback will try to access deleted memory)
						m_zombie->m_stateManager.SetState(new RetreatState(m_zombie));
				}
		}
		else if (CheckForPlayer())
		{
				//chase player
				if (!m_requestedPath)
				{
						//only switch states if there is no path requested (or the callback will try to access deleted memory)
						m_zombie->m_stateManager.SetState(new SmartChaseState(m_zombie));
				}
		}
}

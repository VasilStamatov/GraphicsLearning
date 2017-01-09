#include "RetreatState.h"

#include "Zombie.h"
#include "SmartChaseState.h"
#include "SmartPatrolState.h"

constexpr float healthPerFrame = 1.0f / 60.0f;

RetreatState::RetreatState(Zombie* _ownerZombie) : ZombieState(_ownerZombie)
{
}


RetreatState::~RetreatState()
{
}

void RetreatState::Update(float _deltaTime)
{
		//retreat the zombie in the opposite player direction
		glm::vec2 directionToPlayer = glm::normalize(m_zombie->GetCenterPos() - m_zombie->m_player.lock()->GetCenterPos());
		m_zombie->m_worldPos += m_zombie->m_direction * m_zombie->m_movementSpeed * _deltaTime;

		//regenerate the zombie a bit (1hp/sec)
		m_zombie->m_health += healthPerFrame;

		//if the zombie is healed enough, change the state he's in
		if (m_zombie->m_health >= 100.0f)
		{
				if (CheckForPlayer())
				{
						//if the player is in range, chase him
						m_zombie->m_stateManager.SetState(new SmartChaseState(m_zombie));
				}
				else
				{
						//otherwise, patrol
						m_zombie->m_stateManager.SetState(new SmartPatrolState(m_zombie));
				}
		}
}

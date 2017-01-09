#include "ChaseState.h"
#include "AlertState.h"
#include "Zombie.h"

ChaseState::ChaseState(Zombie* _ownerZombie) : ZombieState(_ownerZombie)
{
}


ChaseState::~ChaseState()
{
}

void ChaseState::Update(float _deltaTime)
{
		//chase the player by just going in its direction
		glm::vec2 directionToPlayer = glm::normalize(m_zombie->m_player.lock()->GetCenterPos() - m_zombie->GetCenterPos());
		m_zombie->m_direction = directionToPlayer;
		m_zombie->m_worldPos += directionToPlayer * m_zombie->m_movementSpeed * _deltaTime;

		if (!m_zombie->CollideWithAgent(m_zombie->m_player.lock().get()))
		{
				//collision handle
		}

		if (!CheckForPlayer())
		{
				m_zombie->m_stateManager.SetState(new AlertState(m_zombie));
		}
}
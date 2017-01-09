#include "ZombieState.h"
#include "Zombie.h"

ZombieState::ZombieState(Zombie * _ownerZombie) : m_zombie(_ownerZombie) { }

bool ZombieState::CheckForPlayer()

{
		constexpr glm::vec2 zombieRange = glm::vec2(AGENT_DIAMETER * 15.0f);

		auto upperBoundary = glm::greaterThanEqual(m_zombie->m_worldPos + zombieRange, m_zombie->m_player.lock()->GetCenterPos());
		auto lowerBoundary = glm::lessThanEqual(m_zombie->m_worldPos - zombieRange, m_zombie->m_player.lock()->GetCenterPos());

		if (upperBoundary.x && lowerBoundary.x && upperBoundary.y && lowerBoundary.y)
		{
				//player in range
				return true;
		}
		return false;
}
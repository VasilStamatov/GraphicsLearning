#include "PatrolState.h"
#include "AlertState.h"
#include "Zombie.h"


#define PENALIZE_COST 25

PatrolState::PatrolState(Zombie * _ownerZombie) : ZombieState(_ownerZombie)
{
		m_rng.GenSeed(GameEngine::SeedType::CLOCK_TICKS);
		m_nextWaypointIndex = m_rng.GenRandInt(0, m_zombie->m_patrolWaypoints.size() - 1);
}

PatrolState::~PatrolState()
{
		m_zombie->m_pathToTake.clear();
}

void PatrolState::Update(float _deltaTime)
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

		if (CheckForPlayer())
		{
				//chase player
				if (!m_requestedPath)
				{
						//only switch states if there is no path requested (or the callback will try to access deleted memory)
						m_zombie->m_stateManager.SetState(new AlertState(m_zombie));
				}
		}
}

void PatrolState::FindPath()
{
		m_zombie->m_prManager.lock()->RequestPath(m_zombie->m_worldPos, m_zombie->m_patrolWaypoints.at(m_nextWaypointIndex),
				m_zombie->m_algoToUse, Diagonal::IFNOWALLS,
				[this](std::vector<glm::vec2>& _path, bool _success)
		{
				if (_success)
				{
						m_zombie->m_pathToTake = _path;
						PenalizePath();
				}
				m_requestedPath = false;
		});
		m_requestedPath = true;
		m_nextWaypointIndex = m_rng.GenRandInt(0, m_zombie->m_patrolWaypoints.size() - 1);
}

void PatrolState::FollowPath(float _deltaTime)
{
		glm::vec2 currentWaypoint = m_zombie->m_pathToTake.back();

		if (m_zombie->m_world.lock()->GetWorldGrid().lock()->GetNodeAt(m_zombie->m_worldPos).lock() ==
				m_zombie->m_world.lock()->GetWorldGrid().lock()->GetNodeAt(m_zombie->m_pathToTake.back()).lock())
		{
				//Remove the penalizing after exiting this waypoint
				std::weak_ptr<Node> nodeToLeave =
						m_zombie->m_world.lock()->GetWorldGrid().lock()->GetNodeAt(m_zombie->m_pathToTake.back());

				nodeToLeave.lock()->terrainCost =
						m_zombie->m_world.lock()->GetTile(nodeToLeave.lock()->nodeIndex.x, nodeToLeave.lock()->nodeIndex.y).lock()->MovementCost();

				m_zombie->m_pathToTake.pop_back();

				if (m_zombie->m_pathToTake.empty())
				{
						m_zombie->m_direction = glm::normalize(currentWaypoint - m_zombie->GetCenterPos());
						m_zombie->m_worldPos += m_zombie->m_direction * m_zombie->m_movementSpeed * _deltaTime;
						//m_zombie->m_stateManager.SetState(new SearchState(m_zombie));
						return;
				}
				currentWaypoint = m_zombie->m_pathToTake.back();
		}
		m_zombie->m_direction = glm::normalize(currentWaypoint - m_zombie->GetCenterPos());
		m_zombie->m_worldPos += m_zombie->m_direction * m_zombie->m_movementSpeed * _deltaTime;
}

void PatrolState::PenalizePath()
{
		for (size_t i = 0; i < m_zombie->m_pathToTake.size(); i++)
		{
				m_zombie->m_world.lock()->GetWorldGrid().lock()->GetNodeAt(m_zombie->m_pathToTake.at(i)).lock()->terrainCost += PENALIZE_COST;
		}
}

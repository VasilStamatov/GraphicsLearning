#include "SmartChaseState.h"

#include "Zombie.h"
#include "SmartPatrolState.h"
#include "RetreatState.h"

#define PENALIZE_COST 25

SmartChaseState::SmartChaseState(Zombie* _ownerZombie) : ChaseState(_ownerZombie)
{
}


SmartChaseState::~SmartChaseState()
{
		m_zombie->m_pathToTake.clear();
}

void SmartChaseState::Update(float _deltaTime)
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

		if (!m_zombie->CollideWithAgent(m_zombie->m_player.lock().get()))
		{
				//collision handle
		}

		if (m_zombie->m_health <= 50.0f)
		{
				//The zombie is low health, he wants to run !
				if (!m_requestedPath)
				{
						//only switch states if there is no path requested (or the callback will try to access deleted memory)
						m_zombie->m_stateManager.SetState(new RetreatState(m_zombie));
				}
		}
		else if (!CheckForPlayer())
		{
				//The player is out of zombie chase range
				if (!m_requestedPath)
				{
						//only switch states if there is no path requested (or the callback will try to access deleted memory)
						m_zombie->m_stateManager.SetState(new SmartPatrolState(m_zombie));
				}
		}
}

void SmartChaseState::FindPath()
{
		m_zombie->m_prManager.lock()->RequestPath(m_zombie->m_worldPos, m_zombie->m_player.lock()->GetCenterPos(),
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
}

void SmartChaseState::FollowPath(float _deltaTime)
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

void SmartChaseState::PenalizePath()
{
		for (size_t i = 0; i < m_zombie->m_pathToTake.size(); i++)
		{
				m_zombie->m_world.lock()->GetWorldGrid().lock()->GetNodeAt(m_zombie->m_pathToTake.at(i)).lock()->terrainCost += PENALIZE_COST;
		}
}

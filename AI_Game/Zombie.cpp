#include "Zombie.h"
#include "AStar.h"
Zombie::Zombie()
{
}

Zombie::Zombie(float _speed, float _health, const glm::vec2 & _startPos, const GameEngine::GLTexture & _texture,
		GameEngine::ColorRGBA8 & _color, std::weak_ptr<Grid> _grid) :
		Agent(_speed, _health, _startPos, _texture, _color, _grid)
{
	 m_finishedPath = true;
		m_pathFinder = std::make_unique<AStar>(m_grid);
}


Zombie::~Zombie()
{
}

void Zombie::Init(float _speed, float _health, const glm::vec2 & _startPos, const GameEngine::GLTexture & _texture,
		GameEngine::ColorRGBA8 & _color, std::weak_ptr<Grid> _grid)
{
		m_movementSpeed = _speed;
		m_health = _health;
		m_worldPos = _startPos;
		m_texture = _texture;
		m_color = _color;
		m_grid = _grid;
		m_finishedPath = true;
		m_pathFinder = std::make_unique<AStar>(m_grid);
}

void Zombie::Update(float _deltaTime)
{
		if (!m_finishedPath && !m_pathToTake.empty())
		{
				FollowPath(_deltaTime);
		}

		// Do collision
		CollideWithLevel();
}

void Zombie::FindPlayer(const glm::vec2& _playerPos)
{
		m_pathToTake = m_pathFinder->FindPath(GetCenterPos(), _playerPos);
		m_finishedPath = false;
		m_waypointIndex = 0;
}

void Zombie::FollowPath(float _deltaTime)
{
		glm::vec2 currentWaypoint = m_pathToTake.at(m_waypointIndex);

		if (m_grid.lock()->GetNodeAt(m_worldPos).lock() == m_grid.lock()->GetNodeAt(m_pathToTake.at(m_waypointIndex)).lock())
		{
				if (++m_waypointIndex >= m_pathToTake.size())
				{
						m_direction = glm::normalize(currentWaypoint - GetCenterPos());
						m_worldPos += m_direction * m_movementSpeed * _deltaTime;
						m_finishedPath = true;
						return;
				}
				currentWaypoint = m_pathToTake.at(m_waypointIndex);
		}
		m_direction = glm::normalize(currentWaypoint - GetCenterPos());
		m_worldPos += m_direction * m_movementSpeed * _deltaTime;
}

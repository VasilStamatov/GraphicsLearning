#include "Zombie.h"
Zombie::Zombie()
{
}

Zombie::Zombie(float _speed, float _health, const glm::vec2 & _startPos, const GameEngine::GLTexture & _texture,
		GameEngine::ColorRGBA8 & _color, std::weak_ptr<World> _world, std::weak_ptr<Player> _player, std::weak_ptr<PathRequestManager> _prManager) :
		Agent(_speed, _health, _startPos, _texture, _color, _world), m_player(_player), m_prManager(_prManager)
{
		m_finishedPath = true;
}


Zombie::~Zombie()
{
		m_pathToTake.clear();
}

void Zombie::Init(float _speed, float _health, const glm::vec2 & _startPos, const GameEngine::GLTexture & _texture,
		GameEngine::ColorRGBA8 & _color, std::weak_ptr<World> _world, std::weak_ptr<Player> _player, std::weak_ptr<PathRequestManager> _prManager)
{
		m_movementSpeed = _speed;
		m_health = _health;
		m_worldPos = _startPos;
		m_texture = _texture;
		m_color = _color;
		m_world = _world;
		m_finishedPath = true;
		m_prManager = _prManager;
		m_player = _player;
}

void Zombie::Update(float _deltaTime)
{
		if (!m_finishedPath && !m_pathToTake.empty())
		{
				FollowPath(_deltaTime);
		}
		else
		{
				if (!m_requestedPath)
				{
						FindPlayer();
				}
		}

		// Do collision
		CollideWithLevel();
}

void Zombie::FindPlayer()
{
		m_prManager.lock()->RequestPath(GetCenterPos(), m_player.lock()->GetCenterPos(), Algorithm::BEST_FIRST, [this](std::vector<glm::vec2>& _path, bool _success)
		{
				if (_success)
				{
						m_pathToTake = _path;
						m_finishedPath = false;
						m_requestedPath = false;
						m_waypointIndex = 0;
				}
		});
		m_requestedPath = true;
}

void Zombie::FollowPath(float _deltaTime)
{
		glm::vec2 currentWaypoint = m_pathToTake.at(m_waypointIndex);

		if (m_world.lock()->GetWorldGrid().lock()->GetNodeAt(m_worldPos).lock() == m_world.lock()->GetWorldGrid().lock()->GetNodeAt(m_pathToTake.at(m_waypointIndex)).lock())
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
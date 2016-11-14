#include "Zombie.h"
Zombie::Zombie()
{
}

Zombie::Zombie(float _speed, float _health, const glm::vec2 & _startPos, const GameEngine::GLTexture & _texture,
		GameEngine::ColorRGBA8 & _color, std::weak_ptr<World> _world, std::weak_ptr<Player> _player, std::weak_ptr<PathRequestManager> _prManager) :
		Agent(_speed, _health, _startPos, _texture, _color, _world), m_player(_player), m_prManager(_prManager)
{
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
		m_prManager = _prManager;
		m_player = _player;
}

void Zombie::Update(float _deltaTime)
{
		if (!CollideWithAgent(m_player.lock().get()))
		{
				if (!m_pathToTake.empty())
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
		}
		// Do collision
		CollideWithLevel();
}

void Zombie::FindPlayer()
{
		m_prManager.lock()->RequestPath(m_worldPos, m_player.lock()->GetCenterPos(),
				Algorithm::ASTAR, Diagonal::IFNOWALLS,
				[this](std::vector<glm::vec2>& _path, bool _success)
		{
				if (_success)
				{
						m_pathToTake = _path;
				}
						m_requestedPath = false;
		});
		m_requestedPath = true;
}

void Zombie::FollowPath(float _deltaTime)
{
		glm::vec2 currentWaypoint = m_pathToTake.back();

		if (m_world.lock()->GetWorldGrid().lock()->GetNodeAt(m_worldPos).lock() == m_world.lock()->GetWorldGrid().lock()->GetNodeAt(m_pathToTake.back()).lock())
		{
				m_pathToTake.pop_back();
				if (m_pathToTake.empty())
				{
						m_direction = glm::normalize(currentWaypoint - GetCenterPos());
						m_worldPos += m_direction * m_movementSpeed * _deltaTime;
						return;
				}
				currentWaypoint = m_pathToTake.back();
		}
		m_direction = glm::normalize(currentWaypoint - GetCenterPos());
		m_worldPos += m_direction * m_movementSpeed * _deltaTime;
}
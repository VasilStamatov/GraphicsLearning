#include "Zombie.h"
#include "PatrolState.h"

Zombie::Zombie()
{
}

Zombie::Zombie(float _speed, float _health, const glm::vec2 & _startPos, const GameEngine::GLTexture & _texture,
		GameEngine::ColorRGBA8 & _color, std::weak_ptr<World> _world, std::weak_ptr<Player> _player,
		std::weak_ptr<PathRequestManager> _prManager, const std::vector<glm::vec2>& _patrolWaypoints) :
		Agent(_speed, _health, _startPos, _texture, _color, _world), m_player(_player), m_prManager(_prManager), m_patrolWaypoints(_patrolWaypoints)
{
		m_algoToUse = Algorithm::ASTAR;
		m_stateManager.SetState(new PatrolState(this));
}


Zombie::~Zombie()
{
		m_pathToTake.clear();
}

void Zombie::Init(float _speed, float _health, const glm::vec2 & _startPos, const GameEngine::GLTexture & _texture,
		GameEngine::ColorRGBA8 & _color, std::weak_ptr<World> _world, std::weak_ptr<Player> _player,
		std::weak_ptr<PathRequestManager> _prManager, const std::vector<glm::vec2>& _patrolWaypoints)
{
		m_movementSpeed = _speed;
		m_health = _health;
		m_worldPos = _startPos;
		m_texture = _texture;
		m_color = _color;
		m_world = _world;
		m_prManager = _prManager;
		m_player = _player;
		m_algoToUse = Algorithm::ASTAR;
		m_patrolWaypoints = _patrolWaypoints;
}

void Zombie::Update(float _deltaTime)
{
		m_stateManager.Update(_deltaTime);
		// Do collision
		CollideWithLevel();
}
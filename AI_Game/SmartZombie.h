#pragma once

#include "Zombie.h"

class SmartZombie : public Zombie
{
public:
		SmartZombie(float _speed, float _health, const glm::vec2& _startPos, const GameEngine::GLTexture& _texture,
				GameEngine::ColorRGBA8& _color, std::weak_ptr<World> _world, std::weak_ptr<Player> _player,
				std::weak_ptr<PathRequestManager> _prManager, const std::vector<glm::vec2>& _patrolWaypoints);
		~SmartZombie();
};


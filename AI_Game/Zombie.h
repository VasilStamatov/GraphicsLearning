#pragma once
#include "Agent.h"
#include "Player.h"
#include "PathRequestManager.h"

class Zombie :	public Agent
{
public:
		Zombie();
		Zombie(float _speed, float _health, const glm::vec2& _startPos, const GameEngine::GLTexture& _texture,
				GameEngine::ColorRGBA8& _color, std::weak_ptr<World> _world, std::weak_ptr<Player> _player, std::weak_ptr<PathRequestManager> _prManager);
		~Zombie();

		void Init(float _speed, float _health, const glm::vec2& _startPos, const GameEngine::GLTexture& _texture,
				GameEngine::ColorRGBA8& _color, std::weak_ptr<World> _world, std::weak_ptr<Player> _player, std::weak_ptr<PathRequestManager> _prManager);

		virtual void Update(float _deltaTime) override;

		void FindPlayer();
		
		const std::vector<glm::vec2>& GetPath() { return m_pathToTake; }

private:
		void FollowPath(float _deltaTime);
private:
		std::weak_ptr<Player> m_player;
		std::weak_ptr<PathRequestManager> m_prManager;
		std::vector<glm::vec2> m_pathToTake;
		bool m_requestedPath{ false };
};


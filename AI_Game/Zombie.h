#pragma once
#include "Agent.h"
#include "PathFinder.h"
#include "Player.h"

class Zombie :	public Agent
{
public:
		Zombie();
		Zombie(float _speed, float _health, const glm::vec2& _startPos, const GameEngine::GLTexture& _texture,
				GameEngine::ColorRGBA8& _color, std::weak_ptr<Grid> _grid);
		~Zombie();

		void Init(float _speed, float _health, const glm::vec2& _startPos, const GameEngine::GLTexture& _texture,
				GameEngine::ColorRGBA8& _color, std::weak_ptr<Grid> _grid);

		virtual void Update(float _deltaTime) override;

		void FindPlayer(const glm::vec2& _playerPos);
		
		const std::vector<glm::vec2>& GetPath() { return m_pathToTake; }

private:
		void FollowPath(float _deltaTime);
private:
		std::unique_ptr<PathFinder> m_pathFinder = nullptr;
		std::vector<glm::vec2> m_pathToTake;
		size_t m_waypointIndex{ 0 };
		bool m_finishedPath{ true };
};


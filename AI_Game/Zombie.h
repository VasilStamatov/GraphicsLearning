#pragma once
#include "Agent.h"
#include "Player.h"
#include "PathRequestManager.h"
#include "StateManager.h"

class Zombie :	public Agent
{
		friend class ZombieState;
		friend class ChaseState;
		friend class PatrolState;
		friend class AlertState;
		friend class SmartPatrolState;
		friend class SmartChaseState;
		friend class RetreatState;

public:
		/** \brief default constructor */
		Zombie();

		/** \brief overloaded constructor */
		Zombie(float _speed, float _health, const glm::vec2& _startPos, const GameEngine::GLTexture& _texture,
				GameEngine::ColorRGBA8& _color, std::weak_ptr<World> _world, std::weak_ptr<Player> _player,
				std::weak_ptr<PathRequestManager> _prManager, const std::vector<glm::vec2>& _patrolWaypoints);

		/** \brief zombie destructor */
		virtual ~Zombie();

		/** \brief Init function for the zombie if he could not be constructed with the overloaded constructor */
		void Init(float _speed, float _health, const glm::vec2& _startPos, const GameEngine::GLTexture& _texture,
				GameEngine::ColorRGBA8& _color, std::weak_ptr<World> _world, std::weak_ptr<Player> _player,
				std::weak_ptr<PathRequestManager> _prManager, const std::vector<glm::vec2>& _patrolWaypoints);

		/** \brief Update function for the zombie */
		virtual void Update(float _deltaTime) override;
		
		/** \brief Sets the pathfinding algorithm to use */
		void SetPFAlgo(const Algorithm& _algo) { m_algoToUse = _algo; }
		/** \brief Gets the pathfinding algorithm in use */
		const Algorithm& GetAlgo() const noexcept { return m_algoToUse; }

		/** \brief Returns the path the zombie is currently following */
		const std::vector<glm::vec2>& GetPath() { return m_pathToTake; }

protected:
		std::weak_ptr<Player> m_player;																///< reference to the player so the zombie can always find him (and collide with him)
		std::weak_ptr<PathRequestManager> m_prManager; ///< reference to the main path request manager in the game screen
		Algorithm m_algoToUse{ Algorithm::ASTAR };					///< the algorithm the zombie will send in his request
		StateManager m_stateManager;																			///< FSM of the zombie
		std::vector<glm::vec2> m_pathToTake;											///< set of world position waypoints the zombie will follow, if any
		std::vector<glm::vec2> m_patrolWaypoints;						///< set of world position waypoints the zombie will follow, if any
};


#pragma once
#include "Agent.h"
#include "Player.h"
#include "PathRequestManager.h"

class Zombie :	public Agent
{
public:
		/** \brief default constructor */
		Zombie();

		/** \brief overloaded constructor */
		Zombie(float _speed, float _health, const glm::vec2& _startPos, const GameEngine::GLTexture& _texture,
				GameEngine::ColorRGBA8& _color, std::weak_ptr<World> _world, std::weak_ptr<Player> _player, std::weak_ptr<PathRequestManager> _prManager);

		/** \brief zombie destructor */
		~Zombie();

		/** \brief Init function for the zombie if he could not be constructed with the overloaded constructor */
		void Init(float _speed, float _health, const glm::vec2& _startPos, const GameEngine::GLTexture& _texture,
				GameEngine::ColorRGBA8& _color, std::weak_ptr<World> _world, std::weak_ptr<Player> _player, std::weak_ptr<PathRequestManager> _prManager);

		/** \brief Update function for the zombie */
		virtual void Update(float _deltaTime) override;

		/** \brief send a path request to the pr manager to find the player */
		void FindPlayer();
		
		/** \brief Sets the pathfinding algorithm to use */
		void SetPFAlgo(const Algorithm& _algo) { m_algoToUse = _algo; }
		/** \brief Gets the pathfinding algorithm in use */
		const Algorithm& GetAlgo() const noexcept { return m_algoToUse; }

		/** \brief Returns the path the zombie is currently following */
		const std::vector<glm::vec2>& GetPath() { return m_pathToTake; }

private:
		/** \brief Makes the zombie follow the waypoints in pathToTake */
		void FollowPath(float _deltaTime);
		/** \brief Penalizes the path the zombie is taking,
		so that other zombies would prefer different paths */
		void PenalizePath();
private:
		std::weak_ptr<Player> m_player;																///< reference to the player so the zombie can always find him (and collide with him)
		std::weak_ptr<PathRequestManager> m_prManager; ///< reference to the main path request manager in the game screen
		Algorithm m_algoToUse{ Algorithm::ASTAR };					///< the algorithm the zombie will send in his request
		std::vector<glm::vec2> m_pathToTake;											///< set of world position waypoints the zombie will follow, if any
		bool m_requestedPath{ false };																 ///< flag whether the zombie has already send a path request manager, in order to avoid request flooding
};


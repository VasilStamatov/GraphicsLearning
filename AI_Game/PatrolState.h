#pragma once

#include "ZombieState.h"

#include <vector>
#include <glm\vec2.hpp>
#include <GameEngine\Random.h>

class PatrolState : public ZombieState
{
public:
		PatrolState(Zombie* _ownerZombie);
		virtual ~PatrolState();

		void Update(float _deltaTime) override;

protected:
		void FindPath();
		void FollowPath(float _deltaTime);
		void PenalizePath();

protected:
		bool m_requestedPath{ false };	///< flag whether the zombie has already send a path request manager, in order to avoid request flooding
		int m_nextWaypointIndex{ 0 };

		GameEngine::Random m_rng;
};


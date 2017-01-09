#pragma once

#include "ChaseState.h"

class SmartChaseState : public ChaseState
{
public:
		SmartChaseState(Zombie* _ownerZombie);
		~SmartChaseState();

		void Update(float _deltaTime) override;
		
private:
		void FindPath();
		void FollowPath(float _deltaTime);
		void PenalizePath();

private:
		bool m_requestedPath{ false };	///< flag whether the zombie has already send a path request manager, in order to avoid request flooding
};


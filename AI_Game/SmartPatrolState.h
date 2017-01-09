#pragma once

#include "PatrolState.h"

class SmartPatrolState : public PatrolState
{
public:
		SmartPatrolState(Zombie* _ownerZombie);
		~SmartPatrolState();

		void Update(float _deltaTime) override;
};


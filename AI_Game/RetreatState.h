#pragma once

#include "ZombieState.h"

class RetreatState : public ZombieState
{
public:
		RetreatState(Zombie* _ownerZombie);
		~RetreatState();

		void Update(float _deltaTime) override;
};


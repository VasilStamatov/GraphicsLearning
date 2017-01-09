#pragma once

#include "ZombieState.h"
#include <vector>
#include <glm\vec2.hpp>

class ChaseState :	public ZombieState
{
public:
		ChaseState(Zombie* _ownerZombie);
		~ChaseState();

		void Update(float _deltaTime) override;
};


#pragma once

#include "ZombieState.h"

#include <GameEngine\Timing.h>

class AlertState :	public ZombieState
{
public:
		AlertState(Zombie* _ownerZombie);
		~AlertState();

		void Update(float _deltaTime) override;

private:
		GameEngine::HRTimer m_timer;
};


#pragma once

#include "State.h"

class Zombie;

class ZombieState : public State
{
public:
		ZombieState(Zombie* _ownerZombie);
		virtual ~ZombieState() { m_zombie = nullptr; }

		virtual void Update(float _deltaTime) = 0;

protected:
		virtual bool CheckForPlayer();

protected:
		Zombie* m_zombie{ nullptr };
};


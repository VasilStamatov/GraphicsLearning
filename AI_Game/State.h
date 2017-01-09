#pragma once

class StateManager;

class State
{
public:
		State() {}
		virtual ~State() {	 }

		virtual void Update(float _deltaTime) = 0;

protected:
};
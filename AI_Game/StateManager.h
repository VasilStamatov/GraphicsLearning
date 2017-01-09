#pragma once

class State;

class StateManager
{
public:
		StateManager();
		~StateManager();

		void Update(float _deltaTime);

		void SetState(State* _state);

private:
		State* m_currentState{ nullptr };
};


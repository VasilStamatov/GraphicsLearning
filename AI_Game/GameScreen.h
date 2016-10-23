#pragma once

#include <GameEngine/IGameScreen.h>
#include <GameEngine\Window.h>

class GameScreen : public GameEngine::IGameScreen
{
public:
		GameScreen(GameEngine::Window* _window);
		~GameScreen();

		virtual int GetNextScreenIndex() const override;

		virtual int GetPreviousScreenIndex() const override;

		virtual void Build() override;

		virtual void Destroy() override;

		virtual void OnEntry() override;

		virtual void OnExit() override;

		virtual void Update() override;

		virtual void Draw() override;
private:
		//Pointer to the window this screen belongs to
		GameEngine::Window* m_window{ nullptr };
};


#pragma once
#include <GameEngine\IMainGame.h>

#include "GameScreen.h"

// The custom app that inherits from IMainGame
class App : public GameEngine::IMainGame
{
public:
		App();
		~App();
		// called on initialization
		virtual void OnInit() override;
		// called when exiting
		virtual void OnExit() override;
		// used to add screens
		virtual void AddScreens() override;

private:
		//unique pointer of all the screens
		std::shared_ptr<GameScreen> m_gameScreen = nullptr;
};
#pragma once

#include <GameEngine/IGameScreen.h>
#include <GameEngine/Window.h>
#include <GameEngine\Camera2D.h>
#include <GameEngine\GLSLProgram.h>
#include <GameEngine\Timing.h>
#include "World.h"
#include "Player.h"
#include "Zombie.h"
#include "PathRequestManager.h"
// Our custom gameplay screen that inherits from the IGameScreen
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
		void CheckInput();

		std::shared_ptr<PathRequestManager> m_pathRequestManger;
		//Pointer to the window this screen belongs to
		GameEngine::Window* m_window{ nullptr };

		//the main game camera
		GameEngine::Camera2D m_camera;

		//The player
		std::shared_ptr<Player> m_player;
		std::vector<std::unique_ptr<Zombie>> m_zombies;
		//The spritebatch for batched rendering
		GameEngine::SpriteBatch m_spriteBatch;

		//shader
		GameEngine::GLSLProgram m_shader;

		//Timer
		GameEngine::HRTimer m_timer;

		//vector of all levels
		std::vector<std::shared_ptr<World>> m_gameWorlds;
		int m_currentLevel{ -1 };
};
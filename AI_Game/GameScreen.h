#pragma once

#include <GameEngine/IGameScreen.h>
#include <GameEngine/Window.h>
#include <GameEngine\Camera2D.h>
#include <GameEngine\GLSLProgram.h>
#include <GameEngine\Timing.h>
#include <GameEngine\Random.h>
#include "World.h"
#include "Player.h"
#include "Zombie.h"
#include "PathRequestManager.h"

// Our custom gameplay screen that inherits from IGameScreen
class GameScreen : public GameEngine::IGameScreen
{
public:
		GameScreen(GameEngine::Window* _window);
		~GameScreen();

		/** \brief Returns the next screen index (returns none if there is no next one) */
		virtual int GetNextScreenIndex() const override;

		/** \brief Returns the previous screen index (returns none if there is no next one) */
		virtual int GetPreviousScreenIndex() const override;

		/** \brief function called on scene build (when it's first allocated) */
		virtual void Build() override;

		/** \brief function called on scene destruction (when it's de-allocated) */
		virtual void Destroy() override;

		/** \brief function called on scene entry */
		virtual void OnEntry() override;

		/** \brief function called on scene exit */
		virtual void OnExit() override;

		/** \brief function called on every frame to update the current screen */
		virtual void Update() override;

		/** \brief function called every frame to draw the current screen */
		virtual void Draw() override;
private:
		/** \brief Check the user input from the event poll */
		void CheckInput();

		/** \brief function called every frame to draw the UI
		 * \param  _shader - shader to use */
		void DrawUI(GameEngine::GLSLProgram& _shader);

private:
		std::shared_ptr<PathRequestManager> m_pathRequestManger; ///< the path request manager for pathfinding
		
		GameEngine::Window* m_window{ nullptr }; ///< Pointer to the window this screen belongs to

		GameEngine::Camera2D m_camera;				///< the main game camera
		GameEngine::Camera2D m_hudCamera; ///the camera for UI

		std::shared_ptr<Player> m_player;															///< the Player
		std::vector<std::unique_ptr<Zombie>> m_zombies; ///< The set of zombies

		GameEngine::SpriteBatch m_spriteBatch; ///< The spritebatch for batched rendering for agents
		GameEngine::SpriteBatch m_hudSpriteBatch; ///< The spritebatch for batched rendering for UI		

		GameEngine::SpriteFont m_spriteFont; ///< Spritefont for rendering text
		std::string m_currentAlgo = "ASTAR"; ///< the name of the current algorithm used

		GameEngine::GLSLProgram m_shader; ///< GLSL shader program

		GameEngine::HRTimer m_timer; ///< high-resolution timer

		std::vector<std::shared_ptr<World>> m_gameWorlds; ///< set of all the game worlds/levels (only 1 currently)
		int m_currentLevel{ -1 }; ///< index of the current level

		bool m_debugMode{ false }; ///< flag whether to have debug code on or off

		GameEngine::Random m_random; ///< random number generator
};
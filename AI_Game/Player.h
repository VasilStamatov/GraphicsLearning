#pragma once
#include "Agent.h"

#include <GameEngine\InputManager.h>
#include <GameEngine\Camera2D.h>

class Player : public Agent
{
public:
		/** \brief default constructor */
		Player();

		/** \brief overloaded constructor */
		Player(float _speed, float _health, const glm::vec2& _startPos, GameEngine::InputManager* _inputManager,
				GameEngine::Camera2D* _camera, const GameEngine::GLTexture& _texture, GameEngine::ColorRGBA8& _color, std::weak_ptr<World> _world);

		/** \brief player destructor */
		~Player();

		/** \brief Init function for the player if he could not be constructed with the overloaded constructor */
		void Init(float _speed, float _health, const glm::vec2& _startPos, GameEngine::InputManager* _inputManager,
				GameEngine::Camera2D* _camera, const GameEngine::GLTexture& _texture, GameEngine::ColorRGBA8& _color, std::weak_ptr<World> _world);

		/** \brief Update function for the player */
		virtual void Update(float _deltaTime) override;

private:
		GameEngine::InputManager* m_inputManager = nullptr; ///< input manager for the user inputs
		GameEngine::Camera2D* m_camera = nullptr; ///< camera reference to get the direction to the mouse cursor
};


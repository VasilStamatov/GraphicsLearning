#pragma once
#include "Agent.h"

#include <GameEngine\InputManager.h>
#include <GameEngine\Camera2D.h>

class Player : public Agent
{
public:
		Player();
		Player(float _speed, float _health, const glm::vec2& _startPos, GameEngine::InputManager* _inputManager,
				GameEngine::Camera2D* _camera, const GameEngine::GLTexture& _texture, GameEngine::ColorRGBA8& _color, std::weak_ptr<Grid> _grid);
		~Player();

		void Init(float _speed, float _health, const glm::vec2& _startPos, GameEngine::InputManager* _inputManager,
				GameEngine::Camera2D* _camera, const GameEngine::GLTexture& _texture, GameEngine::ColorRGBA8& _color, std::weak_ptr<Grid> _grid);

		virtual void Update(float _deltaTime) override;

private:
		GameEngine::InputManager* m_inputManager = nullptr;
		GameEngine::Camera2D* m_camera = nullptr;
};


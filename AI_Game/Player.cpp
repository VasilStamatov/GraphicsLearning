#include "Player.h"



Player::Player(float _speed, float _health, const glm::vec2& _startPos, GameEngine::InputManager* _inputManager,
		GameEngine::Camera2D* _camera, const GameEngine::GLTexture& _texture, GameEngine::ColorRGBA8& _color, std::weak_ptr<Grid> _grid) :
		Agent(_speed, _health, _startPos, _texture, _color, _grid), m_inputManager(_inputManager), m_camera(_camera)
{
}


Player::Player()
{
}

Player::~Player()
{
		m_camera = nullptr;
		m_inputManager = nullptr;
}

void Player::Init(float _speed, float _health, const glm::vec2 & _startPos, GameEngine::InputManager * _inputManager,
		GameEngine::Camera2D * _camera, const GameEngine::GLTexture& _texture, GameEngine::ColorRGBA8& _color, std::weak_ptr<Grid> _grid)
{
		m_movementSpeed = _speed;
		m_health = _health;
		m_worldPos = _startPos;
		m_inputManager = _inputManager;
		m_camera = _camera;
		m_texture = _texture;
		m_color = _color;
		m_grid = _grid;
}

void Player::Update(float _deltaTime)
{
		if (m_inputManager->IsKeyDown(SDLK_w)) 
		{
				m_worldPos.y += m_movementSpeed * _deltaTime;
		}
		else if (m_inputManager->IsKeyDown(SDLK_s))
		{
				m_worldPos.y -= m_movementSpeed * _deltaTime;
		}

		if (m_inputManager->IsKeyDown(SDLK_a))
		{
				m_worldPos.x -= m_movementSpeed * _deltaTime;
		}
		else if (m_inputManager->IsKeyDown(SDLK_d))
		{
				m_worldPos.x += m_movementSpeed * _deltaTime;
		}

		glm::vec2 mouseCoords = m_inputManager->GetMouseCoords();
		mouseCoords = m_camera->ConvertScreenToWorld(mouseCoords);


		glm::vec2 centerPosition = m_worldPos + glm::vec2(AGENT_RADIUS);

		m_direction = glm::normalize(mouseCoords - centerPosition);

		CollideWithLevel();
}

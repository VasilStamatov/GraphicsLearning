#ifndef _PLAYER_
#define _PLAYER_

#include "Entity.h"

#include <GameEngine/InputManager.h>
#include <GameEngine\AudioEngine.h>

enum class PlayerMoveState { STANDING, RUNNING, JUMPING, DYING };

class Player : public Entity
{
public:
  //Initialize the player with the custom init function
  virtual void Init(b2World* _world,
        const glm::vec2& _position,
        const glm::vec2& _drawDims,
        glm::vec2& _collisionDims,
        GameEngine::ColorRGBA8 _color) override;
  //Draw the player
  virtual void Draw(GameEngine::SpriteBatch& _spriteBatch) override;
  //Update the player
  void Update(GameEngine::InputManager& _inputManager);

  // returns true if the player shot
  bool GetPlayerShot() { return m_playerShot; }
  //set the player shot bool to true/false
  void SetPlayerShot(bool _setShot) { m_playerShot = _setShot; }
  void IncrementShots() { m_maxShots++; }

private:
  GameEngine::AudioEngine m_audio;
  PlayerMoveState m_moveState = PlayerMoveState::STANDING;
  bool m_onGround = false;
  bool m_playerShot = false;
  int m_maxShots = 3;
};

#endif
#ifndef _ENEMYROBOT_
#define _ENEMYROBOT_

#include "Entity.h"
#include "Player.h"

enum class RobotState { IDLEORWALK, PUNCHING, SPAWNING, DYING };

class EnemyRobot : public Entity
{
public:
  //Initialize the enemy
  virtual void Init(b2World* _world, const glm::vec2& _pos, const glm::vec2& _drawDims, glm::vec2& _collisionDims, GameEngine::ColorRGBA8 _color) override;
  //Draw the enemy
  virtual void Draw(GameEngine::SpriteBatch& _spriteBatch) override;
  //Update him every frame
  void Update(const glm::vec2& _playerPos);

  const bool GetSpawn() const { return m_hasSpawned; }
private:
  RobotState m_state = RobotState::SPAWNING;
  bool m_hasSpawned = false;
  bool m_isTriggered = false;
  bool m_isPunching = false;
};

#endif
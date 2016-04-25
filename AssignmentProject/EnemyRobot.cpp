#include "EnemyRobot.h"
#include "GameStats.h"
#include <GameEngine\ResourceManager.h>

void EnemyRobot::Init(b2World* _world, const glm::vec2& _pos, const glm::vec2& _drawDims, glm::vec2& _collisionDims, GameEngine::ColorRGBA8 _color)
{
  //set up the member variables and initialize the body (capsule) and the starting texture
  GameEngine::GLTexture texture = GameEngine::ResourceManager::GetTexture("Assets/EnemyRobot/appear/appear_1.png");
  m_color = _color;
  m_collisionDims = _collisionDims;
  m_drawDims = _drawDims;
  m_capsule.Init(_world, _pos, _collisionDims, 1.0f, 0.1f, true);
  m_texture.Init(texture, glm::ivec2(1, 1));
}

void EnemyRobot::Draw(GameEngine::SpriteBatch& _spriteBatch)
{
  glm::vec4 destRect;
  b2Body* body = m_capsule.GetBody();
  destRect.x = body->GetPosition().x - m_drawDims.x / 2.0f;
  destRect.y = body->GetPosition().y - m_capsule.GetDimensions().y / 2.0f;
  destRect.z = m_drawDims.x;
  destRect.w = m_drawDims.y;

  int tileIndex = 0;
  int numTiles = 0;

  float animSpeed = 0.2f;
  glm::vec2 velocity;
  velocity.x = body->GetLinearVelocity().x;
  velocity.y = body->GetLinearVelocity().y;

  //do the animation calculations
  if (m_isTriggered && !m_hasSpawned)
  {
    GameEngine::GLTexture texture = GameEngine::ResourceManager::GetTexture("Assets/EnemyRobot/appear/SPAWN.png");
    m_texture.Init(texture, glm::ivec2(15, 1));

    //spawn the enemy
    numTiles = 15;
    tileIndex = 0;
    if (m_state != RobotState::SPAWNING)
    {
      m_state = RobotState::SPAWNING;
      m_animTime = 0.0f;
    }
  }
  else if (m_hasSpawned)
  {
    //it spawned and it's either moving or idle (same animation for both)
    if (m_healthPoints <= 0.0f)
    {
      GameEngine::GLTexture texture = GameEngine::ResourceManager::GetTexture("Assets/EnemyRobot/die/DIE.png");
      m_texture.Init(texture, glm::ivec2(7, 1));

      numTiles = 7;
      tileIndex = 0;
      if (m_state != RobotState::DYING)
      {
        m_state = RobotState::DYING;
        m_animTime = 0.0f;
      }
    }
    else
    {
      if (m_isPunching)
      {
        GameEngine::GLTexture texture = GameEngine::ResourceManager::GetTexture("Assets/EnemyRobot/attack/ATTACK.png");
        m_texture.Init(texture, glm::ivec2(6, 1));
        //he's punching
        numTiles = 6;
        tileIndex = 0;
        if (m_state != RobotState::PUNCHING)
        {
          m_state = RobotState::PUNCHING;
          m_animTime = 0.0f;
        }
        if (m_animTime > numTiles)
        {
          m_isPunching = false;
        }
      }
      else
      {
        GameEngine::GLTexture texture = GameEngine::ResourceManager::GetTexture("Assets/EnemyRobot/idle-walk/IDLEE.png");
        m_texture.Init(texture, glm::ivec2(6, 1));
        //he's idling or walking (same stuff)
        numTiles = 6;
        tileIndex = 0;
        if (velocity.x != 0.0f)
        {
          // make sure not to multiply by zero!
          animSpeed = abs(velocity.x * 0.025f);
        }
        if (m_state != RobotState::IDLEORWALK)
        {
          m_state = RobotState::IDLEORWALK;
          m_animTime = 0.0f;
        }
      }
    }
  }
  else
  {
    //not triggered and not spawned
    numTiles = 1;
    tileIndex = 1;
  }
  //increment animation time
  m_animTime += animSpeed;

  //check for spawn end
  if (m_animTime > numTiles && m_state == RobotState::SPAWNING)
  {
    m_hasSpawned = true;
  }
  //check for death end
  else if (m_animTime > numTiles && m_state == RobotState::DYING)
  {
    m_isDead = true;
    m_enemiesKilled++;
  }
  //check for punch end
  else if (m_animTime > numTiles && m_state == RobotState::PUNCHING)
  {
    m_isPunching = false;
  }
  //apply animation
  tileIndex = tileIndex + (int)m_animTime % numTiles;

  //get the uv coords from the tile index
  glm::vec4 uvRect;

  uvRect = m_texture.GetUVs(tileIndex);


  //check direction
  if (m_direction == -1)
  {
    //flip it
    uvRect.x += 1.0f / m_texture.dims.x;
    uvRect.z *= -1;
  }
  //draw the sprite
  _spriteBatch.Draw(destRect, uvRect, m_texture.texture.id, 0.0f, m_color, body->GetAngle());

}

void EnemyRobot::Update(const glm::vec2& _playerPos)
{
  b2Body* body = m_capsule.GetBody();

  //check if the player is close to the enemy to trigger him
  if ((body->GetPosition().x - _playerPos.x <= 10.0f) && (body->GetPosition().x - _playerPos.x >= -10.0f))
  {
    if (m_isTriggered == false)
    {
      m_isTriggered = true;
    }
  }
  //check if the enemy has already spawned
  if (m_hasSpawned)
  {
    //check if the player is close to the enemy
    if ((body->GetPosition().x - _playerPos.x <= 17.0f) && (body->GetPosition().x - _playerPos.x >= 0.0f))
    {
      // walk left
      body->ApplyForceToCenter(b2Vec2(-50.0f, 0.0f), true);
      //check if it's in punching range
      if ((body->GetPosition().x - _playerPos.x <= 2.0f) && (body->GetPosition().x - _playerPos.x >= 0.0f))
      {
        m_isPunching = true;
      }
      m_direction = 1;
    }
    else if ((body->GetPosition().x - _playerPos.x <= -0.5f) && (body->GetPosition().x - _playerPos.x >= -17.0f))
    {
      //walk right
      body->ApplyForceToCenter(b2Vec2(50.0f, 0.0f), true);
      //check if it's in punching range
      if ((body->GetPosition().x - _playerPos.x <= -0.5f) && (body->GetPosition().x - _playerPos.x >= -2.5f))
      {
        m_isPunching = true;
      }
      m_direction = -1;
    }
    else
    {
      //drifting
      body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x * 0.95f, body->GetLinearVelocity().y));
    }
    //constrol the max speed possible
    const float MAX_SPEED = 9.0f;
    if (body->GetLinearVelocity().x < -MAX_SPEED)
    {
      body->SetLinearVelocity(b2Vec2(-MAX_SPEED, body->GetLinearVelocity().y));
    }
    else if (body->GetLinearVelocity().x > MAX_SPEED)
    {
      body->SetLinearVelocity(b2Vec2(MAX_SPEED, body->GetLinearVelocity().y));
    }
  }
}

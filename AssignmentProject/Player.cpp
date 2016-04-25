#include "Player.h"
#include "Box.h"
#include <GameEngine/ResourceManager.h>
#include <GameEngine\GameEngineErrors.h>
#include <SDL/SDL.h>

void Player::Init(b2World* _world,
  const glm::vec2& _position,
  const glm::vec2& _drawDims,
  glm::vec2& _collisionDims,
  GameEngine::ColorRGBA8 _color)
{
  //set the member variables and initialize the capsule and texture
  GameEngine::GLTexture texture = GameEngine::ResourceManager::GetTexture("Assets/Char2/IDLE.png");
  m_color = _color;
  m_drawDims = _drawDims;
  m_collisionDims = _collisionDims;
  m_capsule.Init(_world, _position, _collisionDims, 1.0f, 0.1f, true);
  m_texture.Init(texture, glm::ivec2(10, 1));
  //hardcode some damage so it's not the default
  m_attackDamage = 0.50f;
}

void Player::Draw(GameEngine::SpriteBatch& _spriteBatch)
{
  glm::vec4 destRect;
  //set the player destination rectangle
  b2Body* body = m_capsule.GetBody();
  destRect.x = body->GetPosition().x - m_drawDims.x / 2.0f;
  destRect.y = body->GetPosition().y - m_capsule.GetDimensions().y / 2.0f;
  destRect.z = m_drawDims.x;
  destRect.w = m_drawDims.y;

  int tileIndex;
  int numTiles;

  float animSpeed = 0.2f;
  glm::vec2 velocity;
  velocity.x = body->GetLinearVelocity().x;
  velocity.y = body->GetLinearVelocity().y;

  //Calculate animation
  if (m_healthPoints <= 0.0f)
  {
    //dying
    GameEngine::GLTexture texture = GameEngine::ResourceManager::GetTexture("Assets/Char2/DYING.png");
    m_texture.Init(texture, glm::ivec2(10, 1));
    numTiles = 10;
    tileIndex = 0;
    if (m_moveState != PlayerMoveState::DYING)
    {
      m_moveState = PlayerMoveState::DYING;
      m_animTime = 0.0f;
    }
  }
  else
  {
    if (m_onGround)
    {
      if (abs(velocity.x) > 1.0f && ((velocity.x > 0 && m_direction > 0) || (velocity.x < 0 && m_direction < 0)))
      {
        //// is moving
        GameEngine::GLTexture texture = GameEngine::ResourceManager::GetTexture("Assets/Char2/RUN.png");
        m_texture.Init(texture, glm::ivec2(8, 1));
        numTiles = 8;
        tileIndex = 0;
        animSpeed = abs(velocity.x * 0.025f);
        if (m_moveState != PlayerMoveState::RUNNING)
        {
          m_moveState = PlayerMoveState::RUNNING;
          m_animTime = 0.0f;
        }
      }
      else
      {
        //standing
        GameEngine::GLTexture texture = GameEngine::ResourceManager::GetTexture("Assets/Char2/IDLE.png");
        m_texture.Init(texture, glm::ivec2(10, 1));
        numTiles = 10;
        tileIndex = 0;
        m_moveState = PlayerMoveState::STANDING;
      }
    }
    else
    {
      //jumping
      GameEngine::GLTexture texture = GameEngine::ResourceManager::GetTexture("Assets/Char2/JUMP.png");
      m_texture.Init(texture, glm::ivec2(10, 1));
      if (velocity.y < 0.0f)
      {
        //falling
        numTiles = 1;
        tileIndex = 7;
        m_moveState = PlayerMoveState::JUMPING;
      }
      else
      {
        //rising
        numTiles = 1;
        tileIndex = 2;
        m_moveState = PlayerMoveState::JUMPING;
      }
    }
  }
  //increment animation time
  m_animTime += animSpeed;

  // check for death

  if (m_animTime > numTiles && m_moveState == PlayerMoveState::DYING)
  {
    m_isDead = true;
  }

  //apply animation
  tileIndex = tileIndex + (int)m_animTime % numTiles;

  //get the uv coordinates from the tile index
  glm::vec4 uvRect = m_texture.GetUVs(tileIndex);

  //check direction
  if (m_direction == -1)
  {
    uvRect.x += 1.0f / m_texture.dims.x;
    uvRect.z *= -1;
  }

  //Draw the sprite
  _spriteBatch.Draw(destRect, uvRect, m_texture.texture.id, 1.0f, m_color, body->GetAngle());
}

void Player::Update(GameEngine::InputManager& _inputManager)
{
  //check if the player is alive
  if (m_healthPoints > 0.0f)
  {
    b2Body* body = m_capsule.GetBody();
    if (_inputManager.IsKeyPressed(SDLK_SPACE) && m_maxShots > 0)
    {
      //player shoot
      m_playerShot = true;
      m_maxShots--;
      //shooting sound
      GameEngine::SoundEffect soundEffect = m_audio.LoadSoundEffect("Sound/Laser_Shoot6.ogg");
      soundEffect.Play();
    }
    if (_inputManager.IsKeyDown(SDLK_a))
    {
      //walking left
      body->ApplyForceToCenter(b2Vec2(-100.0f, 0.0f), true);
      m_direction = -1;
    }
    else if (_inputManager.IsKeyDown(SDLK_d))
    {
      //walking right
      body->ApplyForceToCenter(b2Vec2(100.0f, 0.0f), true);
      m_direction = 1;
    }
    else
    {
      //drifting
      body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x * 0.95f, body->GetLinearVelocity().y));
    }

    //Control the max possible speed
    const float MAX_SPEED = 10.0f;
    if (body->GetLinearVelocity().x < -MAX_SPEED)
    {
      body->SetLinearVelocity(b2Vec2(-MAX_SPEED, body->GetLinearVelocity().y));
    }
    else if (body->GetLinearVelocity().x > MAX_SPEED)
    {
      body->SetLinearVelocity(b2Vec2(MAX_SPEED, body->GetLinearVelocity().y));
    }

    //loop through all contact points
    m_onGround = false;
    for (b2ContactEdge* ce = body->GetContactList(); ce != nullptr; ce = ce->next)
    {
      b2Contact* c = ce->contact;
      if (c->IsTouching())
      {
        b2WorldManifold manifold;
        c->GetWorldManifold(&manifold);

        //check if the points are below
        bool below = false;
        for (int i = 0; i < b2_maxManifoldPoints; i++)
        {
          if (manifold.points[i].y < body->GetPosition().y - m_capsule.GetDimensions().y / 2.0f + m_capsule.GetDimensions().x / 2.0f + 0.01f)
          {
            below = true;
            break;
          }
        }
        if (below)
        {
          m_onGround = true;
          //player can jump
          if (_inputManager.IsKeyPressed(SDLK_w))
          {
            GameEngine::SoundEffect soundEffect = m_audio.LoadSoundEffect("Sound/Jump17.ogg");
            soundEffect.Play();
            body->ApplyLinearImpulse(b2Vec2(0.0f, 30.0f), b2Vec2(0.0f, 0.0f), true);
            break;
          }
        }
      }
    }
  }
}

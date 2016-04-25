#include "Projectile.h"
#include <GameEngine/ResourceManager.h>


Projectile::Projectile(b2World* _world, const glm::vec2& _pos, const glm::vec2& _drawDims, glm::vec2& _collisionDims,
  GameEngine::ColorRGBA8 _color, int _direction, float _speed)
{
  //Initialize the projectile's body and texture and set the member vars
  GameEngine::GLTexture texture = GameEngine::ResourceManager::GetTexture("Assets/Char2/BULLET.png");
  m_color = _color;
  m_drawDims = _drawDims;
  m_collisionDims = _collisionDims;
  m_direction = _direction;
  m_travelSpeed = _speed;
  m_bulletBody.Init(_world, _pos, _collisionDims, 0.01f, 0.1f);
  m_texture.Init(texture, glm::ivec2(5, 1));
}
Projectile::~Projectile()
{

}

void Projectile::Draw(GameEngine::SpriteBatch& _spriteBatch)
{
  glm::vec4 destRect;
  b2Body* body = m_bulletBody.GetBody();
  //Set up the destRect
  destRect.x = body->GetPosition().x - m_drawDims.x / 2.0f;
  destRect.y = body->GetPosition().y - m_bulletBody.GetDimensions().y / 2.0f;
  destRect.z = m_drawDims.x;
  destRect.w = m_drawDims.y;

  int tileIndex;
  int numTiles;

  float animSpeed = 0.2f;
  glm::vec2 velocity;
  velocity.x = body->GetLinearVelocity().x;
  velocity.y = body->GetLinearVelocity().y;

  //Calculate animation
  if (m_hasCollided)
  {
    //When the projectile collides
    GameEngine::GLTexture texture = GameEngine::ResourceManager::GetTexture("Assets/Char2/MUZZLE.png");
    m_texture.Init(texture, glm::ivec2(5, 1));

    numTiles = 5;
    tileIndex = 0;

    if (m_bulletState != BulletState::COLLIDED)
    {
      m_bulletState = BulletState::COLLIDED;
      m_animTime = 0.0f;
    }
  }
  else
  {
    //When the projectile is travelling
    numTiles = 5;
    tileIndex = 0;
    if (m_bulletState != BulletState::TRAVELLING)
    {
      m_bulletState = BulletState::TRAVELLING;
      m_animTime = 0.0f;
    }
  }
  //increment animation time
  m_animTime += animSpeed;

  // check for death

  if (m_animTime > numTiles && m_bulletState == BulletState::COLLIDED)
  {
    m_isDestroyed = true;
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
  _spriteBatch.Draw(destRect, uvRect, m_texture.texture.id, 0.0f, m_color, body->GetAngle());
}


void Projectile::Destroy(b2World* _world)
{
  //Destroy the body
  m_bulletBody.Destroy(_world);
}

void Projectile::DrawDebug(GameEngine::DebugRenderer& _debugRenderer)
{
  //Draw debug
  m_bulletBody.DrawDebug(_debugRenderer);
}

void Projectile::Update()
{
  b2Body* body = m_bulletBody.GetBody();
  //Apply linear impulse,, because it's a projectile
  body->ApplyLinearImpulse(b2Vec2(0.008f * m_direction, 0.0f), b2Vec2(body->GetWorldCenter().x, body->GetWorldCenter().y), true);
  //Check for collision
  for (b2ContactEdge* ce = body->GetContactList(); ce != nullptr; ce = ce->next)
  {
    b2Contact* c = ce->contact;
    if (c->IsTouching())
    {
      m_hasCollided = true;
    }
  }
  /* Check if it still has lifetime and if it does decrement it by 1
     (makes the projectile be alive for only x amount of frames if it doesn't collide ) */
  if (m_lifeTime == 0)
  {
    m_hasCollided = true;
  }
  else
  {
    m_lifeTime--;
  }
}


#ifndef _PROJECTILE_
#define _PROJECTILE_

#include "BulletBody.h"
#include <GameEngine/SpriteBatch.h>
#include <GameEngine/TileSheet.h>

enum class BulletState { TRAVELLING, COLLIDED };

class Projectile
{
public:
  //Constructor
  Projectile(b2World* _world, const glm::vec2& _pos, const glm::vec2& _drawDims,
    glm::vec2& _collisionDims, GameEngine::ColorRGBA8 _color, int _direction, float _speed);
  ~Projectile();
  //Draw it every frame
  void Draw(GameEngine::SpriteBatch& _spriteBatch);
  //Destroy it
  void Destroy(b2World* _world);
  //Show collision outlines
  void DrawDebug(GameEngine::DebugRenderer& _debugRenderer);
  //Update it
  void Update();

  //Getters
  glm::vec2 GetPosition() const
  {
    glm::vec2 rv;
    rv.x = m_bulletBody.GetBody()->GetPosition().x;
    rv.y = m_bulletBody.GetBody()->GetPosition().y;
    return rv;
  }

  bool GetIsDestroyed()                    { return m_isDestroyed; }
  const BulletBody GetBulletBody()         const { return m_bulletBody; }
  const glm::vec2& GetDrawDims()           const { return m_drawDims; }
  const glm::vec2& GetColDims()            const { return m_collisionDims; }

private:
  glm::vec2 m_drawDims;
  glm::vec2 m_collisionDims;
  GameEngine::ColorRGBA8 m_color;
  GameEngine::TileSheet m_texture;
  BulletBody m_bulletBody;
  float m_animTime = 0.0f;
  float m_travelSpeed = 0.0f;
  BulletState m_bulletState = BulletState::TRAVELLING;
  bool m_hasCollided = false;
  bool m_isDestroyed = false;
  //number of frames the projectile can live in
  int m_lifeTime = 180; // 3 seconds in 60 fps
  int m_direction;
};

#endif
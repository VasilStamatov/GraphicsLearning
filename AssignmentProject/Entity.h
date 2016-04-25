#ifndef _ENTITY_
#define _ENTITY_

#include "Capsule.h"
#include <GameEngine/SpriteBatch.h>
#include <GameEngine/TileSheet.h>

//Base class for all entities (player and NPC)
class Entity
{
public:
  Entity() {/*empty*/}
  virtual ~Entity() {/*Empty*/}

  //The functions an entity would need (Update would also be here, but the player and Enemy take different parameters)
  virtual void Init(b2World* _world, const glm::vec2& _pos, const glm::vec2& _drawDims, glm::vec2& _collisionDims, GameEngine::ColorRGBA8 _color) = 0;
  virtual void Draw(GameEngine::SpriteBatch& _spriteBatch) = 0;
  virtual void Destroy(b2World* _world);
  virtual void DrawDebug(GameEngine::DebugRenderer& _debugRenderer);

  //Accessors
  glm::vec2 GetPosition() const
  {
    glm::vec2 rv;
    rv.x = m_capsule.GetBody()->GetPosition().x;
    rv.y = m_capsule.GetBody()->GetPosition().y;
    return rv;
  }
  const Capsule GetCapsule()               const { return m_capsule; }
  const glm::vec2& GetDrawDims()           const { return m_drawDims; }
  const glm::vec2& GetColDims()            const { return m_collisionDims; }
  const GameEngine::ColorRGBA8& GetColor() const { return m_color; }
  bool  GetIsDead()                        const { return m_isDead; }
  int   GetDirection()                     const { return m_direction; }
  float GetHP()                            const { return m_healthPoints; }
  float GetDmg()                           const { return m_attackDamage; }

  //modifiers
  void SetHealth(float _newHealth)    { m_healthPoints = _newHealth; }
  void SetIsDead(bool _isDead)        { m_isDead = _isDead; }
  void SetAttackDamage(float _damage) { m_attackDamage = _damage; }

  //apply damage to the entity
  void ApplyDamage(float _damage) { m_healthPoints -= _damage; }

protected:
  //stuff that every entity should have
  glm::vec2 m_drawDims;
  glm::vec2 m_collisionDims;
  GameEngine::ColorRGBA8 m_color;
  GameEngine::TileSheet m_texture;
  Capsule m_capsule;
  float m_animTime = 0.0f;
  float m_healthPoints = 100.0f;
  float m_attackDamage = 0.25f;
  int m_direction = 1; // 1 or -1
  bool m_isDead = false;
};

#endif

#ifndef _BOX_
#define _BOX_

#include <Box2D/Box2D.h>
#include <glm/glm.hpp>
#include <GameEngine/Vertex.h>
#include <GameEngine/SpriteBatch.h>
#include <GameEngine/GLTexture.h>

class Box
{
public:
  Box();
  ~Box();
  //INitialize a box
  void Init(b2World* _world,
    const glm::vec2& _position,
    const glm::vec2& _dimensions,
    GameEngine::GLTexture _texture,
    GameEngine::ColorRGBA8 _color,
    bool _fixedRotation,
    bool _isDynamic,
    bool _isSensor,
    float angle = 0.0f,
    glm::vec4 _uvRect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

  //Destroy the box
  void Destroy(b2World* _world);

  //Draw the box
  void Draw(GameEngine::SpriteBatch& _spriteBatch);

  //See if the body is dynamic
  bool IsDynamic() const { return m_body->GetType() == b2_dynamicBody; }

  // Test if a point is inside the box
  bool TestPoint(float _x, float _y) const { return m_fixture->TestPoint(b2Vec2(_x, _y)); }


  // accessors
  b2Body*                       GetBody()          const { return m_body; }
  b2Fixture*                    GetFixture()       const { return m_fixture; }
  const glm::vec2&              GetDimensions()    const { return m_dimensions; }
  glm::vec2                     GetPosition()      const { return glm::vec2(m_body->GetPosition().x, m_body->GetPosition().y); }
  glm::vec4                     GetUvRect()        const { return m_uvRect; }
  const GameEngine::ColorRGBA8& GetColor()         const { return m_color; }
  float                         GetAngle()         const { return m_body->GetAngle(); }
  const GameEngine::GLTexture&  GetTexture()       const { return m_texture; }
  const bool&                   GetFixedRotation() const { return m_fixedRotation; }
  const bool&                   GetIsDynamic()     const { return m_isDynamic; }
  const bool&                   GetIsSensor()      const { return m_isSensor; }

private:
  glm::vec4 m_uvRect;
  b2Body* m_body = nullptr;
  b2Fixture* m_fixture = nullptr;
  glm::vec2 m_dimensions;
  GameEngine::ColorRGBA8 m_color;
  GameEngine::GLTexture m_texture;
  bool m_fixedRotation;
  bool m_isDynamic;
  bool m_isSensor;
};

#endif
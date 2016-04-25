#pragma once

#include <Box2D/Box2D.h>
#include <glm/glm.hpp>
#include <GameEngine/DebugRenderer.h>

class BulletBody
{
public:
  //Initialize the bullet's body
  void Init(b2World* _world,
    const glm::vec2& _position,
    const glm::vec2& _dimensions,
    float _density,
    float _friction);

  //Destroy the bullet
  void Destroy(b2World* _world);

  //Outline's the bullet collision box
  void DrawDebug(GameEngine::DebugRenderer& _debugRenderer);

  //Getters
  b2Body* GetBody() const { return m_body; }
  const glm::vec2 GetDimensions() const { return m_dimensions; }

private:
  b2Body* m_body = nullptr;
  b2Fixture* m_fixture = nullptr;
  glm::vec2 m_dimensions;
};


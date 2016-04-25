#ifndef _CAPSULE_
#define _CAPSULE_

#include <Box2D/Box2D.h>
#include <glm/glm.hpp>
#include <GameEngine/DebugRenderer.h>

class Capsule
{
public:
  //Initialize the Capsule
  void Init(b2World* _world,
        const glm::vec2& _position,
        const glm::vec2& _dimensions,
        float _density,
        float _friction,
        bool _fixedRotation);

  //Destroy the capsule
  void Destroy(b2World* _world);

  //Draw the outlines of its collision boxes
  void DrawDebug(GameEngine::DebugRenderer& _debugRenderer);

  //Getters
  b2Body* GetBody() const { return m_body; }
  b2Fixture* GetFixture(int _index) const { return m_fixtures[_index]; }
  const glm::vec2& GetDimensions()  const { return m_dimensions; }

private:
  b2Body* m_body = nullptr;
  b2Fixture* m_fixtures[3];
  glm::vec2 m_dimensions;
};

#endif
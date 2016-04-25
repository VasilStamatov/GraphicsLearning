#include "BulletBody.h"

void BulletBody::Init(b2World* _world,
  const glm::vec2& _position,
  const glm::vec2& _dimensions,
  float _density,
  float _friction)
{
  m_dimensions = _dimensions;

  ////////////////////////MAKE THE BODY///////////////////////////////
                                                                  ////
  b2BodyDef bodyDef;                                              ////
  //bullets must be dynamic                                       ////
  bodyDef.type = b2_dynamicBody;                                  ////
  bodyDef.position.Set(_position.x, _position.y);                 ////
  bodyDef.bullet = true;                                          ////
  bodyDef.fixedRotation = true;                                   ////
  //make the bullets ignore gravity so they don't fall            ////
  bodyDef.gravityScale = 0.0f;                                    ////
  m_body = _world->CreateBody(&bodyDef);                          ////
                                                                  ////
  ////////////////////////////////////////////////////////////////////

  //create the circle
  b2CircleShape circleShape;
  circleShape.m_radius = _dimensions.x / 2.0f;

  //define the fixture
  b2FixtureDef circleDef;
  circleDef.shape = &circleShape;
  circleDef.density = _density;
  circleDef.friction = _friction;

  //Create the fixture
  m_fixture = m_body->CreateFixture(&circleDef);
}

void BulletBody::Destroy(b2World* _world)
{
  //Destroy the body if there is one
  if (m_body)
  {
    _world->DestroyBody(m_body);
    m_body = nullptr;
  }
}

void BulletBody::DrawDebug(GameEngine::DebugRenderer& _debugRenderer)
{
  //white color
  GameEngine::ColorRGBA8 color(255, 255, 255, 255);
  //Set up the destination rectangle
  glm::vec4 destRect;
  destRect.x = m_body->GetPosition().x - m_dimensions.x / 2.0f;
  destRect.y = m_body->GetPosition().y - (m_dimensions.y - m_dimensions.x) / 2.0f;
  destRect.z = m_dimensions.x;
  destRect.w = m_dimensions.y - m_dimensions.x;
  //Draw the circle
  _debugRenderer.DrawCircle(glm::vec2(destRect.x + m_dimensions.x / 2.0f, destRect.y + destRect.w / 2.0f), color, m_dimensions.x / 2.0f);
}

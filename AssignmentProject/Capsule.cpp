#include "Capsule.h"

void Capsule::Init(b2World* _world,
          const glm::vec2& _position,
          const glm::vec2& _dimensions,
          float _density,
          float _friction,
          bool _fixedRotation)
{
  m_dimensions = _dimensions;

  //make the body
  b2BodyDef bodyDef;
  bodyDef.type = b2_dynamicBody;
  bodyDef.position.Set(_position.x, _position.y);
  bodyDef.fixedRotation = _fixedRotation;
  m_body = _world->CreateBody(&bodyDef);

  //create the box
  b2PolygonShape boxShape;
  boxShape.SetAsBox(_dimensions.x / 2.0f, (_dimensions.y - _dimensions.x) / 2.0f);
  
  b2FixtureDef fixtureDef;
  fixtureDef.shape = &boxShape;
  fixtureDef.density = _density;
  fixtureDef.friction = _friction;
  m_fixtures[0] = m_body->CreateFixture(&fixtureDef);

  //create the circles
  b2CircleShape circleShape;
  circleShape.m_radius = _dimensions.x / 2.0f;
  

  b2FixtureDef circleDef;
  circleDef.shape = &circleShape;
  circleDef.density = _density;
  circleDef.friction = _friction;

  //bottom circle
  circleShape.m_p.Set(0.0f, (-m_dimensions.y + _dimensions.x) / 2.0f);
  m_fixtures[1] = m_body->CreateFixture(&circleDef);

  //Top circle
  circleShape.m_p.Set(0.0f, (m_dimensions.y - _dimensions.x) / 2.0f);
  m_fixtures[2] = m_body->CreateFixture(&circleDef);
}

void Capsule::Destroy(b2World* _world)
{
  //Destroy the body if there is one
  if (m_body)
  {
    _world->DestroyBody(m_body);
    m_body = nullptr;
  }
}

void Capsule::DrawDebug(GameEngine::DebugRenderer& _debugRenderer)
{
  GameEngine::ColorRGBA8 color(255, 255, 255, 255);
  //Draw the box
  glm::vec4 destRect;
  destRect.x = m_body->GetPosition().x - m_dimensions.x / 2.0f;
  destRect.y = m_body->GetPosition().y - (m_dimensions.y - m_dimensions.x) / 2.0f;
  destRect.z = m_dimensions.x;
  destRect.w = m_dimensions.y - m_dimensions.x;
  _debugRenderer.DrawBox(destRect, color, m_body->GetAngle());

  //draw the 2 circles
  _debugRenderer.DrawCircle(glm::vec2(destRect.x + m_dimensions.x / 2.0f, destRect.y), color, m_dimensions.x / 2.0f);
  _debugRenderer.DrawCircle(glm::vec2(destRect.x + m_dimensions.x / 2.0f, destRect.y + destRect.w), color, m_dimensions.x / 2.0f);
}

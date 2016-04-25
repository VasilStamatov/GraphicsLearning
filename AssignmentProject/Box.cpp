#include "Box.h"


Box::Box() 
{
}


Box::~Box() 
{
}

void Box::Init(b2World* _world,
      const glm::vec2& _position,
      const glm::vec2& _dimensions,
      GameEngine::GLTexture _texture,
      GameEngine::ColorRGBA8 _color,
      bool _fixedRotation,
      bool _isDynamic,
      bool _isSensor,
      float _angle, /* = 0.0f */
      glm::vec4 _uvRect /*= glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)*/) 
{
  //set all the member variables
  m_dimensions = _dimensions;
  m_color = _color;
  m_texture = _texture;
  m_uvRect = _uvRect;
  m_fixedRotation = _fixedRotation;
  m_isDynamic = _isDynamic;
  m_isSensor = _isSensor;

  // Make the body
  b2BodyDef bodyDef;
  if (_isDynamic) 
  {
    bodyDef.type = b2_dynamicBody;
  }
  else 
  {
    bodyDef.type = b2_staticBody;
  }

  bodyDef.position.Set(_position.x, _position.y);
  bodyDef.fixedRotation = _fixedRotation;
  bodyDef.angle = _angle;
  m_body = _world->CreateBody(&bodyDef);

  b2PolygonShape boxShape;
  boxShape.SetAsBox(_dimensions.x / 2.0f, _dimensions.y / 2.0f);

  //Create the fixture
  b2FixtureDef fixtureDef;
  //recommended settings from documentation
  fixtureDef.isSensor = _isSensor;
  fixtureDef.shape = &boxShape;
  fixtureDef.density = 1.0f;
  fixtureDef.friction = 0.3f;
  m_fixture = m_body->CreateFixture(&fixtureDef);


}

void Box::Destroy(b2World* _world) 
{
  //Destroy the body 
  _world->DestroyBody(m_body);
}

void Box::Draw(GameEngine::SpriteBatch& _spriteBatch)
{
  //Draw the body
  glm::vec4 destRect;
  destRect.x = m_body->GetPosition().x - m_dimensions.x / 2.0f;
  destRect.y = m_body->GetPosition().y - m_dimensions.y / 2.0f;
  destRect.z = m_dimensions.x;
  destRect.w = m_dimensions.y;
  _spriteBatch.Draw(destRect, m_uvRect, m_texture.id, 0.0f, m_color, m_body->GetAngle());
}

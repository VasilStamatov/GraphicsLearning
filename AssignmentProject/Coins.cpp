#include "Coins.h"
#include <GameEngine\ResourceManager.h>

Coins::Coins(b2World* _world, const glm::vec2& _position, const glm::vec2& _dimensions, GameEngine::ColorRGBA8 _color)
{
  //set the member variables and initialize the box and texture
  GameEngine::GLTexture texture = GameEngine::ResourceManager::GetTexture("Assets/Objects/coin_sheet.png");
  m_color = _color;
  m_dimensions = _dimensions;
  m_box.Init(_world, _position, _dimensions, texture, _color, true, false, true);
  m_texture.Init(texture, glm::ivec2(6, 1));
}


Coins::~Coins()
{
}

void Coins::Draw(GameEngine::SpriteBatch& _spriteBatch)
{
  glm::vec4 destRect;
  b2Body* body = m_box.GetBody();
  //set up the destRect
  destRect.x = body->GetPosition().x - m_dimensions.x / 2.0f;
  destRect.y = body->GetPosition().y - m_box.GetDimensions().y / 2.0f;
  destRect.z = m_dimensions.x;
  destRect.w = m_dimensions.y;

  //set up the animation variables
  int tileIndex = 0;
  int numTiles = 6;
  float animSpeed = 0.2f;

  //increment animation time
  m_animTime += animSpeed;

  //apply animation
  tileIndex = tileIndex + (int)m_animTime % numTiles;

  //Get the UV coords from the tile index
  glm::vec4 uvRect = m_texture.GetUVs(tileIndex);

  //draw the sprite
  _spriteBatch.Draw(destRect, uvRect, m_texture.texture.id, 0.0f, m_color, body->GetAngle());
}
void Coins::Destroy(b2World* _world)
{
  //Destroy the box
  m_box.Destroy(_world);
}
void Coins::DrawDebug(GameEngine::DebugRenderer& _debugRenderer)
{
  glm::vec4 destRect;
  //set the main position to bottom left of the sprite
  destRect.x = m_box.GetBody()->GetPosition().x - m_box.GetDimensions().x / 2.0f;
  destRect.y = m_box.GetBody()->GetPosition().y - m_box.GetDimensions().y / 2.0f;
  destRect.z = m_box.GetDimensions().x;
  destRect.w = m_box.GetDimensions().y;
  //dynamic = green / static = red
  if (m_box.IsDynamic())
  {
    _debugRenderer.DrawBox(destRect, GameEngine::ColorRGBA8(0, 255, 0, 255), m_box.GetBody()->GetAngle());
  }
  else
  {
    _debugRenderer.DrawBox(destRect, GameEngine::ColorRGBA8(255, 0, 0, 255), m_box.GetBody()->GetAngle());
  }
}
#pragma once

#include <GameEngine/SpriteBatch.h>
#include <GameEngine/TileSheet.h>
#include "Box.h"
#include <GameEngine\DebugRenderer.h>
class Coins
{
public:
  Coins(b2World* _world, const glm::vec2& _position, const glm::vec2& _dimensions, GameEngine::ColorRGBA8 _color);
  ~Coins();

  //void Init(b2World* _world, const glm::vec2& _pos, const glm::vec2& _drawDims, glm::vec2& _collisionDims, GameEngine::ColorRGBA8 _color);
  void Draw(GameEngine::SpriteBatch& _spriteBatch);
  void Destroy(b2World* _world);
  void DrawDebug(GameEngine::DebugRenderer& _debugRenderer);

  //Getters
  glm::vec2 GetPosition() const
  {
    glm::vec2 rv;
    rv.x = m_box.GetBody()->GetPosition().x;
    rv.y = m_box.GetBody()->GetPosition().y;
    return rv;
  }
  const Box GetBox()                         const { return m_box; }
  const glm::vec2& GetDimensions()           const { return m_dimensions; }
  const GameEngine::ColorRGBA8& GetColor()   const { return m_color; }
private:
  glm::vec2 m_dimensions;
  GameEngine::ColorRGBA8 m_color;
  GameEngine::TileSheet m_texture;
  Box m_box;
  float m_animTime = 0.0f;
};


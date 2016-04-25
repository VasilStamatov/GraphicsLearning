#ifndef _LIGHT_
#define _LIGHT_

#include <GameEngine\Vertex.h>
#include <GameEngine\SpriteBatch.h>
#include <glm\glm.hpp>

class Light
{
public:
  void Draw(GameEngine::SpriteBatch& _spriteBatch)
  {
    //draw the light
    glm::vec4 destRect;
    destRect.x = position.x - size / 2.0f;
    destRect.y = position.y - size / 2.0f;
    destRect.z = size;
    destRect.w = size;
    _spriteBatch.Draw(destRect, glm::vec4(-1.0f,-1.0f,2.0f,2.0f), 0, 0.0f, color, 0.0f);
  }
  GameEngine::ColorRGBA8 color;
  glm::vec2 position;
  float size;
};

#endif
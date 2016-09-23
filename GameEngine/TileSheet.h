#pragma once

#include "GLTexture.h"
#include <glm\glm.hpp>

namespace GameEngine
{
  class TileSheet
  {
  public:
    //initialize the tilesheet with the spritesheet and the dimensions
    void Init(const GLTexture& _texture, const glm::ivec2& _tileDims)
    {
      this->texture = _texture;
      this->dims = _tileDims;
    }
    //gets the UVs
    glm::vec4 GetUVs(int _index)
    {
      //make sure the xTile isn't bigger than dims.x
      int xTile = _index % dims.x;
      int yTile = _index / dims.x;

      glm::vec4 uvs;
      uvs.x = xTile / (float)dims.x;
      uvs.y = yTile / (float)dims.y;
      uvs.z = 1.0f / dims.x;
      uvs.w = 1.0f / dims.y;

      return uvs;
    }

    GLTexture texture;
    glm::ivec2 dims{ 0, 0 };
  };
}
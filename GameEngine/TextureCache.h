#pragma once
#include <map>
#include "GLTexture.h"

namespace GameEngine
{
  //This caches the textures so that multiple sprites can use the same textures

  class TextureCache
  {
  public:
    TextureCache();
    ~TextureCache();
    //gets the texture in the filepath passed(or returns an already cached texture without loading a new one)
    GLTexture GetTexture(std::string _texturePath);

  private:
    std::map<std::string, GLTexture> m_textureMap; //< texture cache
  };
}



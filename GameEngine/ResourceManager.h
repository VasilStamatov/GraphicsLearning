#pragma once
#include "TextureCache.h"
#include <string>

namespace GameEngine
{
  //This is a way to access all the resources, such as textures.
  class ResourceManager
  {
  public:
    //gets the texture from the specified filepath
    static GLTexture GetTexture(std::string _texturePath);
  private:
    //the texture cache
    static TextureCache s_textureCache;
  };
}



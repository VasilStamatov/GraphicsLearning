#pragma once
#include "TextureCache.h"
#include <string>
#include <vector>

namespace GameEngine
{
  //This is a way to access all the resources, such as textures.
  class ResourceManager
  {
  public:
    //gets the texture from the specified filepath
    static GLTexture GetTexture(const std::string& _texturePath, bool _alpha = false);
    //
    static GLCubemap GetCubemap(const std::vector<std::string>& _facesPaths);
  private:
    //the texture cache
    static TextureCache s_textureCache;
  };
}



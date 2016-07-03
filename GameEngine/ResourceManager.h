#pragma once
#include "Cache.h"
#include <string>
#include <vector>

namespace GameEngine
{
  //This is a way to access all the resources, such as textures.
  class ResourceManager
  {
  public:
    //gets the texture from the specified filepath
    static GLTexture GetTexture(const std::string& _texturePath, bool _alpha = true);
    //
    static GLCubemap GetCubemap(const std::vector<std::string>& _facesPaths);
    //
    static Model GetModel(const std::string& _texturePath);
  private:
    //the texture cache
    static Cache s_cache;
  };
}



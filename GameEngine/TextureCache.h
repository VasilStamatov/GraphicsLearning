#pragma once
#include <map>
#include <vector>
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
    GLTexture GetTexture(const std::string& _texturePath, bool _alpha);
    GLCubemap GetCubemap(const std::vector<std::string>& _facesPaths);
  private:
    std::map<std::string, GLTexture> m_textureMap; //< texture cache
    std::map<std::vector<std::string>, GLCubemap> m_cubeMapMap;
  };
}



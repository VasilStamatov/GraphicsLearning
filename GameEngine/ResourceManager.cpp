#include "ResourceManager.h"

namespace GameEngine
{
  TextureCache ResourceManager::s_textureCache;

  GLTexture ResourceManager::GetTexture(const std::string& _texturePath, bool _alpha)
  {
    //use the cache to get the texture
    return s_textureCache.GetTexture(_texturePath, _alpha);
  }
  GLCubemap ResourceManager::GetCubemap(const std::vector<std::string>& _facesPaths)
  {
    //use the cache to get the cubemap
    return s_textureCache.GetCubemap(_facesPaths);
  }

}


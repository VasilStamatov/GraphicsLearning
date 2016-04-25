#include "ResourceManager.h"

namespace GameEngine
{
  TextureCache ResourceManager::s_textureCache;

  GLTexture ResourceManager::GetTexture(std::string _texturePath)
  {
    //use the cache to get the texture
    return s_textureCache.GetTexture(_texturePath);
  }
}


#include "ResourceManager.h"
#include "GLTexture.h"
#include "Model.h"

namespace GameEngine
{
  Cache ResourceManager::s_cache;

  GLTexture ResourceManager::GetTexture(const std::string& _texturePath, bool _alpha)
  {
    //use the cache to get the texture
    return s_cache.GetTexture(_texturePath, _alpha);
  }
  GLCubemap ResourceManager::GetCubemap(const std::vector<std::string>& _facesPaths)
  {
    //use the cache to get the cubemap
    return s_cache.GetCubemap(_facesPaths);
  }
  Model ResourceManager::GetModel(const std::string& _texturePath)
  {
    return s_cache.GetModel(_texturePath);
  }
}


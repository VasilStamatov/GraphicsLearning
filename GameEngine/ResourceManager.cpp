#include "ResourceManager.h"
#include "GLTexture.h"
#include "Model.h"
#include "GLSLProgram.h"
namespace GameEngine
{
  // Instantiate static variables
  Cache ResourceManager::s_cache;

  GLTexture ResourceManager::GetTexture(const std::string& _texturePath, bool _alpha)
  {
    //use the cache to get the texture
    return s_cache.GetTexture(_texturePath, _alpha);
  }
  GLCubemap ResourceManager::GetCubemap(const std::string& _directory,
    const std::string& _posXFilename,
    const std::string& _negXFilename,
    const std::string& _posYFilename,
    const std::string& _negYFilename,
    const std::string& _posZFilename,
    const std::string& _negZFilename,
    const std::string& _cubemapName)
  {
    //use the cache to get the cubemap
    return s_cache.GetCubemap(_directory, _posXFilename, _negXFilename, _posYFilename, _negYFilename, _posZFilename, _negZFilename, _cubemapName);
  }
  void ResourceManager::GetSkinnedModel(const std::string & _filepath, SkinnedModel* _model)
  {
    s_cache.GetSkinnedModel(_filepath, _model);
  }
  void ResourceManager::GetStaticModel(const std::string & _filepath, StaticModel* _model)
  {
    s_cache.GetStaticModel(_filepath, _model);
  }
  void ResourceManager::Clear()
  {
    s_cache.ClearCache();
  }
}


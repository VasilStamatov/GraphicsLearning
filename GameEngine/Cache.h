#pragma once
#include <map>
#include <vector>

namespace GameEngine
{
  //This caches the textures so that multiple sprites can use the same textures
  struct GLTexture;
  struct GLCubemap;
  class SkinnedModel;
  class StaticModel;

  class Cache
  {
  public:
    Cache();
    ~Cache();
    //gets the texture in the filepath passed(or returns an already cached texture without loading a new one)
    GLTexture GetTexture(const std::string& _texturePath, bool _alpha);
    GLCubemap GetCubemap(const std::string& _directory, const std::string& _posXFilename, const std::string& _negXFilename,
      const std::string& _posYFilename, const std::string& _negYFilename, const std::string& _posZFilename,
      const std::string& _negZFilename, const std::string& _cubemapName);
    void GetSkinnedModel(const std::string& _filePath, SkinnedModel* _model);
    void GetStaticModel(const std::string& _filePath, StaticModel* _model);

    void ClearCache();
  private:
    std::map<std::string, GLTexture> m_textureCache; //< texture cache
    std::map<std::string, GLCubemap> m_cubemapCache;
    std::map<std::string, SkinnedModel*> m_skinnedModelCache;
    std::map<std::string, StaticModel*> m_staticModelCache;
  };
}



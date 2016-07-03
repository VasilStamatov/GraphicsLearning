#pragma once
#include <map>
#include <vector>

namespace GameEngine
{
  //This caches the textures so that multiple sprites can use the same textures
  class GLTexture;
  class GLCubemap;
  class Model;

  class Cache
  {
  public:
    Cache();
    ~Cache();
    //gets the texture in the filepath passed(or returns an already cached texture without loading a new one)
    GLTexture GetTexture(const std::string& _texturePath, bool _alpha);
    GLCubemap GetCubemap(const std::vector<std::string>& _facesPaths);
    Model GetModel(const std::string& _filePath);
  private:
    std::map<std::string, GLTexture> m_textureCache; //< texture cache
    std::map<std::vector<std::string>, GLCubemap> m_cubemapCache;
    std::map<std::string, Model> m_modelCache;
  };
}



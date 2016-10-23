#include "Cache.h"
#include "ImageLoader.h"
#include "GLTexture.h"
#include "AssimpLoader.h"

namespace GameEngine
{
  Cache::Cache()
  {
  }


  Cache::~Cache()
  {
  }

  GLTexture Cache::GetTexture(const std::string& _texturePath, bool _alpha)
  {
    //lookup the texture and see if it's in the map
    //std::map<std::string, GLTexture>::iterator mit = m_textureMap.find(_filePath);
    auto mit = m_textureCache.find(_texturePath);
    //check if it's not in the map
    if (mit == m_textureCache.end())
    {
      //if it's not in the map, then load the texture
      GLTexture newTexture = ImageLoader::LoadPNG(_texturePath, _alpha);
      //insert it into the map
      m_textureCache.insert(std::make_pair(_texturePath, newTexture));
      //return the new texture
      return newTexture;
    }
    //if the texture is in the map, then
    //return the already existing (cached) texture
    return mit->second;
  }
  GLCubemap Cache::GetCubemap(const std::string& _directory, const std::string& _posXFilename, const std::string& _negXFilename, const std::string& _posYFilename,
    const std::string& _negYFilename, const std::string& _posZFilename, const std::string& _negZFilename, const std::string& _cubemapName)
  {
    auto it = m_cubemapCache.find(_cubemapName);
    //check if it's not in the map
    if (it == m_cubemapCache.end())
    {
      //if it's not in the map, then load the texture
      GLCubemap newCubemap = ImageLoader::LoadCubemap( _directory,
        _posXFilename,
        _negXFilename,
        _posYFilename,
        _negYFilename,
        _posZFilename,
        _negZFilename);
      //insert it into the map
      m_cubemapCache.insert(std::make_pair(_cubemapName, newCubemap));
      //return the new texture
      return newCubemap;
    }
    //if the texture is in the map, then
    //return the already existing (cached) texture
    return it->second;
  }
  void Cache::GetSkinnedModel(const std::string& _filePath, SkinnedModel* _model)
  {
    auto it = m_skinnedModelCache.find(_filePath);
    //check if it's not in the map
    if (it == m_skinnedModelCache.end())
    {
      //if it's not in the map, then load the texture
      AssimpLoader loader;
      if (loader.LoadSkinnedModel(_filePath, _model))
      {
        //insert it into the map
        m_skinnedModelCache.insert(std::make_pair(_filePath, _model));
        return;
      }
      return;
    }
    //if the texture is in the map, then
    //return the already existing (cached) texture
    _model = it->second;
    return;
  }

  void Cache::GetStaticModel(const std::string & _filePath, StaticModel* _model)
  {
    auto it = m_staticModelCache.find(_filePath);
    //check if it's not in the map
    if (it == m_staticModelCache.end())
    {
      //if it's not in the map, then load the texture
      AssimpLoader loader;
      if (loader.LoadStaticModel(_filePath, _model))
      {
        //insert it into the map
        m_staticModelCache.insert(std::make_pair(_filePath, _model));
        return;
      }
      return;
    }
    //if the texture is in the map, then
    //return the already existing (cached) texture
    _model = it->second;
    return;
  }
  void Cache::ClearCache()
  {
    for (auto& texture : m_textureCache)
    {
      texture.second.Dispose();
    }
    m_textureCache.clear();

    for (auto& cubemap : m_cubemapCache)
    {
      cubemap.second.Dispose();
    }
    m_cubemapCache.clear();

    for (auto& staticModel : m_staticModelCache)
    {
      staticModel.second->Dispose();
    }
    m_staticModelCache.clear();

    for (auto& skinnedModel : m_skinnedModelCache)
    {
      skinnedModel.second->Dispose();
    }
    m_skinnedModelCache.clear();
  }
}


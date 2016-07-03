#include "Cache.h"
#include "ImageLoader.h"
#include "GLTexture.h"
#include "Model.h"

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
      m_textureCache.insert(make_pair(_texturePath, newTexture));
      //return the new texture
      return newTexture;
    }
    //if the texture is in the map, then
    //return the already existing (cached) texture
    return mit->second;
  }
  GLCubemap Cache::GetCubemap(const std::vector<std::string>& _facesPaths)
  {
    auto it = m_cubemapCache.find(_facesPaths);
    //check if it's not in the map
    if (it == m_cubemapCache.end())
    {
      //if it's not in the map, then load the texture
      GLCubemap newCubemap = ImageLoader::LoadCubemap(_facesPaths);
      //insert it into the map
      m_cubemapCache.insert(make_pair(_facesPaths, newCubemap));
      //return the new texture
      return newCubemap;
    }
    //if the texture is in the map, then
    //return the already existing (cached) texture
    return it->second;
  }
  Model Cache::GetModel(const std::string& _filePath)
  {
    auto it = m_modelCache.find(_filePath);
    //check if it's not in the map
    if (it == m_modelCache.end())
    {
      //if it's not in the map, then load the texture
      Model newModel(_filePath);
      //insert it into the map
      m_modelCache.insert(make_pair(_filePath, newModel));
      //return the new texture
      return newModel;
    }
    //if the texture is in the map, then
    //return the already existing (cached) texture
    return it->second;
  }
}


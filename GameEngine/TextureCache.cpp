#include "TextureCache.h"
#include "ImageLoader.h"

namespace GameEngine
{
  TextureCache::TextureCache()
  {
  }


  TextureCache::~TextureCache()
  {
  }

  GLTexture TextureCache::GetTexture(const std::string& _texturePath, bool _alpha)
  {
    //lookup the texture and see if it's in the map
    //std::map<std::string, GLTexture>::iterator mit = m_textureMap.find(_filePath);
    auto mit = m_textureMap.find(_texturePath);
    //check if it's not in the map
    if (mit == m_textureMap.end())
    {
      //if it's not in the map, then load the texture
      GLTexture newTexture = ImageLoader::LoadPNG(_texturePath, _alpha);
      //insert it into the map
      m_textureMap.insert(make_pair(_texturePath, newTexture));
      //return the new texture
      return newTexture;
    }
    //if the texture is in the map, then
    //return the already existing (cached) texture
    return mit->second;
  }
  GLCubemap TextureCache::GetCubemap(const std::vector<std::string>& _facesPaths)
  {
    auto it = m_cubeMapMap.find(_facesPaths);
    //check if it's not in the map
    if (it == m_cubeMapMap.end())
    {
      //if it's not in the map, then load the texture
      GLCubemap newCubemap = ImageLoader::LoadCubemap(_facesPaths);
      //insert it into the map
      m_cubeMapMap.insert(make_pair(_facesPaths, newCubemap));
      //return the new texture
      return newCubemap;
    }
    //if the texture is in the map, then
    //return the already existing (cached) texture
    return it->second;

  }
}


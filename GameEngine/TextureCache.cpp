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

  GLTexture TextureCache::GetTexture(std::string _texturePath)
  {
    //lookup the texture and see if it's in the map
    //std::map<std::string, GLTexture>::iterator mit = m_textureMap.find(_filePath);
    auto mit = m_textureMap.find(_texturePath);
    //check if it's not in the map
    if (mit == m_textureMap.end())
    {
      //if it's not in the map, then load the texture
      GLTexture newTexture = ImageLoader::LoadPNG(_texturePath);
      //insert it into the map
      m_textureMap.insert(make_pair(_texturePath, newTexture));
      //return the new texture
      return newTexture;
    }
    //if the texture is in the map, then
    //return the already existing (cached) texture
    return mit->second;
  }
}


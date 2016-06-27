#pragma once

#include "GLTexture.h"
#include <string>
namespace GameEngine
{
  class ImageLoader
  {
  public:
    //png image loader
    static GLTexture LoadPNG(const std::string& _filePath, bool _alpha);
    static GLCubemap LoadCubemap(const std::vector<std::string>& _faces);
  };
}



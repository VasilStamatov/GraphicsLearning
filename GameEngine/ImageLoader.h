#pragma once

#include "GLTexture.h"
#include <string>
namespace GameEngine
{
  class ImageLoader
  {
  public:
    //png image loader
    static GLTexture LoadPNG(std::string _filePath);
  };
}



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
    static GLCubemap LoadCubemap(const std::string& _directory,
      const std::string& _posXFilename,
      const std::string& _negXFilename,
      const std::string& _posYFilename,
      const std::string& _negYFilename,
      const std::string& _posZFilename,
      const std::string& _negZFilename);
  };
}



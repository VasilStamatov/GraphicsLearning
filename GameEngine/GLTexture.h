#pragma once
#include <GL/glew.h>
#include <string>

namespace GameEngine
{
  struct GLTexture
  {
    std::string filePath = "";
    GLuint id;
    int width;
    int height;
  };
}

#pragma once
#include <GL/glew.h>
#include <string>

namespace GameEngine
{
  struct GLTexture
  {
    std::string filePath = "";
    std::string type = "";
    GLuint id;
    int width;
    int height;
  };
}

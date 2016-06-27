#pragma once
#include <GL/glew.h>
#include <string>
#include <vector>

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

  struct GLCubemap
  {
    std::vector<GLTexture> textures;
    GLuint id;
  };
}

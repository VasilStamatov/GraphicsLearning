#pragma once
#include <GL/glew.h>
#include <string>
#include <array>

namespace GameEngine
{
  struct GLTexture
  {

    std::string filePath{ "" };
    std::string type{ "" };
    GLuint id{ 0 };
    int width{ 500 };
    int height{ 500 };

    void Dispose()
    {
      glDeleteTextures(1, &id);
      id = 0;
    }
  };

  struct GLCubemap
  {
    std::array<GLTexture, 6> textures;
    GLuint id;

    void Dispose()
    {
      glDeleteTextures(1, &id);
    }
  };
}

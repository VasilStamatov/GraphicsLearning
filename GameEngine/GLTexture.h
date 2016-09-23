#pragma once
#include <GL/glew.h>
#include <string>
#include <vector>

namespace GameEngine
{
  struct GLTexture
  {
    std::string filePath{ "" };
    std::string type{ "" };
    GLuint id{ 0 };
    int width{ 500 };
    int height{ 500 };

    void Bind(GLenum _textureUnit)
    {
      glActiveTexture(_textureUnit);
      glBindTexture(GL_TEXTURE_2D, id);
    }

    static void UnbindTexture()
    {
      glBindTexture(GL_TEXTURE_2D, 0);
    }
  };

  struct GLCubemap
  {
    std::vector<GLTexture> textures;
    GLuint id;

    void Bind(GLenum _textureUnit)
    {
      glActiveTexture(_textureUnit);
      glBindTexture(GL_TEXTURE_CUBE_MAP, id);
    }

    static void UnbindCubemap()
    {
      glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
  };
}

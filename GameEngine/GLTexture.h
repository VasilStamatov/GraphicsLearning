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

    void Bind(GLenum _textureUnit)
    {
      glActiveTexture(_textureUnit);
      glBindTexture(GL_TEXTURE_2D, id);
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
  };
}

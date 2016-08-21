#pragma once
#include <GL\glew.h>

namespace GameEngine
{
  class ScreenQuad
  {
  public:
    ScreenQuad();
    ~ScreenQuad();

    void Init();
    void Dispose();
    void Render(GLuint _textureID);
  private:
    GLuint m_VAO = 0, m_VBO = 0, m_EBO = 0;
  };
}


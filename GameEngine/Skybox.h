#pragma once
#include "GLTexture.h"
#include "GLSLProgram.h"
#include "Camera3D.h"

#include <memory>

namespace GameEngine
{
  //class GLTexture;
  //class GLSLProgram;
  //class Camera3D;

  class Skybox
  {
  public:
    Skybox();
    ~Skybox();

    void Init(const GLCubemap& _cubemap);

    void Dispose();

    void Render();

    void SetCubemap(const GLCubemap& _cubemap) { m_cubemap = _cubemap; }
  private:
    GLuint m_VAO{ 0 }, m_VBO{ 0 };
    GLCubemap m_cubemap;
  };
}



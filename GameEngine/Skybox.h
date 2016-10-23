#pragma once
#include <memory>

#include "GLTexture.h"
#include "GLSLProgram.h"
#include "Camera3D.h"
#include "Model.h"

namespace GameEngine
{
  /*class GLTexture;
  class GLSLProgram;
  class Camera3D;
  class GLCubemap;
  class StaticModel;*/

  class Skybox
  {
  public:
    Skybox();
    ~Skybox();

    void Init(std::unique_ptr<GLCubemap> _cubemap, std::unique_ptr<GLSLProgram> _shader, std::weak_ptr<Camera3D> _camera, bool _isSphere = true);

    void Dispose();

    void Render();

    void SetCubemap(std::unique_ptr<GLCubemap> _cubemap) { m_cubemap = std::move(_cubemap); }
    void SetShader(std::unique_ptr<GLSLProgram> _shader) { m_shader = std::move(_shader); }
    void SetCamera(std::weak_ptr<Camera3D> _camera)      { m_camera = _camera; }
  private:
    std::unique_ptr<GLCubemap> m_cubemap;
    std::unique_ptr<GLSLProgram> m_shader;
    std::unique_ptr<StaticModel> m_model;
    std::weak_ptr<Camera3D> m_camera;
  };
}



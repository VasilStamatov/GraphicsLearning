#include "Skybox.h"
#include "ResourceManager.h"

namespace GameEngine
{
  Skybox::Skybox()
  {
  }


  Skybox::~Skybox()
  {
    Dispose();
  }

  void Skybox::Init(std::unique_ptr<GLCubemap> _cubemap, std::unique_ptr<GLSLProgram> _shader, std::weak_ptr<Camera3D> _camera, bool _isSphere)
  {
    m_cubemap = std::move(_cubemap);
    m_shader = std::move(_shader);
    m_camera = _camera;

    m_model = std::make_unique<StaticModel>();

    if (_isSphere)
    {
      ResourceManager::GetStaticModel("Assets/Sphere/sphere.obj", m_model.get());
    }
    else
    {
      ResourceManager::GetStaticModel("Assets/Box/box2.obj", m_model.get());
    }
  }

  void Skybox::Dispose()
  {
    //delete the vao's and vbo's and reset them to 0
    m_model->Dispose();
  }

  void Skybox::Render()
  {
    m_shader->Use();

    GLint oldCullMode{ 0 };
    glGetIntegerv(GL_CULL_FACE_MODE, &oldCullMode);
    GLint oldDepthFunc{ 0 };
    glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFunc);

    glCullFace(GL_FRONT);
    glDepthFunc(GL_LEQUAL);

    // skybox cube
    m_shader->UploadValue("view", m_camera._Get()->GetViewMatrix());
    m_shader->UploadValue("projection", m_camera._Get()->GetProjectionMatrix());
    m_shader->UploadValue("skybox", 0, *m_cubemap);

    m_model->SetScale(glm::vec3(20.0f));
    m_model->SetPosition(m_camera._Get()->GetPosition());
    m_model->SetRotation(glm::vec3(0.0, 0.0f, 0.0f));
    m_model->Draw(*m_shader);

    glCullFace(oldCullMode);
    glDepthFunc(oldDepthFunc);

    m_shader->UnUse();
  }
}
#pragma once
#include <glm\mat4x4.hpp>
#include <glm\vec3.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <vector>

namespace GameEngine
{
  class LightCamera
  {
  public:
    LightCamera()
    {
    }

    ~LightCamera()
    {
    }

    void InitForPointLight(float _fov, int _screenWidth, int _screenHeight, float _near, float _far, const glm::vec3& _lightPos)
    {
      m_far = _far;
      glm::mat4 projection = glm::perspective(glm::radians(_fov), (float)_screenWidth / (float)_screenHeight, _near, _far);
      m_shadowTransforms.clear();
      m_shadowTransforms.resize(6);
      m_shadowTransforms.at(0) = projection *
        glm::lookAt(_lightPos, _lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
      m_shadowTransforms.at(1) = projection *
        glm::lookAt(_lightPos, _lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
      m_shadowTransforms.at(2) = projection *
        glm::lookAt(_lightPos, _lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
      m_shadowTransforms.at(3) = projection *
        glm::lookAt(_lightPos, _lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
      m_shadowTransforms.at(4) = projection *
        glm::lookAt(_lightPos, _lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
      m_shadowTransforms.at(5) = projection *
        glm::lookAt(_lightPos, _lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));
    }

    void InitForDirLight(float _left, float _right, float _bottom, float _top, float _near, float _far,
      const glm::vec3& _lightPos, glm::vec3& _direction)
    {
      glm::mat4 projection = glm::ortho(_left, _right, _bottom, _top, _near, _far);
      m_shadowTransforms.clear();
      m_shadowTransforms.resize(1);
      m_shadowTransforms.at(0) = projection *
        glm::lookAt(_lightPos, _lightPos + _direction, glm::vec3(0.0, 1.0, 0.0));
    }

    std::vector<glm::mat4>& GetLightTransforms() { return m_shadowTransforms; }
    float GetFarPlane() const noexcept { return m_far; }
  private:
    std::vector<glm::mat4> m_shadowTransforms;
    float m_far;
  };
}

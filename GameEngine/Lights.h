#pragma once

#include <glm\vec3.hpp>
#include <string>
#include "GLSLProgram.h"

namespace GameEngine
{
  class PointLight
  {
  public:
    PointLight(const glm::vec3& _position,
      const glm::vec3& _ambient, const glm::vec3& _diffuse, const glm::vec3& _specular,
      float _constant, float _linear, float _quadratic);
    PointLight() { }
    ~PointLight();

    void Init(const glm::vec3& _position,
      const glm::vec3& _ambient, const glm::vec3& _diffuse, const glm::vec3& _specular,
      float _constant, float _linear, float _quadratic);

    void UploadToShader(GLSLProgram& _shader, const std::string& _uniformName);

    glm::vec3 GetPosition() const { return m_position; }

  private:
    glm::vec3 m_position;

    glm::vec3 m_ambient;
    glm::vec3 m_diffuse;
    glm::vec3 m_specular;

    float m_constant;
    float m_linear;
    float m_quadratic;
  };

  class SpotLight
  {
  public:
    SpotLight(const glm::vec3& _position, const glm::vec3& _direction,
      const glm::vec3& _ambient, const glm::vec3& _diffuse, const glm::vec3& _specular,
      float _constant, float _linear, float _quadratic,
      float _cutOff, float _outerCutOff);
    SpotLight() { }
    ~SpotLight();

    void Init(const glm::vec3& _position, const glm::vec3& _direction,
      const glm::vec3& _ambient, const glm::vec3& _diffuse, const glm::vec3& _specular,
      float _constant, float _linear, float _quadratic,
      float _cutOff, float _outerCutOff);

    void UploadToShader(GLSLProgram& _shader, const std::string& _uniformName);

    void SetPosition(const glm::vec3& _position)
    {
      m_position = _position;
    }
    void SetDirection(const glm::vec3& _direction)
    {
      m_direction = _direction;
    }

    glm::vec3 GetPosition() const { return m_position; }

  private:
    glm::vec3 m_position;
    glm::vec3 m_direction;

    glm::vec3 m_ambient;
    glm::vec3 m_diffuse;
    glm::vec3 m_specular;

    float m_constant;
    float m_linear;
    float m_quadratic;

    float m_cutOff;
    float m_outerCutOff;
  };

  class DirectionalLight
  {
  public:
    DirectionalLight(const glm::vec3& _direction, const glm::vec3& _ambient, const glm::vec3& _diffuse, const glm::vec3& _specular);
    DirectionalLight() { }
    ~DirectionalLight();

    void Init(const glm::vec3& _direction, const glm::vec3& _ambient, const glm::vec3& _diffuse, const glm::vec3& _specular);
    void UploadToShader(GLSLProgram& _shader, const std::string& _uniformName);

  private:
    glm::vec3 m_direction;

    glm::vec3 m_ambient;
    glm::vec3 m_diffuse;
    glm::vec3 m_specular;
  };
}



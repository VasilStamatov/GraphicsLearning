#pragma once

#include <glm\vec3.hpp>
#include <string>
#include "GLSLProgram.h"

namespace GameEngine
{
  struct Attenuation
  {
    Attenuation(float _constant, float _linear, float _quadratic) :
      m_constant(_constant), m_linear(_linear), m_quadratic(_quadratic) {}
    float m_constant{ 0.0f };
    float m_linear{ 0.0f };
    float m_quadratic{ 0.0f };
  };

  class BaseLight
  {
  public:
    BaseLight() {}
    virtual ~BaseLight() {}

    virtual void UploadToShader(GLSLProgram& _shader, const std::string& _uniformName) = 0;

    void SetName(const std::string& _name) { m_name = _name; }
    void SetColor(const glm::vec3& _color) { m_color = _color; }
    void SetAmbient(const float& _ambient) { m_ambient = _ambient; }
    void SetDiffuse(const float& _diffuse) { m_diffuse = _diffuse; }
    void SetSpecular(const float& _specular) { m_specular = _specular; }

  protected:
    //name of the light
    std::string m_name{ "default" };
    //The color of the light
    glm::vec3 m_color{ 1.0f, 1.0f, 1.0f };
    //the ambient intensity
    float m_ambient{ 1.0f };
    //the diffuse intensity
    float m_diffuse{ 1.0f };
    //the specular intensity
    float m_specular{ 1.0f };
  };

  class PointLight : public BaseLight
  {
  public:
    PointLight(const glm::vec3& _position,
      const float& _ambient, const float& _diffuse, const float& _specular,
      float _constant, float _linear, float _quadratic);
    PointLight() { }
    ~PointLight();

    void Init(const glm::vec3& _position,
      const float& _ambient, const float& _diffuse, const float& _specular,
      float _constant, float _linear, float _quadratic);

    void UploadToShader(GLSLProgram& _shader, const std::string& _uniformName) override;

    void SetPosition(const glm::vec3& _position) { m_position = _position; }

    glm::vec3 GetPosition() const { return m_position; }

    void SetAttenuation(const Attenuation& _attenuation) { m_attenuation = _attenuation; }
    void SetConstant(float _constant) { m_attenuation.m_constant = _constant; }
    void SetLinear(float _linear) { m_attenuation.m_linear = _linear; }
    void SetQuadratic(float _quadratic) { m_attenuation.m_quadratic = _quadratic; }

  protected:
    //The light's position
    glm::vec3 m_position{ 0.0f, 0.0f, 0.0f };

    Attenuation m_attenuation{ 0.0f, 0.0f, 0.0f };
  };

  class SpotLight : public PointLight
  {
  public:
    SpotLight(const glm::vec3& _position, const glm::vec3& _direction,
      const float& _ambient, const float& _diffuse, const float& _specular,
      float _constant, float _linear, float _quadratic,
      float _cutOff, float _outerCutOff);
    SpotLight() { }
    ~SpotLight();

    void Init(const glm::vec3& _position, const glm::vec3& _direction,
      const float& _ambient, const float& _diffuse, const float& _specular,
      float _constant, float _linear, float _quadratic,
      float _cutOff, float _outerCutOff);

    void UploadToShader(GLSLProgram& _shader, const std::string& _uniformName) override;

    void SetDirection(const glm::vec3& _direction) { m_direction = _direction; }
    void SetInnerCutoff(float _cutoff) { m_cutOff = _cutoff; }
    void SetOuterCutoff(float _outer) { m_outerCutOff = _outer; }

    glm::vec3 GetDirection() const { return m_direction; }

  private:
    glm::vec3 m_direction{ 0.0f, 0.0f, 0.0f };

    float m_cutOff{ 0.0f };
    float m_outerCutOff{ 0.0f };
  };

  class DirectionalLight : public BaseLight
  {
  public:
    DirectionalLight(const glm::vec3& _direction, const float& _ambient, const float& _diffuse, const float& _specular);
    DirectionalLight() { }
    ~DirectionalLight();

    void Init(const glm::vec3& _direction, const float& _ambient, const float& _diffuse, const float& _specular);

    void UploadToShader(GLSLProgram& _shader, const std::string& _uniformName) override;

    void SetDirection(const glm::vec3& _direction) { m_direction = _direction; }

    glm::vec3 GetDirection() const { return m_direction; }
  private:
    //The light's direction
    glm::vec3 m_direction{ 0.0f, 0.0f, 0.0f };
  };
}



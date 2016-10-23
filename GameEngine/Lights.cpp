#include "Lights.h"

namespace GameEngine
{
  // ------------------------- POINT LIGHT ------------------------------------

  PointLight::PointLight(const glm::vec3& _position,
    const float& _ambient, const float& _diffuse, const float& _specular,
    float _constant, float _linear, float _quadratic)
  {
    m_position = _position;
    m_ambient = _ambient;
    m_diffuse = _diffuse;
    m_specular = _specular;
    m_attenuation.m_constant = _constant;
    m_attenuation.m_linear = _linear;
    m_attenuation.m_quadratic = _quadratic;
  }


  PointLight::~PointLight()
  {
  }

  void PointLight::Init(const glm::vec3& _position,
    const float& _ambient, const float& _diffuse, const float& _specular,
    float _constant, float _linear, float _quadratic)
  {
    m_position  = _position;
    m_ambient   = _ambient;
    m_diffuse   = _diffuse;
    m_specular  = _specular;
    m_attenuation.m_constant = _constant;
    m_attenuation.m_linear = _linear;
    m_attenuation.m_quadratic = _quadratic;
  }
  void PointLight::UploadToShader(GLSLProgram& _shader, const std::string& _uniformName)
  {
    _shader.UploadValue(_uniformName + ".position", m_position);
    _shader.UploadValue(_uniformName + ".ambient", m_ambient);
    _shader.UploadValue(_uniformName + ".diffuse", m_diffuse);
    _shader.UploadValue(_uniformName + ".specular", m_specular);
    _shader.UploadValue(_uniformName + ".constant", m_attenuation.m_constant);
    _shader.UploadValue(_uniformName + ".linear", m_attenuation.m_linear);
    _shader.UploadValue(_uniformName + ".quadratic", m_attenuation.m_quadratic);
    _shader.UploadValue(_uniformName + ".color", m_color);

  }


  // ------------------------- SPOT LIGHT ------------------------------------

  SpotLight::SpotLight(const glm::vec3& _position, const glm::vec3& _direction,
    const float& _ambient, const float& _diffuse, const float& _specular,
    float _constant, float _linear, float _quadratic,
    float _cutOff, float _outerCutOff) : m_cutOff(_cutOff), m_outerCutOff(_outerCutOff)
  {
    m_position = _position;
    m_direction = _direction;
    m_ambient = _ambient;
    m_diffuse = _diffuse;
    m_specular = _specular;
    m_attenuation.m_constant = _constant;
    m_attenuation.m_linear = _linear;
    m_attenuation.m_quadratic = _quadratic;
  }

  SpotLight::~SpotLight()
  {
  }

  void SpotLight::Init(const glm::vec3& _position, const glm::vec3& _direction,
    const float& _ambient, const float& _diffuse, const float& _specular,
    float _constant, float _linear, float _quadratic,
    float _cutOff, float _outerCutOff)
  {
    m_position = _position;
    m_direction = _direction;
    m_ambient = _ambient;
    m_diffuse = _diffuse;
    m_specular = _specular;
    m_attenuation.m_constant = _constant;
    m_attenuation.m_linear = _linear;
    m_attenuation.m_quadratic = _quadratic;
    m_cutOff = _cutOff;
    m_outerCutOff = _outerCutOff;
  }
  void SpotLight::UploadToShader(GLSLProgram& _shader, const std::string& _uniformName)
  {
    _shader.UploadValue(_uniformName + ".position", m_position);
    _shader.UploadValue(_uniformName + ".direction", m_direction);
    _shader.UploadValue(_uniformName + ".ambient", m_ambient);
    _shader.UploadValue(_uniformName + ".diffuse", m_diffuse);
    _shader.UploadValue(_uniformName + ".specular", m_specular);
    _shader.UploadValue(_uniformName + ".constant", m_attenuation.m_constant);
    _shader.UploadValue(_uniformName + ".linear", m_attenuation.m_linear);
    _shader.UploadValue(_uniformName + ".quadratic", m_attenuation.m_quadratic);
    _shader.UploadValue(_uniformName + ".cutOff", m_cutOff);
    _shader.UploadValue(_uniformName + ".outerCutOff", m_outerCutOff);
    _shader.UploadValue(_uniformName + ".color", m_color);
  }

  // ------------------------- DIRECTIONAL LIGHT ------------------------------------

  DirectionalLight::DirectionalLight(const glm::vec3& _direction, const float& _ambient, const float& _diffuse, const float& _specular)
  {
    m_direction = _direction;
    m_ambient = _ambient;
    m_diffuse = _diffuse;
    m_specular = _specular;
  }

  DirectionalLight::~DirectionalLight()
  {
  }

  void DirectionalLight::Init(const glm::vec3& _direction, const float& _ambient, const float& _diffuse, const float& _specular)
  {
    m_direction = _direction;
    m_ambient = _ambient;
    m_diffuse = _diffuse;
    m_specular = _specular;
  }
  void DirectionalLight::UploadToShader(GLSLProgram& _shader, const std::string& _uniformName)
  {
    _shader.UploadValue(_uniformName + ".direction", m_direction);
    _shader.UploadValue(_uniformName + ".ambient", m_ambient);
    _shader.UploadValue(_uniformName + ".diffuse", m_diffuse);
    _shader.UploadValue(_uniformName + ".specular", m_specular);
    _shader.UploadValue(_uniformName + ".color", m_color);
  }
}
#include "Lights.h"

namespace GameEngine
{
  // ------------------------- POINT LIGHT ------------------------------------

  PointLight::PointLight(const glm::vec3& _position,
    const glm::vec3& _ambient, const glm::vec3& _diffuse, const glm::vec3& _specular,
    float _constant, float _linear, float _quadratic) :
    m_position(_position),
    m_ambient(_ambient), m_diffuse(_diffuse), m_specular(_specular),
    m_constant(_constant), m_linear(_linear), m_quadratic(_quadratic)
  {
  }


  PointLight::~PointLight()
  {
  }

  void PointLight::Init(const glm::vec3& _position,
    const glm::vec3& _ambient, const glm::vec3& _diffuse, const glm::vec3& _specular,
    float _constant, float _linear, float _quadratic)
  {
    m_position  = _position;
    m_ambient   = _ambient;
    m_diffuse   = _diffuse;
    m_specular  = _specular;
    m_constant  = _constant;
    m_linear    = _linear;
    m_quadratic = _quadratic;
  }
  void PointLight::UploadToShader(GLSLProgram& _shader, const std::string& _uniformName)
  {
    glUniform3f(_shader.GetUniformLocation(_uniformName + ".position"), m_position.x, m_position.y, m_position.z);
    glUniform3f(_shader.GetUniformLocation(_uniformName + ".ambient"), m_ambient.x, m_ambient.y, m_ambient.z);
    glUniform3f(_shader.GetUniformLocation(_uniformName + ".diffuse"), m_diffuse.x, m_diffuse.y, m_diffuse.z);
    glUniform3f(_shader.GetUniformLocation(_uniformName + ".specular"), m_specular.x, m_specular.y, m_specular.z);
    glUniform1f(_shader.GetUniformLocation(_uniformName + ".constant"), m_constant);
    glUniform1f(_shader.GetUniformLocation(_uniformName + ".linear"), m_linear);
    glUniform1f(_shader.GetUniformLocation(_uniformName + ".quadratic"), m_quadratic);
  }


  // ------------------------- SPOT LIGHT ------------------------------------

  SpotLight::SpotLight(const glm::vec3& _position, const glm::vec3& _direction,
    const glm::vec3& _ambient, const glm::vec3& _diffuse, const glm::vec3& _specular,
    float _constant, float _linear, float _quadratic,
    float _cutOff, float _outerCutOff) :
    m_position(_position), m_direction(_direction),
    m_ambient(_ambient), m_diffuse(_diffuse), m_specular(_specular),
    m_constant(_constant), m_linear(_linear), m_quadratic(_quadratic),
    m_cutOff(_cutOff), m_outerCutOff(_outerCutOff)
  {
  }

  SpotLight::~SpotLight()
  {
  }

  void SpotLight::Init(const glm::vec3& _position, const glm::vec3& _direction,
    const glm::vec3& _ambient, const glm::vec3& _diffuse, const glm::vec3& _specular,
    float _constant, float _linear, float _quadratic,
    float _cutOff, float _outerCutOff)
  {
    m_position = _position;
    m_direction = _direction;
    m_ambient = _ambient;
    m_diffuse = _diffuse;
    m_specular = _specular;
    m_constant = _constant;
    m_linear = _linear;
    m_quadratic = _quadratic;
    m_cutOff = _cutOff;
    m_outerCutOff = _outerCutOff;
  }
  void SpotLight::UploadToShader(GLSLProgram& _shader, const std::string& _uniformName)
  {
    glUniform3f(_shader.GetUniformLocation(_uniformName + ".position"), m_position.x, m_position.y, m_position.z);
    glUniform3f(_shader.GetUniformLocation(_uniformName + ".direction"), m_direction.x, m_direction.y, m_direction.z);
    glUniform3f(_shader.GetUniformLocation(_uniformName + ".ambient"), m_ambient.x, m_ambient.y, m_ambient.z);
    glUniform3f(_shader.GetUniformLocation(_uniformName + ".diffuse"), m_diffuse.x, m_diffuse.y, m_diffuse.z);
    glUniform3f(_shader.GetUniformLocation(_uniformName + ".specular"), m_specular.x, m_specular.y, m_specular.z);
    glUniform1f(_shader.GetUniformLocation(_uniformName + ".constant"), m_constant);
    glUniform1f(_shader.GetUniformLocation(_uniformName + ".linear"), m_linear);
    glUniform1f(_shader.GetUniformLocation(_uniformName + ".quadratic"), m_quadratic);
    glUniform1f(_shader.GetUniformLocation(_uniformName + ".cutOff"), m_cutOff);
    glUniform1f(_shader.GetUniformLocation(_uniformName + ".outerCutOff"), m_outerCutOff);
  }

  // ------------------------- DIRECTIONAL LIGHT ------------------------------------

  DirectionalLight::DirectionalLight(const glm::vec3& _direction, const glm::vec3& _ambient, const glm::vec3& _diffuse, const glm::vec3& _specular) :
    m_direction(_direction), m_ambient(_ambient), m_diffuse(_diffuse), m_specular(_specular)
  {
  }

  DirectionalLight::~DirectionalLight()
  {
  }

  void DirectionalLight::Init(const glm::vec3& _direction, const glm::vec3& _ambient, const glm::vec3& _diffuse, const glm::vec3& _specular)
  {
    m_direction = _direction;
    m_ambient = _ambient;
    m_diffuse = _diffuse;
    m_specular = _specular;
  }
  void DirectionalLight::UploadToShader(GLSLProgram& _shader, const std::string& _uniformName)
  {
    glUniform3f(_shader.GetUniformLocation(_uniformName + ".direction"), m_direction.x, m_direction.y, m_direction.z);
    glUniform3f(_shader.GetUniformLocation(_uniformName + ".ambient"), m_ambient.x, m_ambient.y, m_ambient.z);
    glUniform3f(_shader.GetUniformLocation(_uniformName + ".diffuse"), m_diffuse.x, m_diffuse.y, m_diffuse.z);
    glUniform3f(_shader.GetUniformLocation(_uniformName + ".specular"), m_specular.x, m_specular.y, m_specular.z);
  }
}
#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
namespace GameEngine
{
  class Camera3D
  {
  public:
    Camera3D();
    ~Camera3D();

    void Init(float _fov, int _screenWidth, int _screenHeight);
    void Update(glm::vec2 _mousePos);

    //setters
    void SetPosition(const glm::vec3& _newPosition)
    {
      m_position = _newPosition;
      m_needsMatrixUpdate = true;
    }
    void SetScale(float _newScale)
    {
      m_scale = _newScale;
      m_needsMatrixUpdate = true;
    }
    void SetScreenValues(float _fov, int _screenWidth, int _screenHeight)
    {
      m_fieldOfView = _fov;
      m_screenHeight = _screenHeight;
      m_screenWidth = _screenWidth;
      m_needsMatrixUpdate = true;
    }

    //getters
    float GetAspectRatio() const
    {
      return (float)m_screenWidth / (float)m_screenHeight;
    }
    glm::mat4 GetProjectionMatrix()
    {
      return m_projectionMatrix;
    }
    glm::mat4 GetViewMatrix()
    {
      return m_viewMatrix;
    }
    float GetScale()
    {
      return m_scale;
    }
    glm::vec3 GetPosition() const { return m_position; }

  private:
    glm::mat4 m_projectionMatrix; // The projection matrix
    glm::mat4 m_viewMatrix; // Camera matrix

    glm::vec3 m_position;


    float m_fieldOfView;
    float m_scale;

    int m_screenWidth;
    int m_screenHeight;

    bool m_needsMatrixUpdate;

    float horizontalAngle = 3.14f;
    // Initial vertical angle : none
    float verticalAngle = 0.0f;

    float mouseSpeed = 0.005f;
  };

}
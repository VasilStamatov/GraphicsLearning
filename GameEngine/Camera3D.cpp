#include "Camera3D.h"
#include <SDL\SDL_mouse.h>

namespace GameEngine
{
  Camera3D::Camera3D() :
    m_position(0.0f, 0.0f, 3.0f),
    m_projectionMatrix(1.0f),
    m_viewMatrix(1.0f),
    m_scale(1.0f),
    m_fieldOfView(45.0f),
    m_screenWidth(500.0f),
    m_screenHeight(500.0f),
    m_needsMatrixUpdate(true)
  {

  }


  Camera3D::~Camera3D()
  {
  }

  void Camera3D::Init(float _fov, int _screenWidth, int _screenHeight)
  {
    m_screenWidth = _screenWidth;
    m_screenHeight = _screenHeight;
    m_fieldOfView = _fov;
    //projection matrix
    m_projectionMatrix = glm::perspective(m_fieldOfView, GetAspectRatio(), 0.1f, 100.0f);
    SDL_SetRelativeMouseMode(SDL_bool::SDL_TRUE);
  }
  void Camera3D::Update()
  {
    if (m_needsMatrixUpdate)
    {
      int x, y;
      SDL_GetMouseState(&x, &y);

      horizontalAngle += mouseSpeed * float(m_screenWidth / 2 - x);
      verticalAngle += mouseSpeed * float(m_screenHeight / 2 - y);

      // Direction : Spherical coordinates to Cartesian coordinates conversion
      glm::vec3 direction(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
        );
      // Right vector
      glm::vec3 right = glm::vec3(
        sin(horizontalAngle - 3.14f / 2.0f),
        0,
        cos(horizontalAngle - 3.14f / 2.0f)
        );
      // Up vector
      glm::vec3 up = glm::cross(right, direction);

      //Camera Translation
      glm::vec3 translate(-m_position.x + m_screenWidth / 2, -m_position.y + m_screenHeight / 2, -m_position.z);
      //View matrix is translated
      m_viewMatrix = glm::lookAt(
        translate,           // Camera is here
        translate + direction, // and looks here : at the same position, plus "direction"
        up                  // Head is up (set to 0,-1,0 to look upside-down)
        );

      //Camera Scale
      glm::vec3 scale(m_scale, m_scale, m_scale);
      //multiply the camera matrix by the scale matrix
      m_viewMatrix = glm::scale(glm::mat4(1.0f), scale) * m_viewMatrix;

      glm::mat4 modelMatrix(1.0f);

      m_MVP = m_projectionMatrix * m_viewMatrix * modelMatrix;
      
      m_needsMatrixUpdate = false;
    }
  }
}
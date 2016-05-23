#include "Camera3D.h"
#include <SDL\SDL_mouse.h>
#include <SDL\SDL_events.h>
namespace GameEngine
{
  Camera3D::Camera3D() :
    m_position(0, 0, 5),
    m_projectionMatrix(1.0f),
    m_viewMatrix(1.0f),
    m_screenWidth(500),
    m_screenHeight(500)
  {
    //camera direction
    m_direction = glm::vec3(
      cos(m_verticalAngle) * sin(m_horizontalAngle),
      sin(m_verticalAngle),
      cos(m_verticalAngle) * cos(m_horizontalAngle)
      );

    // Right vector
    m_right = glm::vec3(
      sin(m_horizontalAngle - glm::radians(90.0f)),
      0.0f,
      cos(m_horizontalAngle - glm::radians(90.0f))
      );
    // Up vector : perpendicular to both direction and right
    m_up = glm::cross(m_right, m_direction);
  }


  Camera3D::~Camera3D()
  {
  }

  void Camera3D::Init(float _fov, int _screenWidth, int _screenHeight, const SDL_bool& _relativeMM)
  {
    m_screenWidth = _screenWidth;
    m_screenHeight = _screenHeight;
    m_initialFoV = _fov;
    //projection matrix
    m_projectionMatrix = glm::perspective(m_initialFoV, GetAspectRatio(), 0.1f, 100.0f);

    SDL_SetRelativeMouseMode(_relativeMM);
  }

  void Camera3D::Update()
  {
    if (m_needsMatrixUpdate)
    {
      m_viewMatrix = glm::lookAt(m_position, m_position + m_direction, m_up);
    }
  }
  void Camera3D::Move(const MoveState& _ms, float _fps)
  {
    float deltaTime = 1.0f;
    if (_fps != 0.0f)
    {
      deltaTime = 1.0f / _fps;
    }
    switch (_ms)
    {
    case MoveState::FORWARD:
      m_position += m_direction * deltaTime * m_speed;
      break;
    case MoveState::BACKWARD:
      m_position -= m_direction * deltaTime * m_speed;
      break;
    case MoveState::LEFT:
      m_position -= m_right * deltaTime * m_speed;
      break;
    case MoveState::RIGHT:
      m_position += m_right * deltaTime * m_speed;
      break;
    case MoveState::UP:
      m_position += m_up * deltaTime * m_speed;
      break;
    case MoveState::DOWN:
      m_position -= m_up * deltaTime * m_speed;
      break;
    default:
      break;
    }
    m_needsMatrixUpdate = true;
  }

  void Camera3D::Rotate(float _xrel, float _yrel, float _fps)
  {
    float deltaTime = 1.0f;
    if (_fps != 0.0f)
    {
      deltaTime = 1.0f / _fps;
    }

    m_horizontalAngle -= m_mouseSpeed * deltaTime * _xrel;

    m_verticalAngle -= m_mouseSpeed * deltaTime * _yrel;

    // Restrict the verticle angle rotations (looking up and down) so you don't make air-rolls
    if (m_verticalAngle < -glm::radians(90.0f))
    {
      m_verticalAngle = -glm::radians(90.0f);
    }
    else if (m_verticalAngle > glm::radians(90.0f))
    {
      m_verticalAngle = glm::radians(90.0f);
    }
    // Keep the horizontal rotation range always between 0 and 360 degrees (but in radians ofc) so it doesn't go too high or low
    if (m_horizontalAngle < 0.0f)
    {
      m_horizontalAngle += glm::radians(360.0f);
    }
    else if (m_horizontalAngle > glm::radians(360.0f))
    {
      m_horizontalAngle -= glm::radians(360.0f);
    }

    m_direction = glm::vec3(
      cos(m_verticalAngle) * sin(m_horizontalAngle),
      sin(m_verticalAngle),
      cos(m_verticalAngle) * cos(m_horizontalAngle));

    // Right vector
    m_right = glm::vec3(
      sin(m_horizontalAngle - glm::radians(90.0f)),
      0.0f,
      cos(m_horizontalAngle - glm::radians(90.0f))
      );
    // Up vector : perpendicular to both direction and right
    m_up = glm::cross(m_right, m_direction);

    m_needsMatrixUpdate = true;
  }
}
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

  }


  Camera3D::~Camera3D()
  {
  }

  void Camera3D::Init(float _fov, int _screenWidth, int _screenHeight)
  {
    m_screenWidth = _screenWidth;
    m_screenHeight = _screenHeight;
    initialFoV = _fov;
    //projection matrix
    m_projectionMatrix = glm::perspective(initialFoV, GetAspectRatio(), 0.1f, 100.0f);

    SDL_SetRelativeMouseMode(SDL_TRUE);
    ;
  }

  void Camera3D::Update(float _fps)
  {
    float deltaTime = 1.0f;
    if (_fps != 0.0f)
    {
      deltaTime = 1.0f / _fps;
    }

    glm::vec3 direction(
      cos(verticalAngle) * sin(horizontalAngle),
      sin(verticalAngle),
      cos(verticalAngle) * cos(horizontalAngle)
      );

    // Right vector
    glm::vec3 right = glm::vec3(
      sin(horizontalAngle - glm::radians(90.0f)),
      0.0f,
      cos(horizontalAngle - glm::radians(90.0f))
      );
    // Up vector : perpendicular to both direction and right
    glm::vec3 up = glm::cross(right, direction);

    SDL_Event evnt;
    while (SDL_PollEvent(&evnt))
    {
      switch (evnt.type)
      {
      case SDL_MOUSEMOTION:
        // Compute new orientation
        horizontalAngle -= mouseSpeed * deltaTime * evnt.motion.xrel;
        verticalAngle -= mouseSpeed * deltaTime * evnt.motion.yrel;

        // Restrict the verticle angle rotations (looking up and down) so you don't make air-rolls
        if (verticalAngle < -glm::radians(90.0f))
        {
          verticalAngle = -glm::radians(90.0f);
        }
        else if (verticalAngle > glm::radians(90.0f))
        {
          verticalAngle = glm::radians(90.0f);
        }
        // Keep the horizontal rotation range always between 0 and 360 degrees (but in radians ofc) so it doesn't go too high or low
        if (horizontalAngle < 0.0f)
        {
          horizontalAngle += glm::radians(360.0f);
        }
        else if (horizontalAngle > glm::radians(360.0f))
        {
          horizontalAngle -= glm::radians(360.0f);
        }
        break;
      case SDL_KEYDOWN:
        inputManager.PressKey(evnt.key.keysym.sym);
        break;
      case SDL_KEYUP:
        inputManager.ReleaseKey(evnt.key.keysym.sym);
        break;
      case SDL_MOUSEBUTTONDOWN:
        inputManager.PressKey(evnt.button.button);
        break;
      case SDL_MOUSEBUTTONUP:
        inputManager.ReleaseKey(evnt.button.button);
        break;
      case SDL_MOUSEWHEEL:
        initialFoV -= 5 * evnt.wheel.y;
        printf("FOV angle: %f \n", initialFoV);
      default:
        break;
      }
    }
    if (inputManager.IsKeyDown(SDLK_w))
    {
      m_position += direction * deltaTime * speed;
    }
    if (inputManager.IsKeyDown(SDLK_a))
    {
      m_position -= right * deltaTime * speed;
    }
    if (inputManager.IsKeyDown(SDLK_s))
    {
      m_position -= direction * deltaTime * speed;
    }
    if (inputManager.IsKeyDown(SDLK_d))
    {
      m_position += right * deltaTime * speed;
    }
    m_viewMatrix = glm::lookAt(m_position, m_position + direction, up);
    m_projectionMatrix = glm::perspective(initialFoV, GetAspectRatio(), 0.1f, 100.0f);
  }
}
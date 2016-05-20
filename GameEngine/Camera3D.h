#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <SDL\SDL_video.h>
#include "InputManager.h"

namespace GameEngine
{
  class Camera3D
  {
  public:
    Camera3D();
    ~Camera3D();

    /** \brief The init function for the camera
     * \param _fov - the field of view (for the frustum)
     * \param _screenWidth - the width of the sdl screen
     * \param _screenHeight - the height of the sdl screen
     */
    void Init(float _fov, int _screenWidth, int _screenHeight);

    /** \brief The per-frame function for the camera
     * \param _fps - the current fps on which the game is runnign (for deltaTime)
     */
    void Update(float _fps);

    /** \brief Gets the aspect ratio of the screen
     * \return the division of SW / SH
     */
    float GetAspectRatio() const
    {
      return (float)m_screenWidth / (float)m_screenHeight;
    }

    /** \brief Gets the projection matrix of the camera
     * \return the projection matrix
     */
    glm::mat4 GetProjectionMatrix()
    {
      return m_projectionMatrix;
    }

    /** \brief Gets the view matrix of the camera
     * \return the view matrix
     */
    glm::mat4 GetViewMatrix()
    {
      return m_viewMatrix;
    }

    /** \brief Gets the position vec3 of the camera
     * \return the position vec3
     */
    glm::vec3 GetPosition() const { return m_position; }

  private:
    InputManager inputManager; ///< Handles events

    glm::mat4 m_projectionMatrix; ///< The projection matrix
    glm::mat4 m_viewMatrix; ///< Camera matrix

    glm::vec3 m_position; ///< camera position

    /// horizontal angle : toward -Z
    float horizontalAngle = 3.14f;

    /// vertical angle : 0, look at the horizon
    float verticalAngle = 0.0f;

    /// Initial Field of View
    float initialFoV = 45.0f;

    float speed = 3.0f; ///< speed of 3 units / second

    float mouseSpeed = 0.0322f; ///< mouse speed/sensitivity

    int m_screenWidth; ///< The width of the sdl screen
    int m_screenHeight; ///< The height of the sdl screen
  };
}

#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <SDL\SDL_video.h>
#include "InputManager.h"

namespace GameEngine
{
  enum class MoveState { FORWARD, BACKWARD, LEFT, RIGHT };

  class Camera3D
  {
  public:
    Camera3D();
    ~Camera3D();

    /** \brief The init function for the camera
     * \param _fov - the field of view (for the frustum)
     * \param _screenWidth - the width of the sdl screen
     * \param _screenHeight - the height of the sdl screen
     * \param _relativeMM - pass true if the camera needs to be in relative mouse mode from SDL
     */
    void Init(float _fov, int _screenWidth, int _screenHeight, const SDL_bool& _relativeMM = SDL_FALSE);

    /** \brief The per-frame function for the camera which updates the view matrix if needed
     */
    void Update();

    /** \brief Sets the camera position to a specific one
    * \param _newPos - the new position vector
    */
    void SetPosition(const glm::vec3& _newPos)
    {
      m_position = _newPos;
    }

    /** \brief Translates the camera (can go +/- x axis and +/- z axis)
    * \param _ms - the enum which tells the function how to translate the camera. possible values:
    * - FORWARD - translates towards the +z axis by speed * deltaTime
    * - BACKWARD - translates towards the -z axis by speed * deltaTime
    * - LEFT - translates towards the -x axis by speed * deltaTime
    * - RIGHT - - translates towards the +x axis by speed * deltaTime
    * \param _fps - the current fps
    */
    void Move(const MoveState& _ms, float _fps);

    /** \brief rotates the camera either horizontally or vertically
    * \param _xrel - the value by which the camera rotates horizontally
    * \param _yrel - the value by which the camera rotates vertically
    * \param _fps - the current fps
    */
    void Rotate(float _xrel, float _yrel, float _fps);

    /** \brief Changes the FoV of the projection matrix and updates it
    * \param _value - the new FoV value
    */
    void ChangeFoV(float _value)
    {
      m_initialFoV -= _value;
      m_projectionMatrix = glm::perspective(m_initialFoV, GetAspectRatio(), 0.1f, 100.0f);
    }

    /** \brief Increase (or decreases) the mouse sensitivity
    * \param _val - the value by which the sensitivity is increased(decreased if negative)
    */
    void IncreaseMouseSensitivity(float _val)
    {
      m_mouseSpeed += _val;
    }

    /** \brief Increase (or decreases) the move speed of the camera
    * \param _val - the value by which the speed is increased(decreased if negative)
    */
    void IncreaseMovespeed(float _val)
    {
      m_speed += _val;
    }

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
    InputManager m_inputManager; ///< Handles events

    glm::mat4 m_projectionMatrix; ///< The projection matrix
    glm::mat4 m_viewMatrix; ///< Camera matrix

    glm::vec3 m_position; ///< camera position

    /// horizontal angle : toward -Z
    float m_horizontalAngle = glm::radians(180.0f);

    /// vertical angle : 0, look at the horizon
    float m_verticalAngle = 0.0f;

    //axis vectors
    glm::vec3 m_direction;
    glm::vec3 m_right;
    glm::vec3 m_up;

    //Is true if a value of the projection matrix is changed
    bool m_needsMatrixUpdate = true;

    /// Initial Field of View
    float m_initialFoV = 45.0f;

    float m_speed = 3.0f; ///< speed of 3 units / second

    float m_mouseSpeed = 0.1f; ///< mouse speed/sensitivity

    int m_screenWidth; ///< The width of the sdl screen
    int m_screenHeight; ///< The height of the sdl screen
  };
}

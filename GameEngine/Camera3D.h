#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <SDL\SDL_video.h>
#include "InputManager.h"

namespace GameEngine
{
  /** The movement states of the camera */
  enum class MoveState { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

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

    /** \brief The per-frame function for the camera which updates the view matrix if needed */
    void Update();

    /** \brief Sets the camera position to a specific one
    * \param _newPos - the new position vector
    */
    void SetPosition(const glm::vec3& _newPos)
    {
      m_position = _newPos;
      m_needsMatrixUpdate = true;
    }
    /** \brief offsets the position of the camera
    * \param _offset - the offset which the position will add
    */
    void OffsetPosition(const glm::vec3& _offset) { m_position += _offset; m_needsMatrixUpdate = true; }

    /** \brief Sets the camera orientation to a specific one
    * \param _horizontal - the new horizontal orientation (degrees)
    * \param _vertical - the new vertical orientation (degrees)
    */
    void SetOrientation(float _horizontal, float _vertical)
    {
      m_horizontalAngle = glm::radians(_horizontal);
      m_verticalAngle = glm::radians(_vertical);

      CalculateOrientation(false);

      m_needsMatrixUpdate = true;
    }
    /** \brief offsets the orientation of the camera
    * \param _horizontal - the horizontal offset (degrees)
    * \param _vertical - the vertical offset (degrees)
    */
    void OffsetOrientation(float _horizontal, float _vertical)
    {
      m_horizontalAngle += glm::radians(_horizontal);
      m_verticalAngle += glm::radians(_vertical);

      CalculateOrientation(false);

      m_needsMatrixUpdate = true;
    }

    /** \brief Translates the camera (can go +/- x axis and +/- z axis)
    * \param _ms - the enum which tells the function how to translate the camera. possible values:
    * - FORWARD - translates towards the +z axis by speed * deltaTime
    * - BACKWARD - translates towards the -z axis by speed * deltaTime
    * - LEFT - translates towards the -x axis by speed * deltaTime
    * - RIGHT - - translates towards the +x axis by speed * deltaTime
    * - UP - translates towards the +y axis by speed * deltaTime
    * - DOWN - - translates towards the -y axis by speed * deltaTime
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
      if (m_initialFoV < 1.0f)
      {
        m_initialFoV = 1.0f;
      }
      else if (m_initialFoV > 60.0f)
      {
        m_initialFoV = 60.0f;
      }
      printf("fov: %f \n", m_initialFoV);
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

    /** \brief Gets the direction (always facing forward) vec3 of the camera
      * \return the direction vec3
      */
    glm::vec3 GetDirection() const { return m_direction; }

  private:
    void CalculateOrientation(bool _limit);

    InputManager m_inputManager; ///< Handles events

    glm::mat4 m_projectionMatrix; ///< The projection matrix
    glm::mat4 m_viewMatrix; ///< Camera matrix

    glm::vec3 m_position; ///< camera position

    /// horizontal angle : toward -Z
    float m_horizontalAngle = glm::radians(180.0f); //yaw

    /// vertical angle : 0, look at the horizon
    float m_verticalAngle = 0.0f; //pitch

    //axis vectors
    glm::vec3 m_direction; //The direction the player is facing
    glm::vec3 m_right; // the axis to the right of the player of where he's facing
    glm::vec3 m_up; //the axis up from the player of where he's facing

    //Is true if a value of the projection matrix is changed
    bool m_needsMatrixUpdate = true;

    /// Initial Field of View
    float m_initialFoV = 45.0f;

    float m_speed = 4.0f; ///< speed of 3 units / second

    float m_mouseSpeed = 0.5f; ///< mouse speed/sensitivity

    int m_screenWidth; ///< The width of the sdl screen
    int m_screenHeight; ///< The height of the sdl screen
  };
}

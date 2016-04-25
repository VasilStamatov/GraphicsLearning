#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

namespace GameEngine
{
  class Camera2D
  {
  public:
    Camera2D();
    ~Camera2D();

    //sets up the orthographic matrix and screen dimensions
    void Init(int _screenWidth, int _screenHeight);

    //updates the camera matrix if needed
    void Update();

    //converts from screen space (0,0 topleft to SW,SH botright) to world space (0,0 in the middle and pos is relative to the cam)
    glm::vec2 ConvertScreenToWorld(glm::vec2 _screenCords);

    //checks if the box is in view
    bool IsBoxInView(const glm::vec2& _position, const glm::vec2& _dimensions);

    //offsets the position of the camera
    void OffsetPosition(const glm::vec2& _offset) { m_position += _offset; m_needsMatrixUpdate = true; }

    //offsets the scale of the camera (as in goes further away or in)
    void OffsetScale(float _offset) { m_scale += _offset; if (m_scale < 0.001f) m_scale = 0.001f; m_needsMatrixUpdate = true; }

    //setters

    void SetPosition(const glm::vec2& _newPosition)
    {
      m_position = _newPosition;
      m_needsMatrixUpdate = true;
    }

    void SetScale(float _newScale)
    {
      m_scale = _newScale;
      m_needsMatrixUpdate = true;
    }

    //getters
    glm::vec2 GetPosition()
    {
      return m_position;
    }
    float GetScale()
    {
      return m_scale;
    }
    glm::mat4 GetCameraMatrix()
    {
      return m_cameraMatrix;
    }

    float GetAspectRatio() const
    {
      return (float)m_screenWidth / (float)m_screenHeight;
    }

  private:
    int m_screenWidth;
    int m_screenHeight;

    bool m_needsMatrixUpdate;

    float m_scale;

    glm::vec2 m_position;
    glm::mat4 m_cameraMatrix;
    glm::mat4 m_orthoMatrix;

  };

}

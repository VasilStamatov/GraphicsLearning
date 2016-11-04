#include "Camera2D.h"

namespace GameEngine
{ //initialize the camera with some default values to avoid errors
  Camera2D::Camera2D()
  {
  }


  Camera2D::~Camera2D()
  {
  } 

  void Camera2D::Init(int _screenWidth, int _screenHeight)
  {
    //initialize the screen widght and height and create the ortho matrix
    m_screenWidth = _screenWidth;
    m_screenHeight = _screenHeight;
    //0,0 bottomleft; SW/SH topright and suggested values for near and farshore
    m_orthoMatrix = glm::ortho(0.0f, (float)m_screenWidth, 0.0f, (float)m_screenHeight, 0.1f, 100.0f);
  }

  //updates the camera matrix if needed
  void Camera2D::Update()
  {
    //Only update if our position or scale have changed
    if (m_needsMatrixUpdate)
    {
      //Camera Translation
      glm::vec3 translate(-m_position.x + m_screenWidth / 2, -m_position.y + m_screenHeight / 2, 0.0f);
      m_cameraMatrix = glm::translate(m_orthoMatrix, translate);

      //Camera Scale
      glm::vec3 scale(m_scale, m_scale, 0.0f);
      //multiply the camera matrix by the scale matrix
      m_cameraMatrix = glm::scale(glm::mat4(1.0f), scale) * m_cameraMatrix;

      m_needsMatrixUpdate = false;
    }
  }
		void Camera2D::Resize(int _screenWidth, int _screenHeight)
		{
				Init(_screenWidth, _screenHeight);
		}
  glm::vec2 Camera2D::ConvertScreenToWorld(glm::vec2 _screenCords)
  {
    //invert y direction
    _screenCords.y = m_screenHeight - _screenCords.y;
    //make it so that 0 is the center
    _screenCords -= glm::vec2(m_screenWidth / 2.0f, m_screenHeight / 2.0f);
    //scale the coordinates
    _screenCords /= m_scale;
    //translate the camera position
    _screenCords += m_position;
    return _screenCords;
  }

  // simple AABB test to see if the box is in the camera
  bool Camera2D::IsBoxInView(const glm::vec2& _position, const glm::vec2& _dimensions)
  {
    glm::vec2 scaledScreenDimensions = glm::vec2(m_screenWidth, m_screenHeight) / (m_scale);

    // The minimum distance before a collision occurs
    const float MIN_DISTANCE_X = _dimensions.x / 2.0f + scaledScreenDimensions.x / 2.0f;
    const float MIN_DISTANCE_Y = _dimensions.y / 2.0f + scaledScreenDimensions.y / 2.0f;

    // Center position of the parameters
    glm::vec2 centerPos = _position + _dimensions / 2.0f;
    // Center pos of the camera
    glm::vec2 centerCameraPos = m_position;

    // Vector from the input to the camera
    glm::vec2 distVec = centerPos - centerCameraPos;

    // Get the depth of the collision
    float xDepth = MIN_DISTANCE_X - abs(distVec.x);
    float yDepth = MIN_DISTANCE_Y - abs(distVec.y);
    // If either the depths are > 0, then we collided
    if (xDepth > 0 && yDepth > 0)
    {
      ///there was a collision
      return true;
    }
    return false;
  }
}

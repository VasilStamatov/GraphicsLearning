#pragma once
#include <GL/glew.h>
#include <glm\vec3.hpp>
#include <glm\vec2.hpp>

namespace GameEngine
{
  //color struct
  struct ColorRGBA8
  {
    ColorRGBA8() : r(0), g(0), b(0), a(0) { }
    ColorRGBA8(GLubyte R, GLubyte G, GLubyte B, GLubyte A) : 
      r(R), g(G), b(B), a(A) { }
    GLubyte r;
    GLubyte g;
    GLubyte b;
    GLubyte a;
  };

  //The vertex definition
  struct Vertex
  {
    //vertex position
    glm::vec3 m_position;

    //vertex normal
    glm::vec3 m_normal;

    //vertex texture coordinates
    glm::vec2 m_uv;

    //vertex color
    ColorRGBA8 m_color;

    void SetPosition(const glm::vec3& _position)
    {
      m_position = _position;
    }
    void SetPosition(float _x, float _y, float _z)
    {
      m_position = glm::vec3(_x, _y, _z);
    }

    void SetNormal(const glm::vec3& _normal)
    {
      m_normal = _normal;
    }
    void SetNormal(float _x, float _y, float _z)
    {
      m_normal = glm::vec3(_x, _y, _z);
    }

    void SetUV(const glm::vec2& _uv)
    {
      m_uv = _uv;
    }
    void SetUV(float _x, float _y)
    {
      m_uv = glm::vec2(_x, _y);
    }
    void SetColor(const GLubyte& _r, const GLubyte& _g, const GLubyte& _b, const GLubyte& _a)
    {
      m_color.r = _r;
      m_color.g = _g;
      m_color.b = _b;
      m_color.a = _a;
    }
  };
}

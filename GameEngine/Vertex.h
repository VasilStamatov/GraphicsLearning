#pragma once
#include <GL/glew.h>
#include <glm\vec3.hpp>
#include <glm\vec2.hpp>
#include <glm\vec4.hpp>

namespace GameEngine
{
  //color struct
  struct ColorRGBA8
  {
    ColorRGBA8() : r(0), g(0), b(0), a(0) { }
    ColorRGBA8(GLubyte R, GLubyte G, GLubyte B, GLubyte A) :
      r(R), g(G), b(B), a(A) { }
    ColorRGBA8(GLubyte _rgb, GLubyte _alpha) :
      r(_rgb), g(_rgb), b(_rgb), a(_alpha) { }

    GLubyte r;
    GLubyte g;
    GLubyte b;
    GLubyte a;
  };

  //The vertex definition
  struct Vertex
  {
    Vertex() {}
    Vertex(const glm::vec3& _position, const glm::vec3& _normal, const glm::vec2& _uv, const ColorRGBA8& _color, const glm::vec3& _tangents)
    {
      m_position = _position;
      m_normal = _normal;
      m_uv = _uv;
      m_color = _color;
      m_tangents = _tangents;
    }
    //vertex position
    glm::vec3 m_position;

    //vertex normal
    glm::vec3 m_normal;

    //vertex texture coordinates
    glm::vec2 m_uv;

    //vertex color
    ColorRGBA8 m_color;

    glm::vec3 m_tangents;

    glm::ivec4 m_boneIDs;

    glm::vec4 m_weights;

    void SetBoneIDs(const glm::ivec4& _boneID)
    {
      m_boneIDs = _boneID;
    }
    void SetBoneWeights(const glm::vec4& _weights)
    {
      m_weights = _weights;
    }

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

    void SetTangents(const glm::vec3& _tangents)
    {
      m_tangents = _tangents;
    }
    void SetTangents(float _x, float _y, float _z)
    {
      m_tangents = glm::vec3(_x, _y, _z);
    }
  };
}

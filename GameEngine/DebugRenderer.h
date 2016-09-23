#pragma once

#include "GLSLProgram.h"
#include "Vertex.h"
#include <glm/glm.hpp>
#include <vector>

namespace GameEngine
{
  class DebugRenderer
  {
  public:
    DebugRenderer();
    ~DebugRenderer();

    //initialize the renderer
    void Init();
    //ends the rendering
    void End();
    //draw a single line from point a to point b
    void DrawLine(const glm::vec2& _a, const glm::vec2& _b, const ColorRGBA8& _color);
    //draw a box
    void DrawBox(const glm::vec4& _destRect, const ColorRGBA8& _color, float _angle);
    //draw a circle
    void DrawCircle(const glm::vec2& _center, const ColorRGBA8& _color, float _radius);
    //renders it
    void Render(const glm::mat4& _projectionMatrix, float _lineWidth);
    //disposes the debug renderer
    void Dispose();
    //the debug vertex struct ( a vertex has a position and a color)
    struct DebugVertex
    {
      glm::vec2 m_position;
      ColorRGBA8 m_color;
    };

  private:
    GLSLProgram m_program;
    std::vector<DebugVertex> m_verts;
    std::vector<GLuint> m_indices;
    GLuint m_vbo = 0, m_vao = 0, m_ibo = 0;
    int m_numElements{ 0 };
  };
}



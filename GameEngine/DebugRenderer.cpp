#include "DebugRenderer.h"

const float PI = 3.14159265359f;

namespace GameEngine
{
  DebugRenderer::DebugRenderer()
  {
    // Empty
  }

  DebugRenderer::~DebugRenderer()
  {
    Dispose();
  }

  void DebugRenderer::Init()
  {
    std::vector<Shader> debugShaders =
    {
      { GL_VERTEX_SHADER, "Shaders/debugRenderer.vert", "Debug Vertex Shader" },
      { GL_FRAGMENT_SHADER, "Shaders/debugRenderer.frag", "Debug Fragment Shader" },
    };
    // Shader init
    m_program.CompileShaders(debugShaders);
    m_program.LinkShaders();

    // Set up buffers
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ibo);
    //bind the vertex array object
    glBindVertexArray(m_vao);
    //bind the vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    //bind the index buffer object
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    //enable the vertex attrib array on index 0
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (void*)offsetof(DebugVertex, m_position));
    //enable the vertex attrib array on index 1
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(DebugVertex), (void*)offsetof(DebugVertex, m_color));
    //unbind the vao
    glBindVertexArray(0);
  }

  void DebugRenderer::End()
  {
    //bind the vbo
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    // Orphan the buffer
    glBufferData(GL_ARRAY_BUFFER, m_verts.size() * sizeof(DebugVertex), nullptr, GL_DYNAMIC_DRAW);
    // Upload the data
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_verts.size() * sizeof(DebugVertex), m_verts.data());
    //unbind the vbo
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //bind the ibo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    // Orphan the buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
    // Upload the data
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_indices.size() * sizeof(GLuint), m_indices.data());
    //unbind the ibo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    //set up the numelements to be the same as indices.size
    m_numElements = m_indices.size();
    //clear the inices and vertices vectors
    m_indices.clear();
    m_verts.clear();
  }

  glm::vec2 RotatePoint(const glm::vec2& _pos, float _angle)
  {
    //rotate a point with the math formula
    glm::vec2 newv;
    newv.x = _pos.x * cos(_angle) - _pos.y * sin(_angle);
    newv.y = _pos.x * sin(_angle) + _pos.y * cos(_angle);
    return newv;
  }

  void DebugRenderer::DrawLine(const glm::vec2& _a, const glm::vec2& _b, const ColorRGBA8& _color)
  {
    int i = m_verts.size();
    //resize the verts to +2 in size
    m_verts.resize(m_verts.size() + 2);

    m_verts[i].m_position = _a;
    m_verts[i].m_color = _color;
    m_verts[i + 1].m_position = _b;
    m_verts[i + 1].m_color = _color;
    //push back the indices
    m_indices.push_back(i);
    m_indices.push_back(i + 1);
  }

  void DebugRenderer::DrawBox(const glm::vec4& _destRect, const ColorRGBA8& _color, float _angle)
  {
    int i = m_verts.size();
    //resize the verts to size + 4
    m_verts.resize(m_verts.size() + 4);
    //the half of the box dimensions
    glm::vec2 halfDims(_destRect.z / 2.0f, _destRect.w / 2.0f);

    // Get points centered at origin
    glm::vec2 tl(-halfDims.x, halfDims.y);
    glm::vec2 bl(-halfDims.x, -halfDims.y);
    glm::vec2 br(halfDims.x, -halfDims.y);
    glm::vec2 tr(halfDims.x, halfDims.y);

    glm::vec2 positionOffset(_destRect.x, _destRect.y);

    // Rotate the points
    m_verts[i].m_position = RotatePoint(tl, _angle) + halfDims + positionOffset;
    m_verts[i + 1].m_position = RotatePoint(bl, _angle) + halfDims + positionOffset;
    m_verts[i + 2].m_position = RotatePoint(br, _angle) + halfDims + positionOffset;
    m_verts[i + 3].m_position = RotatePoint(tr, _angle) + halfDims + positionOffset;
    //set up the colors
    for (int j = i; j < i + 4; j++)
    {
      m_verts[j].m_color = _color;
    }
    //reserve the indices size needed
    m_indices.reserve(m_indices.size() + 8);
    //push back the 6 indices (2 triangles)
    m_indices.push_back(i);
    m_indices.push_back(i + 1);

    m_indices.push_back(i + 1);
    m_indices.push_back(i + 2);

    m_indices.push_back(i + 2);
    m_indices.push_back(i + 3);

    m_indices.push_back(i + 3);
    m_indices.push_back(i);
  }

  void DebugRenderer::DrawCircle(const glm::vec2& _center, const ColorRGBA8& _color, float _radius)
  {
    static const int NUM_VERTS = 100;
    // Set up vertices
    int start = m_verts.size();
    m_verts.resize(m_verts.size() + NUM_VERTS);
    for (int i = 0; i < NUM_VERTS; i++)
    {
      //formula to make a circle by setting it's positions
      float angle = ((float)i / NUM_VERTS) * PI * 2.0f;
      m_verts[start + i].m_position.x = cos(angle) * _radius + _center.x;
      m_verts[start + i].m_position.y = sin(angle) * _radius + _center.y;
      //also set it's color
      m_verts[start + i].m_color = _color;
    }

    // Set up indices for indexed drawing
    m_indices.reserve(m_indices.size() + NUM_VERTS * 2);
    for (int i = 0; i < NUM_VERTS - 1; i++)
    {
      m_indices.push_back(start + i);
      m_indices.push_back(start + i + 1);
    }
    m_indices.push_back(start + NUM_VERTS - 1);
    m_indices.push_back(start);
  }

  void DebugRenderer::Render(const glm::mat4& _projectionMatrix, float _lineWidth)
  {
    //begin using the shader program
    m_program.Use();
    //get the uniform location
    GLint pUniform = m_program.GetUniformLocation("P");
    //set up the projection matrix
    glUniformMatrix4fv(pUniform, 1, GL_FALSE, &_projectionMatrix[0][0]);
    //set up the line width
    glLineWidth(_lineWidth);
    //bind the vertex array object
    glBindVertexArray(m_vao);
    //draw the elements
    glDrawElements(GL_LINES, m_numElements, GL_UNSIGNED_INT, 0);
    //unbind the vao
    glBindVertexArray(0);
    //stop using the shader program
    m_program.UnUse();
  }

  void DebugRenderer::Dispose()
  {
    //delete all the buffers
    if (m_vao)
    {
      glDeleteVertexArrays(1, &m_vao);
    }
    if (m_vbo)
    {
      glDeleteBuffers(1, &m_vbo);
    }
    if (m_ibo)
    {
      glDeleteBuffers(1, &m_ibo);
    }
    //dispose of the shader program
    m_program.Dispose();
  }
}

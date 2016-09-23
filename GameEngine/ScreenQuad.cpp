#include "ScreenQuad.h"
namespace GameEngine
{
  ScreenQuad::ScreenQuad()
  {
  }


  ScreenQuad::~ScreenQuad()
  {
    Dispose();
  }

  void ScreenQuad::Init()
  {
    if (m_VAO == 0)
    {
      glGenVertexArrays(1, &m_VAO);
    }
    if (m_VBO == 0)
    {
      glGenBuffers(1, &m_VBO);
    }
    if (m_EBO == 0)
    {
      glGenBuffers(1, &m_EBO);
    }

    GLfloat vertices[] = 
    {
      // Positions   // TexCoords
      -1.0f, 1.0f, 0.0f, 1.0f,
      -1.0f, -1.0f, 0.0f, 0.0f,
      1.0f, -1.0f, 1.0f, 0.0f,

      -1.0f, 1.0f, 0.0f, 1.0f,
      1.0f, -1.0f, 1.0f, 0.0f,
      1.0f, 1.0f, 1.0f, 1.0f
    };

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), nullptr, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices[0]);

    //Upload the position and uv as 1 attribute (vec4) to send all the data in 1 go to the postprocess shader
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));

    glBindVertexArray(0);
  }
  void ScreenQuad::Dispose()
  {
    //delete the vao's and vbo's and reset them to 0
    if (m_VAO != 0)
    {
      glDeleteVertexArrays(1, &m_VAO);
      m_VAO = 0;
    }

    if (m_VBO != 0)
    {
      glDeleteBuffers(1, &m_VBO);
      m_VBO = 0;
    }

    if (m_EBO != 0)
    {
      glDeleteBuffers(1, &m_EBO);
      m_EBO = 0;
    }
  }
  void ScreenQuad::Render(GLuint _textureID)
  {
    glBindVertexArray(m_VAO);

    glBindTexture(GL_TEXTURE_2D, _textureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
  }
}

#include "Skybox.h"

#include <glm\gtc\type_ptr.hpp>

namespace GameEngine
{
  Skybox::Skybox() : m_VAO(0), m_VBO(0)
  {
  }


  Skybox::~Skybox()
  {
    Dispose();
  }

  void Skybox::Init(const GLCubemap& _cubemap)
  {
    m_cubemap = _cubemap;

    GLfloat skyboxVertices[] = 
    {
      // Positions          
      -1.0f, 1.0f, -1.0f,
      -1.0f, -1.0f, -1.0f,
      1.0f, -1.0f, -1.0f,
      1.0f, -1.0f, -1.0f,
      1.0f, 1.0f, -1.0f,
      -1.0f, 1.0f, -1.0f,

      -1.0f, -1.0f, 1.0f,
      -1.0f, -1.0f, -1.0f,
      -1.0f, 1.0f, -1.0f,
      -1.0f, 1.0f, -1.0f,
      -1.0f, 1.0f, 1.0f,
      -1.0f, -1.0f, 1.0f,

      1.0f, -1.0f, -1.0f,
      1.0f, -1.0f, 1.0f,
      1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, -1.0f,
      1.0f, -1.0f, -1.0f,

      -1.0f, -1.0f, 1.0f,
      -1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, 1.0f,
      1.0f, -1.0f, 1.0f,
      -1.0f, -1.0f, 1.0f,

      -1.0f, 1.0f, -1.0f,
      1.0f, 1.0f, -1.0f,
      1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, 1.0f,
      -1.0f, 1.0f, 1.0f,
      -1.0f, 1.0f, -1.0f,

      -1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f, 1.0f,
      1.0f, -1.0f, -1.0f,
      1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f, 1.0f,
      1.0f, -1.0f, 1.0f
    };
    //Setup the skybox ----------
    if (m_VAO == 0)
    {
      glGenVertexArrays(1, &m_VAO);
    }
    if (m_VBO == 0)
    {
      glGenBuffers(1, &m_VBO);
    }

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(skyboxVertices), &skyboxVertices[0]);

    //Tell opengl how to upload the vertex info to the shader
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

    glBindVertexArray(0);
  }

  void Skybox::Dispose()
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
  }

  void Skybox::Render()
  {
    glDepthFunc(GL_LEQUAL);

    // skybox cube
    glBindVertexArray(m_VAO);

    m_cubemap.Bind(GL_TEXTURE0);
    //glUniform1i(m_skyboxShader.GetUniformLocation("skybox"), 0);

    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS);
  }

}
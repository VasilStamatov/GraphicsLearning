#pragma once

#include "Vertex.h"
#include "GLTexture.h"
#include "GLSLProgram.h"
#include <vector>
#include <glm\mat4x4.hpp>

namespace GameEngine
{
  class Mesh
  {
  public:
    Mesh(const std::vector<Vertex>& _vertices, const std::vector<GLuint>& _indices, const std::vector<GLTexture>& _textures);
    ~Mesh();
    /* Mesh data */
    std::vector<Vertex> m_vertices;
    std::vector<GLuint> m_indices;
    std::vector<GLTexture> m_textures;
    /* Mesh functions */
    void Draw(GLSLProgram& _shaderProgram, int _amount = 1, bool _instanced = false);

    /* Getters */
    GLuint GetVAO() { return m_VAO; }
    GLuint GetVBO() { return m_VBO; }
    GLuint GetEBO() { return m_EBO; }

  private:
    /* Render Data */
    GLuint m_VAO = 0, m_VBO = 0, m_EBO = 0;

    /* Setup Function */
    void SetupMesh();
  };
}


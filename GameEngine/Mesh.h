#pragma once

#include "Vertex.h"
#include "GLTexture.h"
#include "GLSLProgram.h"
#include <vector>
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
    void Draw(GLSLProgram& _shaderProgram);
  private:
    /* Render Data */
    GLuint m_VAO = 0, m_VBO = 0, m_EBO = 0;

    /* Setup Function */
    void SetupMesh();
  };
}


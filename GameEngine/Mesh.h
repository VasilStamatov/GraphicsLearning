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
    Mesh()
    {

    }
    Mesh(const std::vector<Vertex>& _vertices, const std::vector<GLuint>& _indices, const std::vector<GLTexture>& _textures);

    Mesh(const std::vector<Vertex>& _vertices, const std::vector<GLuint>& _indices, const std::vector<GLTexture>& _textures,
      bool _hasAnim, const glm::mat4& _baseModelMatrix);
    ~Mesh();
    /* Mesh functions */
    void Draw(GLSLProgram& _shaderProgram, int _amount = 1);
    void Dispose();

    /* Getters */
    GLuint GetVAO() const noexcept { return m_VAO; }
    GLuint GetVBO() const noexcept { return m_VBO; }
    GLuint GetEBO() const noexcept { return m_EBO; }
    GLuint GetMBO() const noexcept { return m_MBO; }

    std::vector<GLTexture> GetTextures() const noexcept { return m_textures; }
    std::vector<Vertex> GetVertices()    const noexcept { return m_vertices; }

  private:
    /* Render Data */
    GLuint m_VAO{ 0 }, m_VBO{ 0 }, m_EBO{ 0 }, m_MBO{ 0 };

    /* Mesh data */
    std::vector<Vertex> m_vertices;
    std::vector<GLuint> m_indices;
    std::vector<GLTexture> m_textures;
    glm::mat4 m_baseModelMatrix{ 1.0f };
    bool m_hasAnimations{ false };
    /* Setup Function */
    void SetupMesh();
  };
}

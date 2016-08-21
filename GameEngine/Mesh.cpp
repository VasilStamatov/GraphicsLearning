#include "Mesh.h"
#include <string>
#include <glm\gtc\matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace GameEngine
{
  Mesh::Mesh(const std::vector<Vertex>& _vertices, const std::vector<GLuint>& _indices, const std::vector<GLTexture>& _textures,
    bool _hasAnim, const glm::mat4& _baseModelMatrix)
  {
    m_vertices = _vertices;
    m_indices = _indices;
    m_textures = _textures;
    m_hasAnimations = _hasAnim;
    m_baseModelMatrix = _baseModelMatrix;
    SetupMesh();
  }
  Mesh::Mesh(const std::vector<Vertex>& _vertices, const std::vector<GLuint>& _indices, const std::vector<GLTexture>& _textures)
  {
    m_vertices = _vertices;
    m_indices = _indices;
    m_textures = _textures;
    SetupMesh();
  }
  Mesh::~Mesh()
  {
  }

  void Mesh::Draw(GLSLProgram& _shaderProgram, int _amount)
  {
    GLuint diffuseNr = 1;
    GLuint specularNr = 1;
    GLuint reflectionNr = 1;
    GLuint normalNr = 1;

    for (GLuint i = 0; i < m_textures.size(); i++)
    {
      GLuint number;
      std::string name = m_textures[i].type;

      if (name == "texture_diffuse")
      {
        number = diffuseNr++;
      }
      else if (name == "texture_specular")
      {
        number = specularNr++;
      }
      else if (name == "texture_reflection")
      {
        number = reflectionNr++;
      }
      else if (name == "texture_normal")
      {
        number = normalNr++;
      }
      // Now set the sampler to the correct texture unit
      glUniform1i(_shaderProgram.GetUniformLocation("material." + name + std::to_string(number)), i);

      // And finally bind the texture
      m_textures.at(i).Bind(GL_TEXTURE0 + i);
    }
    glActiveTexture(GL_TEXTURE0); // Always good practice to set everything back to defaults once configured.

    // Also set each mesh's shininess property to a default value (if you want you could extend this to another mesh property and possibly change this value)
    glUniform1f(_shaderProgram.GetUniformLocation("material.shininess"), 16.0f);
    glUniformMatrix4fv(_shaderProgram.GetUniformLocation("baseModelMatrix"), 1, GL_FALSE, glm::value_ptr(m_baseModelMatrix));
    //Draw mesh
    glBindVertexArray(m_VAO);

    glDrawElementsInstanced(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0, _amount);

    glBindVertexArray(0);
  }

  void Mesh::Dispose()
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

    if (m_MBO != 0)
    {
      glDeleteBuffers(1, &m_MBO);
      m_MBO = 0;
    }

  }

  void Mesh::SetupMesh()
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
    if (m_MBO == 0)
    {
      glGenBuffers(1, &m_MBO);
    }

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), nullptr, GL_STATIC_DRAW);

    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_indices.size() * sizeof(GLuint), m_indices.data());


    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), nullptr, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertices.size() * sizeof(Vertex), m_vertices.data());
    
    GLuint attributeLocation = 0;

    //Vertex position
    glEnableVertexAttribArray(attributeLocation);
    glVertexAttribPointer(attributeLocation++, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, m_position));

    //Vertex normals
    glEnableVertexAttribArray(attributeLocation);
    glVertexAttribPointer(attributeLocation++, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, m_normal));

    //Vertex texture coords
    glEnableVertexAttribArray(attributeLocation);
    glVertexAttribPointer(attributeLocation++, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, m_uv));

    if (m_hasAnimations)
    {
      glEnableVertexAttribArray(attributeLocation);
      glVertexAttribIPointer(attributeLocation++, 4, GL_INT, sizeof(Vertex), (GLvoid*)offsetof(Vertex, m_boneIDs));

      glEnableVertexAttribArray(attributeLocation);
      glVertexAttribPointer(attributeLocation++, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, m_weights));
    }
    glBindBuffer(GL_ARRAY_BUFFER, m_MBO);
    // Set attribute pointers for matrix (4 times vec4)
    for (GLint i = 0; i < 4; i++)
    {
      glEnableVertexAttribArray(attributeLocation + i);
      glVertexAttribPointer(attributeLocation + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(i * sizeof(glm::vec4)));
      glVertexAttribDivisor(attributeLocation + i, 1);
    }

    glBindVertexArray(0);
  }
}
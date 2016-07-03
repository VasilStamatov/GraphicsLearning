#include "Mesh.h"
#include <string>
#include <glm\gtc\matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace GameEngine
{
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

  void Mesh::Draw(GLSLProgram& _shaderProgram, int _amount, bool _instanced)
  {
    GLuint diffuseNr = 1;
    GLuint specularNr = 1;
    GLuint reflectionNr = 1;

    for (GLuint i = 0; i < m_textures.size(); i++)
    {
      glActiveTexture(GL_TEXTURE0 + i); // Activate proper texture unit before binding

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
      // Now set the sampler to the correct texture unit
      glUniform1i(_shaderProgram.GetUniformLocation("material." + name + std::to_string(number)), i);

      // And finally bind the texture
      glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0); // Always good practice to set everything back to defaults once configured.

    // Also set each mesh's shininess property to a default value (if you want you could extend this to another mesh property and possibly change this value)
    glUniform1f(_shaderProgram.GetUniformLocation("material.shininess"), 16.0f);

    //Draw mesh
    glBindVertexArray(m_VAO);

    if (_instanced)
    {
      glDrawElementsInstanced(GL_TRIANGLES, m_vertices.size(), GL_UNSIGNED_INT, 0, _amount);
    }
    else
    {
      glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
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

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), nullptr, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertices.size() * sizeof(Vertex), m_vertices.data());


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), nullptr, GL_STATIC_DRAW);

    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_indices.size() * sizeof(GLuint), m_indices.data());

    //Vertex position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, m_position));

    //Vertex texture coords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, m_normal));

    //Vertex normals
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, m_uv));

    ////Vertex color
    //glEnableVertexAttribArray(3);
    //glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));

    glBindVertexArray(0);

  }
}

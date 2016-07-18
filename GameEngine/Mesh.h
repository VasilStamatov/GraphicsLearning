#pragma once

#include "Vertex.h"
#include "GLTexture.h"
#include "GLSLProgram.h"
#include <vector>
#include <glm\mat4x4.hpp>
#include <assimp\mesh.h>
#include <map>

namespace GameEngine
{
  class Mesh
  {
  public:
    Mesh(const std::vector<Vertex>& _vertices, const std::vector<GLuint>& _indices, const std::vector<GLTexture>& _textures);
    ~Mesh();
    /* Mesh functions */
    void Draw(GLSLProgram& _shaderProgram, int _amount = 1);
    void Dispose();

    /* Getters */
    GLuint GetVAO() const { return m_VAO; }
    GLuint GetVBO() const { return m_VBO; }
    GLuint GetEBO() const { return m_EBO; }
    GLuint GetMBO() const { return m_MBO; }

    std::vector<GLTexture> GetTextures() const { return m_textures; }

  private:
    struct BoneInfo
    {
      glm::mat4 m_boneOffset;
      glm::mat4 m_finalTransform;

      BoneInfo()
      {
        m_boneOffset = glm::mat4(1.0f);
        m_finalTransform = glm::mat4(1.0f);
      }
    };

    struct VertexBoneData
    {
      std::vector<GLuint> m_IDs;
      std::vector<GLfloat> m_weights;

      void Reset()
      {
        m_IDs.clear();
        m_weights.clear();
      }

      void AddBoneData(GLuint _boneID, GLfloat _weight);

      VertexBoneData()
      {
        Reset();
      }

    };

    void LoadBones(GLuint _meshIndex, const aiMesh* _aiMesh, std::vector<VertexBoneData>& _bones);

    std::map<std::string, GLuint> m_boneMapping; // maps a bone name to its index
    GLuint m_numBones;
    std::vector<BoneInfo> m_boneInfo;

    /* Render Data */
    GLuint m_VAO = 0, m_VBO = 0, m_EBO = 0, m_MBO;

    /* Mesh data */
    std::vector<Vertex> m_vertices;
    std::vector<GLuint> m_indices;
    std::vector<VertexBoneData> Bones;
    std::vector<GLTexture> m_textures;

    /* Setup Function */
    void SetupMesh();
  };
}


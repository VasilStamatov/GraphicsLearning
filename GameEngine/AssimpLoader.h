#pragma once

#include "Model.h"

namespace GameEngine
{
  class AssimpLoader
  {
  public:
    bool LoadStaticModel(const std::string& _path, StaticModel* _model);
    bool LoadSkinnedModel(const std::string& _path, SkinnedModel* _model);

  private:
    void ProcessNode(aiNode* _node, const aiScene* _scene, StaticModel* _model);
    void ProcessNode(aiNode* _node, const aiScene* _scene, SkinnedModel* _model);

    Mesh ProcessMesh(aiMesh* _mesh, const aiScene* _scene, aiNode* _node);
    Mesh ProcessMesh(aiMesh* _mesh, const aiScene* _scene, aiNode* _node, SkinnedModel* _model);

    void ProcessAnimations(const aiScene* _scene, SkinnedModel* _model);

    std::vector<GLTexture> LoadMaterialTextures(aiMaterial* _mat, const aiTextureType& _type, const std::string& _typeName);

    void ProcessVertices(aiMesh* _mesh, std::vector<Vertex>& _vertices);
    void ProcessFaces(aiMesh* _mesh, std::vector<GLuint>& _indices);
    void ProcessTextures(const aiScene* _scene, aiMesh* _mesh, std::vector<GLTexture>& _textures);

    glm::mat4 AssimpToGlmMat4(const aiMatrix4x4* _aiMatrix);

    std::string m_directory{ "" }; ///< the directory of the loading model
  };
}
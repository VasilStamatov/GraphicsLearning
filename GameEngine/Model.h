#pragma once

#include <assimp\scene.h>
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>

#include "GLSLProgram.h"
#include "Mesh.h"

namespace GameEngine
{
  class Model
  {
  public:
    Model();
    ~Model();

    void LoadModel(const std::string& _path);
    void Draw(GLSLProgram& _shader);
  private:
    /* Model parameters */
    std::vector<Mesh> m_meshes;
    std::string m_directory;

    /* Model functions */
    void ProcessNode(aiNode* _node, const aiScene* _scene);
    Mesh ProcessMesh(aiMesh* _mesh, const aiScene* _scene);

    std::vector<GLTexture> LoadMaterialTextures(aiMaterial* _mat, aiTextureType _type, const std::string& _typeName);
  };
}



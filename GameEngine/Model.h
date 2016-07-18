#pragma once

#include <assimp\scene.h>
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <glm\mat4x4.hpp>

#include "GLSLProgram.h"
#include "Mesh.h"

namespace GameEngine
{
  class Model
  {
  public:
    Model();
    Model(const std::string& _path)
    {
      LoadModel(_path);
    }

    ~Model();

    void Draw(GLSLProgram& _shader, std::vector<glm::mat4> _modelMatrices);
    void Dispose();


    std::vector<Mesh> GetMeshes() { return m_meshes; }
  private:
    /* Model parameters */
    std::vector<Mesh> m_meshes;
    std::string m_directory;

    bool m_instanceSetup = false;
    /* Model functions */
    void ProcessNode(aiNode* _node, const aiScene* _scene);
    Mesh ProcessMesh(aiMesh* _mesh, const aiScene* _scene);

    void LoadModel(const std::string& _path);

    std::vector<GLTexture> LoadMaterialTextures(aiMaterial* _mat, const aiTextureType& _type, const std::string& _typeName);



    

    void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    GLuint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
    GLuint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
    GLuint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
    const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string& NodeName);
    void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform);
    bool InitMaterials(const aiScene* pScene, const std::string& Filename);

    
    glm::mat4 m_globalInverseTransform;

    const aiScene* m_scene;
    Assimp::Importer m_importer;
  };
}



#pragma once

#include <assimp\scene.h>
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <glm\mat4x4.hpp>
#include <map>

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
    void UpdateAnimation(float _deltaTIme);
    void Dispose();

    std::vector<Mesh> GetMeshes() { return m_meshes; }

  private:
    struct Animation
    {
      struct Channel
      {
        std::string m_name = "";
        glm::mat4 m_offset = glm::mat4(1.0f);
        std::vector <aiVectorKey> m_positionKeys;
        std::vector <aiQuatKey> m_rotationKeys;
        std::vector <aiVectorKey> m_scalingKeys;
      };
      struct BoneNode
      {
        std::string m_name = "";
        BoneNode* m_parent = nullptr;
        std::vector <BoneNode> m_children;
        glm::mat4 m_nodeTransform = glm::mat4(1.0f);
        glm::mat4 m_boneTransform = glm::mat4(1.0f);
      };

      std::string m_name = "";
      float m_duration = 0.0f;
      float m_ticksPerSecond = 0.0f;
      // all of the bone transformations, this is modified every frame
      // assimp calls it a channel, its anims for a node aka bone
      std::map <std::string, glm::mat4> m_findBoneOffsetByName;
      std::vector <glm::mat4> m_boneTrans;
      std::vector <Channel> m_channels;
      BoneNode root;

      void BuildBoneTree(const aiScene* _scene, aiNode* _node, BoneNode* _bNode, Model* _m);
    };

    /* Model parameters */
    std::map<std::string, GLuint> m_findBoneIDbyName;
    std::vector<Mesh> m_meshes;
    std::vector<Animation> m_animations;
    std::string m_directory = "";
    glm::mat4 m_globalInverseTransform = glm::mat4(1.0f);

    GLuint m_currentAnimation = 0;
    bool m_hasAnimation = false;
    /* Model functions */
    void ProcessNode(aiNode* _node, const aiScene* _scene);
    Mesh ProcessMesh(aiMesh* _mesh, const aiScene* _scene, aiNode* _node);
    void ProcessAnimations(const aiScene* scene);

    void LoadModel(const std::string& _path);

    std::vector<GLTexture> LoadMaterialTextures(aiMaterial* _mat, const aiTextureType& _type, const std::string& _typeName);

    void UpdateBoneTree(float _deltaTIme, Animation::BoneNode* _node, const glm::mat4& _parentTransform);
  };
}
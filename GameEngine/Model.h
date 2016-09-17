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

    /** \brief Draws the model
    * \param _shader - the shader used to upload the textures and model matrices
    * \param _modelMatrices - the model matrices which tell the function how many
    instances of this model will be drawn, and their transform in world space
    */
    void Draw(GLSLProgram& _shader, std::vector<glm::mat4> _modelMatrices);

    /** \brief Updates the animation (if the model has one)
    * \param _deltaTIme - used to interpolate the animation keyframes in a normal speed
    */
    void PlayAnimation(float _ticks);

    /** \brief Sets the animation the model shall play
    * \param _animName - the name of the animation (ex. "run", "walk", "jump" etc.)
    */
    void SetAnimation(const std::string& _animName);

    /** \brief Disposes of the model */
    void Dispose();

    /** \brief Get the vector of meshes this model consists of*/
    std::vector<Mesh> GetMeshes() { return m_meshes; }

  private:
    /** \brief The animation struct used to store the animation data of the model*/
    struct Animation
    {
      /** \brief The channel struct used to store bone data.
      Each channel is actually the bone with all its transformations, name and offset*/
      struct Channel
      {
        ///the name of the bone (the bone node corresponding to a bone has the same name)
        std::string m_name = "";
        ///The offset matrix of this bone
        glm::mat4 m_offset = glm::mat4(1.0f);
        ///The position, rotation and scaling values for this bone for every frame of the current animation
        std::vector <aiVectorKey> m_positionKeys;
        std::vector <aiQuatKey> m_rotationKeys;
        std::vector <aiVectorKey> m_scalingKeys;
      };
      /** \brief The bonenode struct used to store bone info in a tree-based way.
      It stores the name of the node(aiNode), the parent of the bone and its children.
      nodeTransform is the matrix that contains the translation, rotation and scaling of the bone in the current frame
      boneTransform is the offset matrix used in the calculation for the final matrix
      (which transforms from model space to bone space) */
      struct BoneNode
      {
        ///the name of this bone node(same as the name of the bone this node corresponds to)
        std::string m_name = "";
        ///the parent of this bone node
        BoneNode* m_parent = nullptr;
        ///The children of this bone node
        std::vector <BoneNode> m_children;
        ///The container of the translation * rotation * scaling matrix
        glm::mat4 m_nodeTransform = glm::mat4(1.0f);
        ///the container of the bone offset for this node
        glm::mat4 m_boneTransform = glm::mat4(1.0f);
      };
      ///the name of this animation
      std::string m_name = "";
      ///the durotation of this animation
      float m_duration = 0.0f;
      ///It's ticks per second
      float m_ticksPerSecond = 0.0f;
      ///a map to find a mat4 bone offset by its name
      std::map <std::string, glm::mat4> m_findBoneOffsetByName;
      ///all of the bone transformations, this is modified every frame
      ///assimp calls it a channel, its anims for a node aka bone
      std::vector <glm::mat4> m_boneTrans;
      ///all of the channels (bones and their transforms) for this animation
      std::vector <Channel> m_channels;
      ///the bonenode root (start of the tree)
      BoneNode root;

      ///Build the bone tree of this one animation
      void BuildBoneTree(const aiScene* _scene, aiNode* _node, BoneNode* _bNode, Model* _m);
    };

    /* Model parameters */
    std::vector<Mesh> m_meshes; ///< all the meshes this model consists of
    std::string m_directory = ""; ///< the directory of this model
    std::map<std::string, GLuint> m_findBoneIDbyName; ///< a map to find a bone's ID by it's name

    std::vector<Animation> m_animations; ///< all the animations this model has
    GLuint m_currentAnimation = 0; ///< the current animation this model will play
    bool m_hasAnimation = false; ///< a flag if this model has any animations or not
    glm::mat4 m_globalInverseTransform = glm::mat4(1.0f); ///the global inverse transform matrix
    /* Model functions */
    void LoadModel(const std::string& _path);

    void ProcessNode(aiNode* _node, const aiScene* _scene);
    Mesh ProcessMesh(aiMesh* _mesh, const aiScene* _scene, aiNode* _node);
    void ProcessAnimations(const aiScene* scene);

    std::vector<GLTexture> LoadMaterialTextures(aiMaterial* _mat, const aiTextureType& _type, const std::string& _typeName);

    void UpdateBoneTree(float _deltaTIme, Animation::BoneNode* _node, const glm::mat4& _parentTransform);

    /** \brief Find which keyframe the animation is at now for the current channel (bone) */
    GLuint FindPositionKey(float _animTime, Animation::Channel& _channel);
    GLuint FindRotationKey(float _animTime, Animation::Channel& _channel);
    GLuint FindScalingKey(float _animTime, Animation::Channel& _channel);

    /** \brief Calculate the interpolated position*/
    glm::vec3 CalcInterpolatedPosition(float _animTime, Animation::Channel& _channel);
    glm::mat4 CalcInterpolatedRotation(float _animTime, Animation::Channel& _channel);
    glm::vec3 CalcInterpolatedScaling(float _animTime, Animation::Channel& _channel);
  };
}
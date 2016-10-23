#pragma once

#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <glm\mat4x4.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\quaternion.hpp>
#include <map>

#include "Mesh.h"

namespace GameEngine
{
  /* Model with skinned bone animation*/
  class SkinnedModel
  {
    friend class AssimpLoader;
  public:
    SkinnedModel() {}
    ~SkinnedModel() { Dispose(); }

    void Dispose();

    void Update(float _elapsed);

    void Draw(GLSLProgram& _shader);

    void SetAnimation(const std::string& _animName);
    void SetAnimationPlay(bool _play) { m_play = _play; }

    void OffsetPosition(const glm::vec3 _position);
    void OffsetRotation(const glm::vec3 _rotation);
    void OffsetScale(const glm::vec3 _scale);

    void SetPosition(const glm::vec3 _position);
    void SetRotation(const glm::vec3 _rotation);
    void SetScale(const glm::vec3 _scale);

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
        std::string m_name{ "" };
        ///The offset matrix of this bone
        glm::mat4 m_offset{ 1.0f };
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
        std::string m_name{ "" };
        ///the parent of this bone node
        BoneNode* m_parent{ nullptr };
        ///The children of this bone node
        std::vector <BoneNode> m_children;
        ///The container of the translation * rotation * scaling matrix
        glm::mat4 m_nodeTransform{ 1.0f };
        ///the container of the bone offset for this node
        glm::mat4 m_boneTransform{ 1.0f };
      };
      ///the name of this animation
      std::string m_name{ "" };
      ///the durotation of this animation
      float m_duration{ 0.0f };
      ///It's ticks per second
      float m_ticksPerSecond{ 0.0f };
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
      void BuildBoneTree(const aiScene* _scene, aiNode* _node, BoneNode* _bNode, SkinnedModel* _model);
    };

    /* Model parameters */
    GLuint m_currentAnimation{ 0 }; ///< the current animation this model will play
    std::vector<Mesh> m_meshes; ///< all the meshes this model consists of
    std::vector<Animation> m_animations; ///< all the animations this model has
    glm::mat4 m_globalInverseTransform{ 1.0f }; ///the global inverse transform matrix
    std::map<std::string, GLuint> m_findBoneIDbyName; ///< a map to find a bone's ID by it's name
    bool m_play{ true };
    glm::vec3 m_position{ 0.0f, 0.0f, 0.0f };
    glm::quat m_rotation{ 0.0f, 0.0f, 0.0f, 1.0f };
    glm::vec3 m_scale{ 0.1f, 0.1f, 0.1f };

    /* Model functions */
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

  /* A Static Model*/
  class StaticModel
  {
    friend class AssimpLoader;
  public:
    StaticModel() {}
    ~StaticModel() { Dispose(); }

    void Dispose();

    void Draw(GLSLProgram& _shader);
    void DrawInstanced(GLSLProgram& _shader, std::vector<glm::mat4>& _modelMatrices);

    std::vector<Mesh> GetMeshes() { return m_meshes; }

    void OffsetPosition(const glm::vec3 _position);
    void OffsetRotation(const glm::vec3 _rotation);
    void OffsetScale(const glm::vec3 _scale);

    void SetPosition(const glm::vec3 _position);
    void SetRotation(const glm::vec3 _rotation);
    void SetScale(const glm::vec3 _scale);
  private:
    std::vector<Mesh> m_meshes; ///< all the meshes this model consists of
    glm::vec3 m_position{ 0.0f, 0.0f, 0.0f };
    glm::quat m_rotation{ 0.0f, 0.0f, 0.0f, 1.0f };
    glm::vec3 m_scale{ 0.1f, 0.1f, 0.1f };
  };
}
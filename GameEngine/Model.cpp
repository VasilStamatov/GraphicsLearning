#include "Model.h"

#include <iostream>

namespace GameEngine
{
  glm::mat4 AssimpToGlmMat4(const aiMatrix4x4 * _aiMatrix)
  {
    glm::mat4 mat;
    mat[0][0] = _aiMatrix->a1; mat[1][0] = _aiMatrix->a2; mat[2][0] = _aiMatrix->a3; mat[3][0] = _aiMatrix->a4;
    mat[0][1] = _aiMatrix->b1; mat[1][1] = _aiMatrix->b2; mat[2][1] = _aiMatrix->b3; mat[3][1] = _aiMatrix->b4;
    mat[0][2] = _aiMatrix->c1; mat[1][2] = _aiMatrix->c2; mat[2][2] = _aiMatrix->c3; mat[3][2] = _aiMatrix->c4;
    mat[0][3] = _aiMatrix->d1; mat[1][3] = _aiMatrix->d2; mat[2][3] = _aiMatrix->d3; mat[3][3] = _aiMatrix->d4;
    return mat;
  }
  //differentiating the nodes and bone nodes
  void SkinnedModel::Animation::BuildBoneTree(const aiScene* _scene, aiNode* _node, BoneNode* _bNode, SkinnedModel* _model)
  {
    if (_scene->HasAnimations())
    {
      //Check if the passed node is a bone node, by cheching to see if the name matches with a bone ID 
      if (_model->m_findBoneIDbyName.find(_node->mName.data) != _model->m_findBoneIDbyName.end())
      {
        // This node is a bone node
        BoneNode tempNode;
        tempNode.m_name = _node->mName.data;
        tempNode.m_parent = _bNode;
        tempNode.m_nodeTransform = AssimpToGlmMat4(&_node->mTransformation);
        // bones and their nodes always share the same name
        tempNode.m_boneTransform = m_findBoneOffsetByName[tempNode.m_name];
        _bNode->m_children.push_back(tempNode);
      }
    }

    for (GLuint x = 0; x < _node->mNumChildren; x++)
    {
      // if the node we just found was a bone node then pass it in (current bone node child vector size - 1)
      if (_model->m_findBoneIDbyName.find(_node->mName.data) != _model->m_findBoneIDbyName.end())
      {
        BuildBoneTree(_scene, _node->mChildren[x], &_bNode->m_children[_bNode->m_children.size() - 1], _model);
      }
      else
      {
        BuildBoneTree(_scene, _node->mChildren[x], _bNode, _model);
      }
    }
  }
  void SkinnedModel::Dispose()
  {
    //delete the vao's and vbo's and reset them to 0
    for (auto& mesh : m_meshes)
    {
      mesh.Dispose();
    }
    m_meshes.clear();
  }
  void SkinnedModel::Update(float _elapsed)
  {
    if (m_play)
    {
      float timeInTicks = _elapsed * m_animations[m_currentAnimation].m_ticksPerSecond;
      UpdateBoneTree(timeInTicks, &m_animations[m_currentAnimation].root, glm::mat4(1.0f));
    }
  }
  void SkinnedModel::Draw(GLSLProgram & _shader)
  {
    glm::mat4 modelMatrix(1.0f);
    glm::mat4 positionMatrix = glm::translate(glm::mat4(1.0f), m_position);
    glm::mat4 rotationMatrix = glm::mat4_cast(m_rotation);
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), m_scale);

    modelMatrix = positionMatrix * rotationMatrix * scaleMatrix;
    _shader.UploadValue("transformMatrix", modelMatrix);
    glUniformMatrix4fv(_shader.GetUniformLocation("gBones"), m_animations[m_currentAnimation].m_boneTrans.size(),
      GL_FALSE, (GLfloat*)&m_animations[m_currentAnimation].m_boneTrans[0][0]);
    for (GLuint i = 0; i < m_meshes.size(); i++)
    {
      //Draw each mesh
      m_meshes.at(i).Draw(_shader);
    }
  }
  void SkinnedModel::SetAnimation(const std::string& _animName)
  {
    for (GLuint animIndex = 0; animIndex < m_animations.size(); animIndex++)
    {
      if (m_animations[animIndex].m_name == _animName)
      {
        m_currentAnimation = animIndex;
        return;
      }
    }
  }

  void SkinnedModel::OffsetPosition(const glm::vec3 _position)
  {
    m_position += _position;
  }

  void SkinnedModel::OffsetRotation(const glm::vec3 _rotation)
  {
    m_rotation += glm::quat(_rotation);
  }

  void SkinnedModel::OffsetScale(const glm::vec3 _scale)
  {
    m_scale += _scale;
  }

  void SkinnedModel::SetPosition(const glm::vec3 _position)
  {
    m_position = _position;
  }

  void SkinnedModel::SetRotation(const glm::vec3 _rotation)
  {
    m_rotation = glm::quat(_rotation);
  }

  void SkinnedModel::SetScale(const glm::vec3 _scale)
  {
    m_scale = _scale;
  }

  void SkinnedModel::UpdateBoneTree(float _deltaTIme, Animation::BoneNode* _node, const glm::mat4& _parentTransform)
  {
    GLint channelIndex = 0;
    std::string boneName = _node->m_name;

    for (GLuint x = 0; x < m_animations[m_currentAnimation].m_channels.size(); x++)
    {
      if (boneName == m_animations[m_currentAnimation].m_channels[x].m_name)
      {
        channelIndex = x;
      }
    }
    float animTime = std::fmod(_deltaTIme, m_animations[m_currentAnimation].m_duration);

    glm::vec3 translation = CalcInterpolatedPosition(animTime, m_animations[m_currentAnimation].m_channels[channelIndex]);
    glm::vec3 scaling = CalcInterpolatedScaling(animTime, m_animations[m_currentAnimation].m_channels[channelIndex]);
    glm::mat4 rotation = CalcInterpolatedRotation(animTime, m_animations[m_currentAnimation].m_channels[channelIndex]);

    _node->m_nodeTransform = glm::translate(glm::mat4(1.0f), translation)
      * rotation
      * glm::scale(glm::mat4(1.0f), scaling);

    glm::mat4 finalModel = _parentTransform * _node->m_nodeTransform;

    m_animations[m_currentAnimation].m_boneTrans[m_findBoneIDbyName[boneName]] =
      m_globalInverseTransform *
      finalModel *
      _node->m_boneTransform;

    // loop through every child and use this bone's transformations as the parent transform
    for (GLuint x = 0; x < _node->m_children.size(); x++)
    {
      UpdateBoneTree(_deltaTIme, &_node->m_children[x], finalModel);
    }
  }

  GLuint SkinnedModel::FindPositionKey(float _animTime, Animation::Channel& _channel)
  {
    for (GLuint i = 0; i < _channel.m_positionKeys.size() - 1; i++)
    {
      if (_animTime < (float)_channel.m_positionKeys.at(i + 1).mTime)
      {
        return i;
      }
    }

    assert(0);

    return 0;
  }

  GLuint SkinnedModel::FindRotationKey(float _animTime, Animation::Channel& _channel)
  {
    for (GLuint i = 0; i < _channel.m_rotationKeys.size() - 1; i++)
    {
      if (_animTime < (float)_channel.m_rotationKeys.at(i + 1).mTime)
      {
        return i;
      }
    }

    assert(0);

    return 0;
  }
  GLuint SkinnedModel::FindScalingKey(float _animTime, Animation::Channel& _channel)
  {
    for (GLuint i = 0; i < _channel.m_scalingKeys.size() - 1; i++)
    {
      if (_animTime < (float)_channel.m_scalingKeys.at(i + 1).mTime)
      {
        return i;
      }
    }

    assert(0);

    return 0;
  }

  glm::vec3 SkinnedModel::CalcInterpolatedPosition(float _animTime, Animation::Channel& _channel)
  {
    glm::vec3 result(0.0f);
    if (_channel.m_positionKeys.size() == 1)
    {
      // we need at least two values to interpolate...
      result = glm::vec3(_channel.m_positionKeys.at(0).mValue.x, _channel.m_positionKeys.at(0).mValue.y, _channel.m_positionKeys.at(0).mValue.z);
      return result;
    }

    GLuint positionIndex = FindPositionKey(_animTime, _channel);
    GLuint nextPositionIndex = (positionIndex + 1);
    assert(nextPositionIndex < _channel.m_positionKeys.size());
    float deltaTime = (float)(_channel.m_positionKeys.at(nextPositionIndex).mTime - _channel.m_positionKeys.at(positionIndex).mTime);
    float factor = (_animTime - (float)_channel.m_positionKeys.at(positionIndex).mTime) / deltaTime;
    assert(factor >= 0.0f && factor <= 1.0f);
    const aiVector3D& start = _channel.m_positionKeys.at(positionIndex).mValue;
    const aiVector3D& end = _channel.m_positionKeys.at(nextPositionIndex).mValue;
    aiVector3D delta = end - start;
    aiVector3D resultingVec = start + (factor * delta);
    result = glm::vec3(resultingVec.x, resultingVec.y, resultingVec.z);
    return result;
  }

  glm::mat4 SkinnedModel::CalcInterpolatedRotation(float _animTime, Animation::Channel& _channel)
  {
    glm::quat result;
    aiQuaternion aiRotation;

    if (_channel.m_rotationKeys.size() == 1)
    {
      // we need at least two values to interpolate...
      aiRotation = _channel.m_rotationKeys.at(0).mValue;
      result = glm::quat((GLfloat)aiRotation.w, (GLfloat)aiRotation.x, (GLfloat)aiRotation.y, (GLfloat)aiRotation.z);
      return glm::mat4_cast(result);
    }

    GLuint rotationIndex = FindRotationKey(_animTime, _channel);
    GLuint nextRotationIndex = (rotationIndex + 1);
    assert(nextRotationIndex < _channel.m_positionKeys.size());
    float deltaTime = (float)(_channel.m_rotationKeys.at(nextRotationIndex).mTime - _channel.m_rotationKeys.at(rotationIndex).mTime);
    float factor = (_animTime - (float)_channel.m_rotationKeys.at(rotationIndex).mTime) / deltaTime;
    assert(factor >= 0.0f && factor <= 1.0f);
    const aiQuaternion& StartRotationQ = _channel.m_rotationKeys.at(rotationIndex).mValue;
    const aiQuaternion& EndRotationQ = _channel.m_rotationKeys.at(nextRotationIndex).mValue;
    aiQuaternion::Interpolate(aiRotation, StartRotationQ, EndRotationQ, factor);
    aiRotation = aiRotation.Normalize();
    result = glm::quat((GLfloat)aiRotation.w, (GLfloat)aiRotation.x, (GLfloat)aiRotation.y, (GLfloat)aiRotation.z);
    return glm::mat4_cast(result);
  }

  glm::vec3 SkinnedModel::CalcInterpolatedScaling(float _animTime, Animation::Channel& _channel)
  {
    glm::vec3 result(0.0f);
    if (_channel.m_scalingKeys.size() == 1)
    {
      // we need at least two values to interpolate...
      result = glm::vec3(_channel.m_scalingKeys.at(0).mValue.x, _channel.m_scalingKeys.at(0).mValue.y, _channel.m_scalingKeys.at(0).mValue.z);
      return result;
    }

    GLuint scalingIndex = FindScalingKey(_animTime, _channel);
    GLuint nextScalingIndex = (scalingIndex + 1);
    assert(nextScalingIndex < _channel.m_scalingKeys.size());
    float deltaTime = (float)(_channel.m_scalingKeys.at(nextScalingIndex).mTime - _channel.m_scalingKeys.at(scalingIndex).mTime);
    float factor = (_animTime - (float)_channel.m_scalingKeys.at(scalingIndex).mTime) / deltaTime;
    assert(factor >= 0.0f && factor <= 1.0f);
    const aiVector3D& start = _channel.m_scalingKeys.at(scalingIndex).mValue;
    const aiVector3D& end = _channel.m_scalingKeys.at(nextScalingIndex).mValue;
    aiVector3D delta = end - start;
    aiVector3D resultingVec = start + (factor * delta);
    result = glm::vec3(resultingVec.x, resultingVec.y, resultingVec.z);
    return result;
  }

  void StaticModel::Draw(GLSLProgram & _shader)
  {
    glm::mat4 modelMatrix(1.0f);
    glm::mat4 positionMatrix = glm::translate(glm::mat4(1.0f), m_position);
    glm::mat4 rotationMatrix = glm::mat4_cast(m_rotation);
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), m_scale);

    modelMatrix = positionMatrix * rotationMatrix * scaleMatrix;
    _shader.UploadValue("transformMatrix", modelMatrix);

    for (auto& mesh : m_meshes)
    {
      //Draw each mesh
      mesh.Draw(_shader);
    }
  }

  void StaticModel::DrawInstanced(GLSLProgram & _shader, std::vector<glm::mat4>& _modelMatrices)
  {
    for (GLuint i = 0; i < m_meshes.size(); i++)
    {
      glBindBuffer(GL_ARRAY_BUFFER, m_meshes.at(i).GetMBO());
      glBufferData(GL_ARRAY_BUFFER, _modelMatrices.size() * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
      glBufferSubData(GL_ARRAY_BUFFER, 0, _modelMatrices.size() * sizeof(glm::mat4), _modelMatrices.data());
      //Draw each mesh
      m_meshes.at(i).Draw(_shader, _modelMatrices.size());
    }
  }

  void StaticModel::OffsetPosition(const glm::vec3 _position)
  {
    m_position += _position;
  }

  void StaticModel::OffsetRotation(const glm::vec3 _rotation)
  {
    m_rotation += glm::quat(_rotation);
  }

  void StaticModel::OffsetScale(const glm::vec3 _scale)
  {
    m_scale += _scale;
  }

  void StaticModel::SetPosition(const glm::vec3 _position)
  {
    m_position = _position;
  }

  void StaticModel::SetRotation(const glm::vec3 _rotation)
  {
    m_rotation = glm::quat(_rotation);
  }

  void StaticModel::SetScale(const glm::vec3 _scale)
  {
    m_scale = _scale;
  }
  void StaticModel::Dispose()
  {
    //delete the vao's and vbo's and reset them to 0
    for (auto& mesh : m_meshes)
    {
      mesh.Dispose();
    }
    m_meshes.clear();
  }
  
}
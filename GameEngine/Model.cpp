#include "Model.h"
#include "ResourceManager.h"

#include <iostream>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtc\matrix_transform.hpp>
namespace GameEngine
{
  glm::mat4 AssimpToGlmMat4(const aiMatrix4x4* _aiMatrix);

#define INVALID_BONE_ID -999
#define MAX_BONES 100

  //differentiating the nodes and bone nodes
  void Model::Animation::BuildBoneTree(const aiScene* _scene, aiNode* _node, BoneNode* _bNode, Model* _m)
  {
    if (_scene->HasAnimations())
    {
      //Check if the passed node is a bone node, by cheching to see if the name matches with a bone ID 
      if (_m->m_findBoneIDbyName.find(_node->mName.data) != _m->m_findBoneIDbyName.end())
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
      if (_m->m_findBoneIDbyName.find(_node->mName.data) != _m->m_findBoneIDbyName.end())
      {
        BuildBoneTree(_scene, _node->mChildren[x], &_bNode->m_children[_bNode->m_children.size() - 1], _m);
      }
      else
      {
        BuildBoneTree(_scene, _node->mChildren[x], _bNode, _m);
      }
    }
  }

  Model::Model()
  {
  }


  Model::~Model()
  {
  }

  void Model::Draw(GLSLProgram& _shader, std::vector<glm::mat4> _modelMatrices)
  {
    for (GLuint i = 0; i < m_meshes.size(); i++)
    {
      //upload the model matrices data into the shader
      glBindBuffer(GL_ARRAY_BUFFER, m_meshes.at(i).GetMBO());
      glBufferData(GL_ARRAY_BUFFER, _modelMatrices.size() * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
      glBufferSubData(GL_ARRAY_BUFFER, 0, _modelMatrices.size() * sizeof(glm::mat4), _modelMatrices.data());
      if (m_hasAnimation)
      {
        //Upload the bone transformation if there is animations
        glUniformMatrix4fv(_shader.GetUniformLocation("gBones"), m_animations[m_currentAnimation].m_boneTrans.size(),
          GL_FALSE, (GLfloat*)&m_animations[m_currentAnimation].m_boneTrans[0][0]);
      }
      //Draw each mesh
      m_meshes.at(i).Draw(_shader, _modelMatrices.size());
    }
  }

  void Model::SetAnimation(const std::string& _animName)
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

  void Model::PlayAnimation(float _ticks)
  {
    float timeInTicks = _ticks * m_animations[m_currentAnimation].m_ticksPerSecond;

    UpdateBoneTree(timeInTicks, &m_animations[m_currentAnimation].root, glm::mat4(1.0f));
  }

  void Model::Dispose()
  {
    //delete the vao's and vbo's and reset them to 0
    for (GLuint i = 0; i < m_meshes.size(); i++)
    {
      m_meshes.at(i).Dispose();
    }
  }


  void Model::LoadModel(const std::string& _path)
  {
    //cleach the mesh vector
    m_meshes.clear();

    //use assimp to import the model
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    //error check
    if (!scene || !scene->mRootNode || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE)
    {
      std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
      return;
    }

    m_directory = _path.substr(0, _path.find_last_of('/'));
    m_meshes.resize(scene->mNumMeshes);
    m_hasAnimation = scene->HasAnimations();

    if (m_hasAnimation)
    {
      //if the model has animations get the global inverse transform
      m_globalInverseTransform = AssimpToGlmMat4(&scene->mRootNode->mTransformation);
      glm::inverse(m_globalInverseTransform);
      //Process all the animations
      ProcessAnimations(scene);
    }
    //Process all the nodes(setup the meshes)
    ProcessNode(scene->mRootNode, scene);

    if (m_hasAnimation)
    {
      //Build the bone trees for all animations
      for (GLuint i = 0; i < m_animations.size(); i++)
      {
        m_animations[i].BuildBoneTree(scene, scene->mRootNode, &m_animations[i].root, this);
      }
    }
  }

  void Model::ProcessNode(aiNode* _node, const aiScene* _scene)
  {
    // Process all the node's meshes (if any)
    for (GLuint i = 0; i < _node->mNumMeshes; i++)
    {
      // The node object only contains indices to index the actual objects in the scene. 
      // The scene contains all the data, node is just to keep stuff organized.
      aiMesh* mesh = _scene->mMeshes[_node->mMeshes[i]];
      m_meshes.push_back(ProcessMesh(mesh, _scene, _node));
    }

    // Then do the same for each of its children
    for (GLuint i = 0; i < _node->mNumChildren; i++)
    {
      // Child nodes are actually stored in the node, not in the scene (which makes sense since nodes only contain
      // links and indices, nothing more, so why store that in the scene)
      ProcessNode(_node->mChildren[i], _scene);
    }
  }

  Mesh Model::ProcessMesh(aiMesh* _mesh, const aiScene* _scene, aiNode* _node)
  {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<GLTexture> textures;

    //reserve space for the vectors for optimization
    vertices.reserve(_mesh->mNumVertices);
    indices.reserve(_mesh->mNumFaces * 3);

    //The base model matrix from the node
    glm::mat4 baseModelMatrix = AssimpToGlmMat4(&_node->mTransformation);
    if (_node->mParent != nullptr)
    {
      baseModelMatrix = AssimpToGlmMat4(&_node->mParent->mTransformation) * baseModelMatrix;
    }

    for (GLuint i = 0; i < _mesh->mNumVertices; i++)
    {
      Vertex vertex;

      vertex.SetPosition(_mesh->mVertices[i].x, _mesh->mVertices[i].y, _mesh->mVertices[i].z);

      if (_mesh->HasNormals())
      {
        vertex.SetNormal(_mesh->mNormals[i].x, _mesh->mNormals[i].y, _mesh->mNormals[i].z);
      }
      if (_mesh->HasTextureCoords(0))
      {
        vertex.SetUV(_mesh->mTextureCoords[0][i].x, _mesh->mTextureCoords[0][i].y);
      }
      if (_mesh->HasTangentsAndBitangents())
      {
        vertex.SetTangents(_mesh->mTangents[i].x, _mesh->mTangents[i].y, _mesh->mTangents[i].z);
      }

      if (m_hasAnimation)
      {
        vertex.SetBoneIDs(glm::ivec4(INVALID_BONE_ID));
        vertex.SetBoneWeights(glm::vec4(1.0f));
      }

      vertices.push_back(vertex);
    }

    if (m_hasAnimation)
    {
      for (GLuint x = 0; x < _mesh->mNumBones; x++)
      {
        // bone index, decides what bone we modify
        GLuint boneIndex = 0;
        //the name of the current bone
        std::string boneName = _mesh->mBones[x]->mName.data;

        //check if there is an ID associated to this boneName already
        if (m_findBoneIDbyName.find(boneName) == m_findBoneIDbyName.end())
        {
          //if there is no ID to this boneName
          //create a new bone
          //current index is the new bone
          boneIndex = m_findBoneIDbyName.size();
        }
        else
        {
          //if a bone ID is already associated to this boneName
          //set the boneindex to the index associated to this boneName
          boneIndex = m_findBoneIDbyName[boneName];
        }

        //Set the ID of the current bone name to the bone index (whether it be new or the same)
        m_findBoneIDbyName[boneName] = boneIndex;

        //iterate through all the channels(bones)
        for (GLuint y = 0; y < m_animations[m_currentAnimation].m_channels.size(); y++)
        {
          //check which channel corresponds to this bone
          if (m_animations[m_currentAnimation].m_channels[y].m_name == boneName)
          {
            //Set the bone offset of this bone name to the corresponding offset matrix from the aimesh
            m_animations[m_currentAnimation].m_findBoneOffsetByName[boneName] = AssimpToGlmMat4(&_mesh->mBones[x]->mOffsetMatrix);
            m_animations[m_currentAnimation].m_channels[y].m_offset = AssimpToGlmMat4(&_mesh->mBones[x]->mOffsetMatrix);
          }
        }

        //Iterate through all the weights of this bone
        for (GLuint y = 0; y < _mesh->mBones[x]->mNumWeights; y++)
        {
          GLuint vertexID = _mesh->mBones[x]->mWeights[y].mVertexId;
          GLfloat weight = _mesh->mBones[x]->mWeights[y].mWeight;
          // first we check if the boneID at this vertex ID has any filled in
          // if it does then we need to fill the weight vector with the same value
          if (vertices.at(vertexID).m_boneIDs.x == INVALID_BONE_ID)
          {
            vertices.at(vertexID).m_boneIDs.x = boneIndex;
            vertices.at(vertexID).m_weights.x = weight;
          }
          else if (vertices.at(vertexID).m_boneIDs.y == INVALID_BONE_ID)
          {
            vertices.at(vertexID).m_boneIDs.y = boneIndex;
            vertices.at(vertexID).m_weights.y = weight;
          }
          else if (vertices.at(vertexID).m_boneIDs.z == INVALID_BONE_ID)
          {
            vertices.at(vertexID).m_boneIDs.z = boneIndex;
            vertices.at(vertexID).m_weights.z = weight;
          }
          else if (vertices.at(vertexID).m_boneIDs.w == INVALID_BONE_ID)
          {
            vertices.at(vertexID).m_boneIDs.w = boneIndex;
            vertices.at(vertexID).m_weights.w = weight;
          }
        }
      }
    }
    //Set the faces
    for (GLuint i = 0; i < _mesh->mNumFaces; i++)
    {
      aiFace face = _mesh->mFaces[i];
      for (GLuint j = 0; j < face.mNumIndices; j++)
      {
        indices.push_back(face.mIndices[j]);
      }
    }

    //set the material textures
    if (_mesh->mMaterialIndex >= 0)
    {
      aiMaterial* material = _scene->mMaterials[_mesh->mMaterialIndex];

      // 1. Diffuse maps
      std::vector<GLTexture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
      textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

      // 2. Specular maps
      std::vector<GLTexture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
      textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

      /* 3. Reflection maps (Note that ASSIMP doesn't load reflection maps properly from wavefront objects,
      so we'll cheat a little by defining the reflection maps as ambient maps in the .obj file, which ASSIMP is able to load)*/
      std::vector<GLTexture> ambientMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_reflection");
      textures.insert(textures.end(), ambientMaps.begin(), ambientMaps.end());

      // 4. Normal maps
      std::vector<GLTexture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
      textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    }

    return Mesh(vertices, indices, textures, m_hasAnimation, baseModelMatrix);
  }

  void Model::ProcessAnimations(const aiScene* scene)
  {
    for (GLuint animationIndex = 0; animationIndex < scene->mNumAnimations; animationIndex++)
    {
      //set up the animation
      Animation tempAnim;
      tempAnim.m_name = scene->mAnimations[animationIndex]->mName.data;
      tempAnim.m_duration = scene->mAnimations[animationIndex]->mDuration;
      tempAnim.m_ticksPerSecond = (scene->mAnimations[animationIndex]->mTicksPerSecond) != 0 ? scene->mAnimations[animationIndex]->mTicksPerSecond : 25.0f;

      // load in required data for animation so that we don't have to save the entire scene
      for (GLuint channelIndex = 0; channelIndex < scene->mAnimations[animationIndex]->mNumChannels; channelIndex++)
      {
        Animation::Channel tempChan;
        tempChan.m_name = scene->mAnimations[animationIndex]->mChannels[channelIndex]->mNodeName.data;

        for (GLuint keyIndex = 0; keyIndex < scene->mAnimations[animationIndex]->mChannels[channelIndex]->mNumPositionKeys; keyIndex++)
        {
          tempChan.m_positionKeys.push_back(scene->mAnimations[animationIndex]->mChannels[channelIndex]->mPositionKeys[keyIndex]);
        }

        for (GLuint keyIndex = 0; keyIndex < scene->mAnimations[animationIndex]->mChannels[channelIndex]->mNumRotationKeys; keyIndex++)
        {
          tempChan.m_rotationKeys.push_back(scene->mAnimations[animationIndex]->mChannels[channelIndex]->mRotationKeys[keyIndex]);
        }
        for (GLuint keyIndex = 0; keyIndex < scene->mAnimations[animationIndex]->mChannels[channelIndex]->mNumScalingKeys; keyIndex++)
        {
          tempChan.m_scalingKeys.push_back(scene->mAnimations[animationIndex]->mChannels[channelIndex]->mScalingKeys[keyIndex]);
        }
        tempAnim.m_channels.push_back(tempChan);
      }

      for (GLuint z = 0; z < MAX_BONES; z++)
      {
        tempAnim.m_boneTrans.push_back(glm::mat4(1.0f));
      }

      m_animations.push_back(tempAnim);
    }

    m_currentAnimation = 0;
    m_animations[m_currentAnimation].root.m_name = "rootBoneTreeNode";
  }

  std::vector<GLTexture> Model::LoadMaterialTextures(aiMaterial* _mat, const aiTextureType& _type, const std::string& _typeName)
  {
    std::vector<GLTexture> textures;

    for (GLuint i = 0; i < _mat->GetTextureCount(_type); i++)
    {
      aiString str;
      _mat->GetTexture(_type, i, &str);
      GLTexture texture;
      std::cout << m_directory + '/' + str.C_Str() << std::endl;
      texture = ResourceManager::GetTexture(m_directory + '/' + str.C_Str());
      texture.type = _typeName;
      textures.push_back(texture);
    }
    return textures;
  }

  void Model::UpdateBoneTree(float _deltaTIme, Animation::BoneNode* _node, const glm::mat4& _parentTransform)
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

  GLuint Model::FindPositionKey(float _animTime, Animation::Channel& _channel)
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

  GLuint Model::FindRotationKey(float _animTime, Animation::Channel& _channel)
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
  GLuint Model::FindScalingKey(float _animTime, Animation::Channel& _channel)
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

  glm::vec3 Model::CalcInterpolatedPosition(float _animTime, Animation::Channel& _channel)
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

  glm::mat4 Model::CalcInterpolatedRotation(float _animTime, Animation::Channel& _channel)
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

  glm::vec3 Model::CalcInterpolatedScaling(float _animTime, Animation::Channel& _channel)
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

  glm::mat4 AssimpToGlmMat4(const aiMatrix4x4* _aiMatrix)
  {
    glm::mat4 mat;
    mat[0][0] = _aiMatrix->a1; mat[1][0] = _aiMatrix->a2; mat[2][0] = _aiMatrix->a3; mat[3][0] = _aiMatrix->a4;
    mat[0][1] = _aiMatrix->b1; mat[1][1] = _aiMatrix->b2; mat[2][1] = _aiMatrix->b3; mat[3][1] = _aiMatrix->b4;
    mat[0][2] = _aiMatrix->c1; mat[1][2] = _aiMatrix->c2; mat[2][2] = _aiMatrix->c3; mat[3][2] = _aiMatrix->c4;
    mat[0][3] = _aiMatrix->d1; mat[1][3] = _aiMatrix->d2; mat[2][3] = _aiMatrix->d3; mat[3][3] = _aiMatrix->d4;
    return mat;
  }
}
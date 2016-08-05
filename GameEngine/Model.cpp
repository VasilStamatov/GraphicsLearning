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
  void Model::Animation::BuildBoneTree(const aiScene* _scene, aiNode* _node, BoneNode* _bNode, Model* _m)
  {
    if (_scene->HasAnimations())
    {
      // found the node
      if (_m->m_findBoneIDbyName.find(_node->mName.data) != _m->m_findBoneIDbyName.end())
      {
        BoneNode tempNode;
        tempNode.m_name = _node->mName.data;
        tempNode.m_parent = _bNode;
        tempNode.m_nodeTransform = AssimpToGlmMat4(&_node->mTransformation);
        // bones and their nodes always share the same name
        tempNode.m_boneTransform = m_findBoneOffsetByName[tempNode.m_name];
        _bNode->m_children.push_back(tempNode);
      }
    }

    if (_node->mNumChildren > 0)
    {
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
      glBindBuffer(GL_ARRAY_BUFFER, m_meshes.at(i).GetMBO());
      glBufferData(GL_ARRAY_BUFFER, _modelMatrices.size() * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
      glBufferSubData(GL_ARRAY_BUFFER, 0, _modelMatrices.size() * sizeof(glm::mat4), _modelMatrices.data());
      if (m_hasAnimation)
      {
        glUniformMatrix4fv(_shader.GetUniformLocation("gBones"), m_animations[m_currentAnimation].m_boneTrans.size(),
          GL_FALSE, (GLfloat*)&m_animations[m_currentAnimation].m_boneTrans[0][0]);
      }
      m_meshes.at(i).Draw(_shader, _modelMatrices.size());
    }
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
    m_meshes.clear();

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

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
      m_globalInverseTransform = AssimpToGlmMat4(&scene->mRootNode->mTransformation);
      glm::inverse(m_globalInverseTransform);
      ProcessAnimations(scene);
    }

    ProcessNode(scene->mRootNode, scene);
    if (m_hasAnimation)
    {
      m_animations[m_currentAnimation].BuildBoneTree(scene, scene->mRootNode, &m_animations[m_currentAnimation].root, this);
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
    
    glm::mat4 baseModelMatrix;

    baseModelMatrix = AssimpToGlmMat4(&_node->mTransformation);
    if (_node->mParent != nullptr)
    {
      baseModelMatrix = AssimpToGlmMat4(&_node->mParent->mTransformation) * baseModelMatrix;
    }

    vertices.reserve(_mesh->mNumVertices);
    indices.reserve(_mesh->mNumFaces * 3);

    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    for (GLuint i = 0; i < _mesh->mNumVertices; i++)
    {
      Vertex vertex;
      const aiVector3D* pPos = &(_mesh->mVertices[i]);
      const aiVector3D* pNormal = _mesh->HasNormals() ? &(_mesh->mNormals[i]) : &Zero3D;
      const aiVector3D* pTexCoord = _mesh->HasTextureCoords(0) ? &(_mesh->mTextureCoords[0][i]) : &Zero3D;
      const aiVector3D* pTangets = _mesh->HasTangentsAndBitangents() ? &(_mesh->mTangents[i]) : &Zero3D;
      vertex.SetPosition(pPos->x, pPos->y, pPos->z);
      vertex.SetNormal(pNormal->x, pNormal->y, pNormal->z);
      vertex.SetUV(pTexCoord->x, pTexCoord->y);
      vertex.SetTangents(pTangets->x, pTangets->y, pTangets->z);

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
        std::string boneName = _mesh->mBones[x]->mName.data;

        if (m_findBoneIDbyName.find(boneName) == m_findBoneIDbyName.end())
        { // create a new bone
          // current index is the new bone
          boneIndex = m_findBoneIDbyName.size();
        }
        else
        {
          boneIndex = m_findBoneIDbyName[boneName];
        }

        m_findBoneIDbyName[boneName] = boneIndex;

        for (GLuint y = 0; y < m_animations[m_currentAnimation].m_channels.size(); y++)
        {
          if (m_animations[m_currentAnimation].m_channels[y].m_name == boneName)
          {
            m_animations[m_currentAnimation].m_findBoneOffsetByName[boneName] = AssimpToGlmMat4(&_mesh->mBones[x]->mOffsetMatrix);
          }
        }

        for (GLuint y = 0; y < _mesh->mBones[x]->mNumWeights; y++)
        {
          GLuint vertexID = _mesh->mBones[x]->mWeights[y].mVertexId;
          GLfloat weight = _mesh->mBones[x]->mWeights[y].mWeight;
          // first we check if the boneid vector has any filled in
          // if it does then we need to fill the weight vector with the same value

          /*for (GLuint i = 0; i < 4; i++) 
          {
            if (vertices.at(vertexID).m_weights[i] == 0.0) 
            {
              vertices.at(vertexID).m_boneIDs[i] = boneIndex;
              vertices.at(vertexID).m_weights[i] = weight;
              break;
            }
          }*/

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

    for (GLuint i = 0; i < _mesh->mNumFaces; i++)
    {
      aiFace face = _mesh->mFaces[i];
      for (GLuint j = 0; j < face.mNumIndices; j++)
      {
        indices.push_back(face.mIndices[j]);
      }
    }

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

      // 2. Normal maps
      std::vector<GLTexture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
      textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    }

    return Mesh(vertices, indices, textures, m_hasAnimation);
  }

  void Model::ProcessAnimations(const aiScene* scene)
  {
    for (GLuint animationIndex = 0; animationIndex < scene->mNumAnimations; animationIndex++)
    {
      Animation tempAnim;
      tempAnim.m_name = scene->mAnimations[animationIndex]->mName.data;
      tempAnim.m_duration = scene->mAnimations[animationIndex]->mDuration;
      tempAnim.m_ticksPerSecond = scene->mAnimations[animationIndex]->mTicksPerSecond;
      //tempAnim.data = scene->mAnimations[animationIndex];

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

  void Model::UpdateAnimation(float _deltaTIme)
  {
    float timeInTicks = _deltaTIme * m_animations[m_currentAnimation].m_ticksPerSecond;

    UpdateBoneTree(timeInTicks, &m_animations[m_currentAnimation].root, glm::mat4(1.0f));
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

    aiQuaternion aiRotation(m_animations[m_currentAnimation].m_channels[channelIndex].m_rotationKeys[0].mValue);
    aiVector3D aiTranslation(m_animations[m_currentAnimation].m_channels[channelIndex].m_positionKeys[0].mValue);
    aiVector3D aiScale(m_animations[m_currentAnimation].m_channels[channelIndex].m_scalingKeys[0].mValue);

    Assimp::Interpolator<aiQuaternion> slerp;
    Assimp::Interpolator<aiVector3D> lerp;

    // get the two animation keys it is between for lerp and slerp
    int key1, key2;
    if (std::round(animTime) < animTime)
    {
      key1 = std::round(animTime);
      key2 = key1 + 1;
    }
    else
    {
      key1 = std::round(animTime) - 1;
      key2 = std::round(animTime);
    }

    if (m_animations[m_currentAnimation].m_channels[channelIndex].m_positionKeys.size() > 1)
    {
      lerp(aiTranslation, m_animations[m_currentAnimation].m_channels[channelIndex].m_positionKeys[key1].mValue,
        m_animations[m_currentAnimation].m_channels[channelIndex].m_positionKeys[key2].mValue, animTime - key1); // translation
      aiTranslation.Normalize();
    }
    if (m_animations[m_currentAnimation].m_channels[channelIndex].m_scalingKeys.size() > 1)
    {
      lerp(aiScale, m_animations[m_currentAnimation].m_channels[channelIndex].m_scalingKeys[key1].mValue,
        m_animations[m_currentAnimation].m_channels[channelIndex].m_scalingKeys[key2].mValue, animTime - key1); // scale
      aiScale.Normalize();
    }
    if (m_animations[m_currentAnimation].m_channels[channelIndex].m_rotationKeys.size() > 1)
    {
      slerp(aiRotation, m_animations[m_currentAnimation].m_channels[channelIndex].m_rotationKeys[key1].mValue,
        m_animations[m_currentAnimation].m_channels[channelIndex].m_rotationKeys[key2].mValue, animTime - key1); // rotation
      aiRotation.Normalize();
    }
    glm::vec3 translation((GLfloat)aiTranslation.x, (GLfloat)aiTranslation.y, (GLfloat)aiTranslation.z);
    glm::vec3 scaling((GLfloat)aiScale.x, (GLfloat)aiScale.y, (GLfloat)aiScale.z);
    glm::quat rotation((GLfloat)aiRotation.w, (GLfloat)aiRotation.x, (GLfloat)aiRotation.y, (GLfloat)aiRotation.z);

    glm::mat4 finalModel =
      _parentTransform
      * glm::translate(glm::mat4(1.0f), translation)
      * glm::mat4_cast(rotation)
      * glm::scale(glm::mat4(1.0f), scaling);

    m_animations[m_currentAnimation].m_boneTrans[m_findBoneIDbyName[boneName]] =
      m_globalInverseTransform *
      finalModel *
      m_animations[m_currentAnimation].m_findBoneOffsetByName[boneName];

    // loop through every child and use this bone's transformations as the parent transform
    for (GLuint x = 0; x < _node->m_children.size(); x++)
    {
      UpdateBoneTree(_deltaTIme, &_node->m_children[x], finalModel);
    }
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
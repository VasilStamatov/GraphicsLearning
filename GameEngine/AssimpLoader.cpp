#include "AssimpLoader.h"
#include "ResourceManager.h"

#include <assimp\Importer.hpp>
#include <iostream>

namespace GameEngine
{
  constexpr int INVALID_BONE_ID = -999;
  constexpr int MAX_BONES = 100;

  /*
    Load a static model
  */
  bool AssimpLoader::LoadStaticModel(const std::string& _path, StaticModel* _model)
  {
    //use assimp to import the model
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    //error check
    if (!scene || !scene->mRootNode || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE)
    {
      std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
      return false;
    }

    m_directory = _path.substr(0, _path.find_last_of('/'));
    _model->m_meshes.resize(scene->mNumMeshes);

    //Process all the nodes(setup the meshes)
    ProcessNode(scene->mRootNode, scene, _model);
    return true;
  }

  /*
    Load a skined model
  */
  bool AssimpLoader::LoadSkinnedModel(const std::string & _path, SkinnedModel* _model)
  {
    //use assimp to import the model
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    //error check
    if (!scene || !scene->mRootNode || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE)
    {
      std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
      return false;
    }

    m_directory = _path.substr(0, _path.find_last_of('/'));
    _model->m_meshes.reserve(scene->mNumMeshes);


    //if the model has animations get the global inverse transform
    _model->m_globalInverseTransform = AssimpToGlmMat4(&scene->mRootNode->mTransformation.Inverse());
    //Process all the animations
    ProcessAnimations(scene, _model);
    //Process all the nodes(setup the meshes)
    ProcessNode(scene->mRootNode, scene, _model);

    //Build the bone trees for all animations
    for (GLuint i = 0; i < _model->m_animations.size(); i++)
    {
      _model->m_animations.at(i).BuildBoneTree(scene, scene->mRootNode, &_model->m_animations.at(i).root, _model);
    }
    return true;

  }

  void AssimpLoader::ProcessNode(aiNode * _node, const aiScene * _scene, StaticModel* _model)
  {
    // Process all the node's meshes (if any)
    for (GLuint i = 0; i < _node->mNumMeshes; i++)
    {
      // The node object only contains indices to index the actual objects in the scene. 
      // The scene contains all the data, node is just to keep stuff organized.
      aiMesh* mesh = _scene->mMeshes[_node->mMeshes[i]];
      _model->m_meshes.push_back(ProcessMesh(mesh, _scene, _node));
    }

    // Then do the same for each of its children
    for (GLuint i = 0; i < _node->mNumChildren; i++)
    {
      // Child nodes are actually stored in the node, not in the scene (which makes sense since nodes only contain
      // links and indices, nothing more, so why store that in the scene)
      ProcessNode(_node->mChildren[i], _scene, _model);
    }
  }

  void AssimpLoader::ProcessNode(aiNode * _node, const aiScene * _scene, SkinnedModel* _model)
  {
    // Process all the node's meshes (if any)
    for (GLuint i = 0; i < _node->mNumMeshes; i++)
    {
      // The node object only contains indices to index the actual objects in the scene. 
      // The scene contains all the data, node is just to keep stuff organized.
      aiMesh* mesh = _scene->mMeshes[_node->mMeshes[i]];
      _model->m_meshes.push_back(ProcessMesh(mesh, _scene, _node, _model));
    }

    // Then do the same for each of its children
    for (GLuint i = 0; i < _node->mNumChildren; i++)
    {
      // Child nodes are actually stored in the node, not in the scene (which makes sense since nodes only contain
      // links and indices, nothing more, so why store that in the scene)
      ProcessNode(_node->mChildren[i], _scene, _model);
    }
  }

  Mesh AssimpLoader::ProcessMesh(aiMesh * _mesh, const aiScene * _scene, aiNode * _node)
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

    ProcessVertices(_mesh, vertices);

    //Set the faces
    ProcessFaces(_mesh, indices);

    //set the material textures
    ProcessTextures(_scene, _mesh, textures);


    return Mesh(vertices, indices, textures, false, baseModelMatrix);
  }

  Mesh AssimpLoader::ProcessMesh(aiMesh * _mesh, const aiScene * _scene, aiNode * _node, SkinnedModel* _model)
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

    ProcessVertices(_mesh, vertices);


    for (GLuint x = 0; x < _mesh->mNumBones; x++)
    {
      // bone index, decides what bone we modify
      GLuint boneIndex = 0;
      //the name of the current bone
      std::string boneName = _mesh->mBones[x]->mName.data;

      //check if there is an ID associated to this boneName already
      if (_model->m_findBoneIDbyName.find(boneName) == _model->m_findBoneIDbyName.end())
      {
        //if there is no ID to this boneName
        //create a new bone
        //current index is the new bone
        boneIndex = _model->m_findBoneIDbyName.size();
      }
      else
      {
        //if a bone ID is already associated to this boneName
        //set the boneindex to the index associated to this boneName
        boneIndex = _model->m_findBoneIDbyName[boneName];
      }

      //Set the ID of the current bone name to the bone index (whether it be new or the same)
      _model->m_findBoneIDbyName[boneName] = boneIndex;

      //iterate through all the channels(bones)
      for (GLuint y = 0; y < _model->m_animations.at(_model->m_currentAnimation).m_channels.size(); y++)
      {
        //check which channel corresponds to this bone
        if (_model->m_animations.at(_model->m_currentAnimation).m_channels[y].m_name == boneName)
        {
          //Set the bone offset of this bone name to the corresponding offset matrix from the aimesh
          _model->m_animations.at(_model->m_currentAnimation).m_findBoneOffsetByName[boneName] =
            AssimpToGlmMat4(&_mesh->mBones[x]->mOffsetMatrix);
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

    //Set the faces
    ProcessFaces(_mesh, indices);

    //set the material textures
    ProcessTextures(_scene, _mesh, textures);

    return Mesh(vertices, indices, textures, true, baseModelMatrix);
  }

  void AssimpLoader::ProcessAnimations(const aiScene * _scene, SkinnedModel* _model)
  {
    for (GLuint animationIndex = 0; animationIndex < _scene->mNumAnimations; animationIndex++)
    {
      //set up the animation
      SkinnedModel::Animation tempAnim;
      tempAnim.m_name = _scene->mAnimations[animationIndex]->mName.data;
      tempAnim.m_duration = _scene->mAnimations[animationIndex]->mDuration;
      tempAnim.m_ticksPerSecond = (_scene->mAnimations[animationIndex]->mTicksPerSecond) != 0 ? _scene->mAnimations[animationIndex]->mTicksPerSecond : 25.0f;

      // load in required data for animation so that we don't have to save the entire scene
      for (GLuint channelIndex = 0; channelIndex < _scene->mAnimations[animationIndex]->mNumChannels; channelIndex++)
      {
        SkinnedModel::Animation::Channel tempChan;
        tempChan.m_name = _scene->mAnimations[animationIndex]->mChannels[channelIndex]->mNodeName.data;

        for (GLuint keyIndex = 0; keyIndex < _scene->mAnimations[animationIndex]->mChannels[channelIndex]->mNumPositionKeys; keyIndex++)
        {
          tempChan.m_positionKeys.push_back(_scene->mAnimations[animationIndex]->mChannels[channelIndex]->mPositionKeys[keyIndex]);
        }

        for (GLuint keyIndex = 0; keyIndex < _scene->mAnimations[animationIndex]->mChannels[channelIndex]->mNumRotationKeys; keyIndex++)
        {
          tempChan.m_rotationKeys.push_back(_scene->mAnimations[animationIndex]->mChannels[channelIndex]->mRotationKeys[keyIndex]);
        }
        for (GLuint keyIndex = 0; keyIndex < _scene->mAnimations[animationIndex]->mChannels[channelIndex]->mNumScalingKeys; keyIndex++)
        {
          tempChan.m_scalingKeys.push_back(_scene->mAnimations[animationIndex]->mChannels[channelIndex]->mScalingKeys[keyIndex]);
        }
        tempAnim.m_channels.push_back(tempChan);
      }

      for (GLuint z = 0; z < MAX_BONES; z++)
      {
        tempAnim.m_boneTrans.push_back(glm::mat4(1.0f));
      }

      _model->m_animations.push_back(tempAnim);
    }

    _model->m_currentAnimation = 0;
    _model->m_animations.at(_model->m_currentAnimation).root.m_name = "rootBoneTreeNode";
  }

  std::vector<GLTexture> AssimpLoader::LoadMaterialTextures(aiMaterial * _mat, const aiTextureType & _type, const std::string & _typeName)
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

  void AssimpLoader::ProcessVertices(aiMesh* _mesh, std::vector<Vertex>& _vertices)
  {
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

      vertex.SetBoneIDs(glm::ivec4(INVALID_BONE_ID));
      vertex.SetBoneWeights(glm::vec4(1.0f));

      _vertices.push_back(vertex);
    }
  }

  void AssimpLoader::ProcessFaces(aiMesh * _mesh, std::vector<GLuint>& _indices)
  {
    for (GLuint i = 0; i < _mesh->mNumFaces; i++)
    {
      aiFace face = _mesh->mFaces[i];
      for (GLuint j = 0; j < face.mNumIndices; j++)
      {
        _indices.push_back(face.mIndices[j]);
      }
    }
  }

  void AssimpLoader::ProcessTextures(const aiScene* _scene, aiMesh * _mesh, std::vector<GLTexture>& _textures)
  {
    if (_mesh->mMaterialIndex >= 0)
    {
      aiMaterial* material = _scene->mMaterials[_mesh->mMaterialIndex];

      // 1. Diffuse maps
      std::vector<GLTexture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
      _textures.insert(_textures.end(), diffuseMaps.begin(), diffuseMaps.end());

      // 2. Specular maps
      std::vector<GLTexture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
      _textures.insert(_textures.end(), specularMaps.begin(), specularMaps.end());

      /* 3. Reflection maps (Note that ASSIMP doesn't load reflection maps properly from wavefront objects,
      so we'll cheat a little by defining the reflection maps as ambient maps in the .obj file, which ASSIMP is able to load)*/
      std::vector<GLTexture> ambientMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_reflection");
      _textures.insert(_textures.end(), ambientMaps.begin(), ambientMaps.end());

      // 4. Normal maps
      std::vector<GLTexture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
      _textures.insert(_textures.end(), normalMaps.begin(), normalMaps.end());
    }
  }

  glm::mat4 AssimpLoader::AssimpToGlmMat4(const aiMatrix4x4 * _aiMatrix)
  {
    glm::mat4 mat;
    mat[0][0] = _aiMatrix->a1; mat[1][0] = _aiMatrix->a2; mat[2][0] = _aiMatrix->a3; mat[3][0] = _aiMatrix->a4;
    mat[0][1] = _aiMatrix->b1; mat[1][1] = _aiMatrix->b2; mat[2][1] = _aiMatrix->b3; mat[3][1] = _aiMatrix->b4;
    mat[0][2] = _aiMatrix->c1; mat[1][2] = _aiMatrix->c2; mat[2][2] = _aiMatrix->c3; mat[3][2] = _aiMatrix->c4;
    mat[0][3] = _aiMatrix->d1; mat[1][3] = _aiMatrix->d2; mat[2][3] = _aiMatrix->d3; mat[3][3] = _aiMatrix->d4;
    return mat;
  }
}


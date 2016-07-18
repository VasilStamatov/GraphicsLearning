#include "Model.h"
#include "ResourceManager.h"

#include <iostream>

namespace GameEngine
{
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

    ProcessNode(scene->mRootNode, scene);
  }

  void Model::ProcessNode(aiNode* _node, const aiScene* _scene)
  {
    // Process all the node's meshes (if any)
    for (GLuint i = 0; i < _node->mNumMeshes; i++)
    {
      // The node object only contains indices to index the actual objects in the scene. 
      // The scene contains all the data, node is just to keep stuff organized.
      aiMesh* mesh = _scene->mMeshes[_node->mMeshes[i]];
      m_meshes.push_back(ProcessMesh(mesh, _scene));
    }
    // Then do the same for each of its children
    for (GLuint i = 0; i < _node->mNumChildren; i++)
    {
      // Child nodes are actually stored in the node, not in the scene (which makes sense since nodes only contain
      // links and indices, nothing more, so why store that in the scene)
      ProcessNode(_node->mChildren[i], _scene);
    }
  }

  Mesh Model::ProcessMesh(aiMesh* _mesh, const aiScene* _scene)
  {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<GLTexture> textures;

    for (GLuint i = 0; i < _mesh->mNumVertices; i++)
    {
      Vertex vertex;
      vertex.m_position.x = _mesh->mVertices[i].x;
      vertex.m_position.y = _mesh->mVertices[i].y;
      vertex.m_position.z = _mesh->mVertices[i].z;

      vertex.m_normal.x = _mesh->mNormals[i].x;
      vertex.m_normal.y = _mesh->mNormals[i].y;
      vertex.m_normal.z = _mesh->mNormals[i].z;

      if (_mesh->mTextureCoords[0])
      {
        vertex.m_uv.x = _mesh->mTextureCoords[0][i].x;
        vertex.m_uv.y = _mesh->mTextureCoords[0][i].y;
      }
      else
      {
        vertex.m_uv.x = 0.0f;
        vertex.m_uv.y = 0.0f;
      }

      vertex.m_tangents.x = _mesh->mTangents[i].x;
      vertex.m_tangents.y = _mesh->mTangents[i].y;
      vertex.m_tangents.z = _mesh->mTangents[i].z;

      vertices.push_back(vertex);
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

    return Mesh(vertices, indices, textures);
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
      //texture.id = TextureFromFile(str.C_Str(), m_directory); //no SOIL
      texture = ResourceManager::GetTexture(m_directory + '/' + str.C_Str());
      texture.type = _typeName;
      textures.push_back(texture);
    }
    return textures;
  }

}

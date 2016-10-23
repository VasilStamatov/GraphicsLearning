#pragma once
#include "Cache.h"

#include <string>
#include <vector>

namespace GameEngine
{
  //This is a way to access all the resources, such as textures.
  class ResourceManager
  {
  public:
    //gets the texture from the specified filepath
    static GLTexture GetTexture(const std::string& _texturePath, bool _alpha = true);
    //gets the cubemap from the specified 6 faces
    static GLCubemap GetCubemap(const std::string& _directory, const std::string& _posXFilename, const std::string& _negXFilename,
      const std::string& _posYFilename, const std::string& _negYFilename, const std::string& _posZFilename, const std::string& _negZFilename, const std::string& _cubemapName );
    //Loads the skinned model from the filepath to the passed skinned model
    static void GetSkinnedModel(const std::string& _filepath, SkinnedModel* _model);
    //Loads the static model from the filepath to the passed static model
    static void GetStaticModel(const std::string& _filepath, StaticModel* _model);

    static void Clear();
  private:
    //the cache
    static Cache s_cache;
  };
}



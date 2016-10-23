#include "ImageLoader.h"
#include "IOManager.h"
#include "GameEngineErrors.h"
#include <SOIL\SOIL.h>

namespace GameEngine
{
  GLTexture ImageLoader::LoadPNG(const std::string& _filePath, bool _alpha)
  {
    //Create a GLTexture and initialize all its fields to 0
    GLTexture texture = {};

    int width, height;

    unsigned char* image = SOIL_load_image(_filePath.c_str(), &width, &height, 0, _alpha ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);

    //Generate the openGL texture object
    glGenTextures(1, &(texture.id));

    //Bind the texture object
    glBindTexture(GL_TEXTURE_2D, texture.id);

    //Upload the pixels to the texture
    glTexImage2D(GL_TEXTURE_2D, 0, _alpha ? GL_RGBA : GL_RGB, width, height, 0, _alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, image);
    //Generate the mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);

    //Set some texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    //Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);

    texture.width = width;
    texture.height = height;
    texture.filePath = _filePath;

    //Return a copy of the texture data
    return texture;
  }
  GLCubemap ImageLoader::LoadCubemap(const std::string& _directory, const std::string& _posXFilename, const std::string& _negXFilename,
    const std::string& _posYFilename, const std::string& _negYFilename, const std::string& _posZFilename, const std::string& _negZFilename)
  {
    std::array<std::string, 6> faces =
    {
      _posXFilename,
      _negXFilename,
      _posYFilename,
      _negYFilename,
      _posZFilename,
      _negZFilename
    };

    GLCubemap cubeMap = {};

    glGenTextures(1, &(cubeMap.id));

    int width, height;
    unsigned char* image;

    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.id);

    for (GLuint i = 0; i < 6; i++)
    {
      image = SOIL_load_image((_directory + faces.at(i)).c_str(), &width, &height, 0, SOIL_LOAD_RGB);

      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
      SOIL_free_image_data(image);

      GLTexture texture;
      texture.width = width;
      texture.height = height;
      texture.filePath = _directory + faces.at(i);

      cubeMap.textures.at(i) = texture;
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return cubeMap;
  }
}


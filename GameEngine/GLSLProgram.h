#pragma once

#include <string>
#include <GL/glew.h>

namespace GameEngine
{
  //This class handles the compilation, linking, and usage of a GLSL shader program.
  //Reference: http://www.opengl.org/wiki/Shader_Compilation

  class GLSLProgram
  {
  public:
    GLSLProgram();
    ~GLSLProgram();

    //Compiles the vertex and fragment shader from their filepath
    void CompileShaders(const std::string& _vertexShaderFilePath, const std::string& _fragmentShaderFilepath);
    //Compiles the vertex and fragment shader from source (code from cpp, not file)
    void CompileShadersFromSource(const char* _vertexSource, const char* _fragmentSource);
    //links the shaders together
    void LinkShaders();
    //adds an attribute to the shader
    void AddAttribute(const std::string& attributeName);
    //get the uniform location
    GLint GetUniformLocation(const std::string& _uniformName);
    //begin using the shader
    void Use();
    //stop using the shader
    void UnUse();
    //dispose of the shader
    void Dispose();
  private:
    int m_numAttributes;
    //compile a single shader program
    void CompileShader(const char* _source, const std::string& _name, GLuint _id);
    GLuint m_programID;
    GLuint m_vertexShaderID;
    GLuint m_fragmentShaderID;
  };
}


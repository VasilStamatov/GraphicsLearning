#include "GLSLProgram.h"
#include "GameEngineErrors.h"
#include "IOManager.h"

#include <fstream>
#include <vector>

namespace GameEngine
{
  //inoitialize all the variables to 0
  GLSLProgram::GLSLProgram() : m_numAttributes(0), m_programID(0)
  {

  }

  GLSLProgram::~GLSLProgram()
  {
  }

  //Compiles the shaders into a form that your GPU can understand
  void GLSLProgram::CompileShaders(const std::vector<Shader>& _shaders)
  {
    /* vertext and fragment shaders are successfully compiled
    * now it's time to link them together into a program.
    * get a program object */
    m_programID = glCreateProgram();

    m_shaders = _shaders;

    //Iterate until the last shader is Compiled
    for (Shader shader : m_shaders)
    {
      shader.shaderID = glCreateShader(shader.type);

      if (shader.shaderID == 0)
      {
        FatalError(shader.name + " failed to be created!");
      }

      std::string shaderSource;

      IOManager::ReadFileToBuffer(shader.filePath, shaderSource);

      CompileShader(shaderSource.c_str(), shader.name, shader.shaderID);

      glAttachShader(m_programID, shader.shaderID);
    }
  }

  void GLSLProgram::CompileShadersFromSource(const char* _vertexSource, const char* _fragmentSource)
  {
    //Create the vertex shader object, and store its ID

    m_shaders = 
    {
      { GL_VERTEX_SHADER, _vertexSource, "Vertex Shader" },
      { GL_FRAGMENT_SHADER, _fragmentSource, "Fragment Shader" },
    };

    for (Shader shader : m_shaders)
    {
      shader.shaderID = glCreateShader(shader.type);

      if (shader.shaderID == 0)
      {
        FatalError(shader.name + " failed to be created!");
      }

      CompileShader(shader.filePath.c_str(), shader.name, shader.shaderID);

      glAttachShader(m_programID, shader.shaderID);
    }
  }

  void GLSLProgram::LinkShaders()
  {
    //link our program
    glLinkProgram(m_programID);

    //note the different functions here: glGetProgram* instead of glGetShader*
    GLint isLinked = 0;
    glGetProgramiv(m_programID, GL_LINK_STATUS, (int *)&isLinked);
    if (isLinked == GL_FALSE)
    {
      GLint maxLength = 0;
      glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &maxLength);
      //The maxLength includes the null char
      std::vector<char> errorLog(maxLength);
      glGetProgramInfoLog(m_programID, maxLength, &maxLength, &errorLog[0]);
      //we don't need this program anymore
      glDeleteProgram(m_programID);
      //dont leak shaders either

      for (Shader shader : m_shaders)
      {
        glDeleteShader(shader.shaderID);
        shader.shaderID = 0;
      }

      //print the error log and quit
      std::printf("%s\n", &errorLog[0]);
      FatalError("Shaders failed to link!");
    }
    //Always detach shaders after a successful link.
    for (Shader shader : m_shaders)
    {
      glDetachShader(m_programID, shader.shaderID);
      glDeleteShader(shader.shaderID);
    }
  }

  //Adds an attribute to our shader. SHould be called between compiling and linking.
  void GLSLProgram::AddAttribute(const std::string& attributeName)
  {
    glBindAttribLocation(m_programID, m_numAttributes++, attributeName.c_str());
    //Remember: binding attribute's locations isn't needed in the new versions of GLSL where the layout locations are specified,
    //all that's needed are enough glEnablings in the loop in Use()
    //m_numAttributes++;
  }

  GLint GLSLProgram::GetAttribLoc(const std::string& _attributeName)
  {
    GLint location = glGetAttribLocation(m_programID, _attributeName.c_str());
    //error check
    if (location == GL_INVALID_INDEX)
    {
      FatalError("Attribute " + _attributeName + " not found in shader!");
    }
    //return it if successful
    return location;
  }

  GLint GLSLProgram::GetUniformLocation(const std::string& _uniformName)
  {
    //get the uniform location
    GLint location = glGetUniformLocation(m_programID, _uniformName.c_str());
    //error check
    if (location == GL_INVALID_INDEX)
    {
      FatalError("Uniform " + _uniformName + " not found in shader!");
    }
    //return it if successful
    return location;
  }

  GLuint GLSLProgram::GetUniformBlockIndex(const std::string& _uniformBlockName)
  {
    GLuint index = glGetUniformBlockIndex(m_programID, _uniformBlockName.c_str());

    //error check
    if (index == GL_INVALID_INDEX)
    {
      FatalError("Uniform block " + _uniformBlockName + " not found in shader!");
    }
    //return if successful
    return index;
  }

  void GLSLProgram::GetUniformBlockDataSize(GLuint _index, GLint* _params)
  {
    glGetActiveUniformBlockiv(m_programID, _index, GL_UNIFORM_BLOCK_DATA_SIZE, _params);
  }

  void GLSLProgram::BindBufferRange(GLenum _target, GLuint _index, GLuint _buffer, GLintptr _offset, GLsizeiptr _size)
  {
    glBindBufferRange(_target, _index, _buffer, _offset, _size);
    if (glGetError() == GL_INVALID_VALUE)
    {
      FatalError("Unable to bind the buffer, check the redbook for possible causes (p.64)!");
    }
  }

  void GLSLProgram::BlockUniformBinding(GLuint _uniformBlockIndex, GLuint _uniformBlockBinding)
  {
    glUniformBlockBinding(m_programID, _uniformBlockIndex, _uniformBlockBinding);
  }

  void GLSLProgram::GetActiveUniformsIndexValues(GLsizei _numUniforms, GLuint * _uniformIndices, GLenum _pname, GLint * _attribute)
  {
    glGetActiveUniformsiv(m_programID, _numUniforms, _uniformIndices, _pname, _attribute);
  }

  void GLSLProgram::GetUniformIndices(GLsizei _uniformCount, const char ** _uniformNames, GLuint * _uniformIndices)
  {
    glGetUniformIndices(m_programID, _uniformCount, _uniformNames, _uniformIndices);
  }

  GLint GLSLProgram::GetSubroutineUniformLocation(GLenum _shaderType, const std::string& _name)
  {
    GLint location = glGetSubroutineUniformLocation(m_programID, _shaderType, _name.c_str());
    //error check
    if (location == GL_INVALID_INDEX)
    {
      FatalError("Uniform subroutine" + _name + " not found in shader!");
    }
    //return it if successful
    return location;
  }

  GLuint GLSLProgram::GetSubroutineIndex(GLenum _shaderType, const std::string& _name)
  {
    GLuint index = glGetSubroutineIndex(m_programID, _shaderType, _name.c_str());
    //error check
    if (index == GL_INVALID_INDEX)
    {
      FatalError("Subroutine index " + _name + " not found in subroutine uniform!");
    }
    //return if successful
    return index;
  }

  void GLSLProgram::UniformSubroutinesuiv(GLenum _shaderType, GLsizei _numSubrUniforms, const GLuint * _indices)
  {
    glUniformSubroutinesuiv(_shaderType, _numSubrUniforms, _indices);

    if (glGetError() == GL_INVALID_VALUE)
    {
      FatalError("Invalid value generated for index!! Go see p.80 of OGL-R");
    }
  }

  //enable the shader
  void GLSLProgram::Use()
  {
    glUseProgram(m_programID);
    for (int i = 0; i < m_numAttributes; i++)
    {
      glEnableVertexAttribArray(i);
    }
  }

  //disable the shader
  void GLSLProgram::UnUse()
  {
    glUseProgram(0);
    for (int i = 0; i < m_numAttributes; i++)
    {
      glDisableVertexAttribArray(i);
    }
    m_numAttributes = 0; // reset the num attributes
  }

  void GLSLProgram::Dispose()
  {
    //deletes the program ID if there is one (not 0)
    if (m_programID) glDeleteProgram(m_programID);

    //clears the shader vector
    m_shaders.clear();
  }

  //Compiles a single shader file
  void GLSLProgram::CompileShader(const char* _source, const std::string& _name, GLuint _id)
  {
    //tell opengl that we want to use fileContents as the contents of the shader file
    glShaderSource(_id, 1, &_source, nullptr);

    //compile the shader
    glCompileShader(_id);

    //check for errors
    GLint success = 0;
    glGetShaderiv(_id, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE)
    {
      GLint maxLength = 0;
      glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &maxLength);

      //The maxLength includes the NULL character
      std::vector<char> errorLog(maxLength);
      glGetShaderInfoLog(_id, maxLength, &maxLength, &errorLog[0]);

      //Provide the infolog in whatever manor you deem best.
      //Exit with failure.
      glDeleteShader(_id); //Don't leak the shader.

      //Print error log and quit
      std::printf("%s\n", &(errorLog[0]));
      FatalError("Shader " + _name + " failed to compile");
    }
  }
}
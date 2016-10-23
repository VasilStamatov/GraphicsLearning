#include "GLSLProgram.h"
#include "GameEngineErrors.h"
#include "IOManager.h"

#include <fstream>
#include <glm\gtc\type_ptr.hpp>
namespace GameEngine
{
  //inoitialize all the variables to 0
  GLSLProgram::GLSLProgram()
  {

  }

  GLSLProgram::~GLSLProgram()
  {
    if (m_programID > 0)
    {
      Dispose();
    }
  }

  //Compiles the shaders into a form that your GPU can understand
  void GLSLProgram::CompileShaders(const std::string& _vsFilePath, const std::string& _fsFilePath, const std::string& _gsFilePath/* = ""*/)
  {
    std::string vsSource;
    std::string fsSource;
    std::string gsSource;

    IOManager::ReadFileToBuffer(_vsFilePath, vsSource);
    IOManager::ReadFileToBuffer(_fsFilePath, fsSource);
    if (_gsFilePath != "")
    {
      IOManager::ReadFileToBuffer(_gsFilePath, gsSource);
      CompileShadersFromSource(vsSource.c_str(), fsSource.c_str(), gsSource.c_str());
    }
    else
    {
      CompileShadersFromSource(vsSource.c_str(), fsSource.c_str());
    }
  }

  void GLSLProgram::CompileShadersFromSource(const char* _vertexSource, const char* _fragmentSource, const char* _geometrySource /*= nullptr*/)
  {
    //Create the GLSL program ID
    m_programID = glCreateProgram();

    //Create the vertex shader object, and store its ID
    m_vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    if (m_vertexShaderID == 0)
    {
      FatalError("Vertex shader failed to be created!");
    }

    //Create the fragment shader object, and store its ID
    m_fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    if (m_fragmentShaderID == 0) 
    {
      FatalError("Fragment shader failed to be created!");
    }

    //Check if there is a geometry shader
    if (_geometrySource != nullptr)
    {
      //Create the geometry shader object, and store its ID
      m_geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
      if (m_geometryShaderID == 0)
      {
        FatalError("Geometry shader failed to be created!");
      }
    }

    //Compile each shader
    CompileShader(_vertexSource, "Vertex Shader", m_vertexShaderID);
    if (_geometrySource != nullptr)
    {
      CompileShader(_geometrySource, "Geometry Shader", m_geometryShaderID);
    }
    CompileShader(_fragmentSource, "Fragment Shader", m_fragmentShaderID);
    
    LinkShaders();
  }

  void GLSLProgram::LinkShaders()
  {
    //Attach our shaders to our program
    glAttachShader(m_programID, m_vertexShaderID);
    glAttachShader(m_programID, m_fragmentShaderID);
    if (m_geometryShaderID != 0)
    {
      glAttachShader(m_programID, m_geometryShaderID);
    }
    //link our program
    glLinkProgram(m_programID);

    //note the different functions here: glGetProgram* instead of glGetShader*
    GLint isLinked = 0;
    glGetProgramiv(m_programID, GL_LINK_STATUS, (int *)&isLinked);

    if (isLinked == GL_FALSE)
    {
      GLint maxLength = 0;
      //get the size of the string (maxlength)
      glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &maxLength);

      //The maxLength includes the null char
      std::vector<char> errorLog(maxLength);
      glGetProgramInfoLog(m_programID, maxLength, &maxLength, &errorLog[0]);

      //we don't need this program anymore
      glDeleteProgram(m_programID);

      //dont leak shaders either
      //Don't leak shaders either.
      glDeleteShader(m_vertexShaderID);
      glDeleteShader(m_fragmentShaderID);
      glDeleteShader(m_geometryShaderID);

      //print the error log and quit
      std::printf("%s\n", &errorLog[0]);
      FatalError("Shaders failed to link!");
    }
    //Always detach shaders after a successful link.
    glDetachShader(m_programID, m_vertexShaderID);
    glDetachShader(m_programID, m_fragmentShaderID);
    glDetachShader(m_programID, m_geometryShaderID);
    glDeleteShader(m_vertexShaderID);
    glDeleteShader(m_fragmentShaderID);
    glDeleteShader(m_geometryShaderID);
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
  }

  //disable the shader
  void GLSLProgram::UnUse()
  {
    glUseProgram(0);
  }

  void GLSLProgram::Dispose()
  {
    //deletes the program ID if there is one (not 0)
    if (m_programID) glDeleteProgram(m_programID);
    m_programID = 0;
  }

  void GLSLProgram::RegisterAttribute(const std::string& _attrib)
  {
    //m_attribList.insert(std::make_pair(_attrib, GetAttribLoc(_attrib)));
    m_attribList[_attrib] = GetAttribLoc(_attrib);
  }

  void GLSLProgram::RegisterUniform(const std::string& _uniform)
  {
    //m_unifLocationList.insert(std::make_pair(_uniform, GetUniformLocation(_uniform)));
    m_unifLocationList[_uniform] = GetUniformLoc(_uniform);
  }

  AttribLocation GLSLProgram::GetAttribLocation(const std::string& _attrib)
  {
    auto it = m_attribList.find(_attrib);
    if (it != m_attribList.end())
    {
      // Found the location
      return it->second;
    }
    else
    {
      // Didn't find the location
      // Try to register it if it just wasn't registered
      RegisterAttribute(_attrib);
      return m_attribList.at(_attrib);
    }
  }

  UniformLocation GLSLProgram::GetUniformLocation(const std::string& _uniform)
  {
    auto it = m_unifLocationList.find(_uniform);
    if (it != m_unifLocationList.end())
    {
      // Found the location
      return it->second;
    }
    else
    {
      // Didn't find the location
      // Try to see if it just wasn't registered
      RegisterUniform(_uniform);
      return m_unifLocationList.at(_uniform);
    }
  }

  void GLSLProgram::UploadValue(const std::string & _uniformName, const glm::mat4 & _matrix)
  {
    glUniformMatrix4fv(GetUniformLocation(_uniformName), 1, GL_FALSE, glm::value_ptr(_matrix));
  }

  void GLSLProgram::UploadValue(const std::string & _uniformName, const float & _float)
  {
    glUniform1f(GetUniformLocation(_uniformName), _float);
  }

  void GLSLProgram::UploadValue(const std::string & _uniformName, const int & _int)
  {
    glUniform1i(GetUniformLocation(_uniformName), _int);
  }

  void GLSLProgram::UploadValue(const std::string & _uniformName, const glm::vec2 & _vec2)
  {
    glUniform2fv(GetUniformLocation(_uniformName), 1, glm::value_ptr(_vec2));
  }

  void GLSLProgram::UploadValue(const std::string & _uniformName, const glm::vec3 & _vec3)
  {
    glUniform3fv(GetUniformLocation(_uniformName), 1, glm::value_ptr(_vec3));
  }

  void GLSLProgram::UploadValue(const std::string & _uniformName, const glm::vec4 & _vec4)
  {
    glUniform4fv(GetUniformLocation(_uniformName), 1, glm::value_ptr(_vec4));
  }

  void GLSLProgram::UploadValue(const std::string & _uniformName, const int & _slot, const GLTexture& _texture)
  {
    //activate the texture
    glActiveTexture(GL_TEXTURE0 + _slot);
    //Bind it
    glBindTexture(GL_TEXTURE_2D, _texture.id);
    // Now set the sampler to the correct texture unit
    glUniform1i(GetUniformLocation(_uniformName), _slot);
  }

  void GLSLProgram::UploadValue(const std::string & _uniformName, const int & _slot, const GLCubemap & _cubemap)
  {
    //activate the texture
    glActiveTexture(GL_TEXTURE0 + _slot);
    //Bind it
    glBindTexture(GL_TEXTURE_CUBE_MAP, _cubemap.id);
    // Now set the sampler to the correct texture unit
    glUniform1i(GetUniformLocation(_uniformName), _slot);
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

  AttribLocation GLSLProgram::GetAttribLoc(const std::string& _attributeName)
  {
    AttribLocation location = glGetAttribLocation(m_programID, _attributeName.c_str());
    //error check
    if (location == GL_INVALID_INDEX)
    {
      FatalError("Attribute " + _attributeName + " not found in shader!");
    }
    //return it if successful
    return location;
  }

  UniformLocation GLSLProgram::GetUniformLoc(const std::string& _uniformName)
  {
    //get the uniform location
    UniformLocation location = glGetUniformLocation(m_programID, _uniformName.c_str());
    //error check
    if (location == GL_INVALID_INDEX)
    {
      FatalError("Uniform " + _uniformName + " not found in shader!");
    }
    //return it if successful
    return location;
  }
}
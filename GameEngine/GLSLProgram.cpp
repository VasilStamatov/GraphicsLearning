#include "GLSLProgram.h"
#include "GameEngineErrors.h"
#include "IOManager.h"

#include <fstream>
#include <vector>

namespace GameEngine
{
  //inoitialize all the variables to 0
  GLSLProgram::GLSLProgram() : m_numAttributes(0), m_programID(0), m_vertexShaderID(0), m_fragmentShaderID(0)
  {

  }


  GLSLProgram::~GLSLProgram()
  {
  }

  //Compiles the shaders into a form that your GPU can understand
  void GLSLProgram::CompileShaders(const std::string& _vertexShaderFilePath, const std::string& _fragmentShaderFilepath)
  {
    std::string vertSource;
    std::string fragSource;

    //Read the files to the buffers ( 2 strings )
    IOManager::ReadFileToBuffer(_vertexShaderFilePath, vertSource);
    IOManager::ReadFileToBuffer(_fragmentShaderFilepath, fragSource);

    //Compile the shaders by using the strings in their c-string forms
    CompileShadersFromSource(vertSource.c_str(), fragSource.c_str());
  }

  void GLSLProgram::CompileShadersFromSource(const char* _vertexSource, const char* _fragmentSource)
  {
    /* vertext and fragment shaders are successfully compiled
    * now it's time to link them together into a program.
    * get a program object */
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
    //Compile each shader
    CompileShader(_vertexSource, "Vertex Shader", m_vertexShaderID);
    CompileShader(_fragmentSource, "Fragment Shader", m_fragmentShaderID);
  }

  void GLSLProgram::LinkShaders()
  {
    //Attach our shaders to the program
    glAttachShader(m_programID, m_vertexShaderID);
    glAttachShader(m_programID, m_fragmentShaderID);

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
      glDeleteShader(m_fragmentShaderID);
      glDeleteShader(m_vertexShaderID);

      //print the error log and quit
      std::printf("%s\n", &errorLog[0]);
      FatalError("Shaders failed to link!");
    }
    //Always detach shaders after a successful link.
    glDetachShader(m_programID, m_vertexShaderID);
    glDetachShader(m_programID, m_fragmentShaderID);
    glDeleteShader(m_vertexShaderID);
    glDeleteShader(m_fragmentShaderID);
  }

  //Adds an attribute to our shader. SHould be called between compiling and linking.
  void GLSLProgram::AddAttribute(const std::string& attributeName)
  {
    glBindAttribLocation(m_programID, m_numAttributes++, attributeName.c_str());
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
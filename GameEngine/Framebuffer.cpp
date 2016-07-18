#include "Framebuffer.h"

namespace GameEngine
{
  Framebuffer::Framebuffer() : m_fbo(0), m_textureBuffer(0), m_rbo(0), m_vao(0)
  {
  }


  Framebuffer::~Framebuffer()
  {
    Destroy();
  }

  void Framebuffer::Init(int _screenWidth, int _screenHeight)
  {
    m_screenWidth = _screenWidth;
    m_screenHeight = _screenHeight;
    if (m_fbo == 0)
    {
      glGenFramebuffers(1, &m_fbo);
    }
    InitRenderData();
  }

  void Framebuffer::Destroy()
  {
    if (m_fbo != 0)
    {
      glDeleteFramebuffers(1, &m_fbo);
      m_fbo = 0;
    }
    if (m_textureBuffer != 0)
    {
      glDeleteTextures(1, &m_textureBuffer);
      m_textureBuffer = 0;
    }
    if (m_rbo != 0)
    {
      glDeleteRenderbuffers(1, &m_rbo);
      m_rbo = 0;
    }
    if (m_vao != 0)
    {
      glDeleteVertexArrays(1, &m_vao);
      m_vao = 0;
    }
  }

  void Framebuffer::BindFramebuffer(GLenum _target)
  {
    if (m_fbo != 0)
    {
      glBindFramebuffer(_target, m_fbo);
    }
  }

  void Framebuffer::UnbindFramebuffer(GLenum _target)
  {
    glBindFramebuffer(_target, 0);
  }

  void Framebuffer::AttachTexture2D(GLboolean _depth, GLboolean _stencil, GLboolean _multisampled, GLint _samples)
  {
    GLenum attachmentType;
    if (_depth && _stencil)
    {
      attachmentType = GL_DEPTH24_STENCIL8;
    }
    else if (!_depth && !_stencil)
    {
      attachmentType = GL_RGB;
    }
    else if (_depth && !_stencil)
    {
      attachmentType = GL_DEPTH_COMPONENT;
    }
    else if (!_depth && _stencil)
    {
      attachmentType = GL_STENCIL_INDEX;
    }

    glGenTextures(1, &m_textureBuffer);
    glBindTexture(GL_TEXTURE_2D, m_textureBuffer);

    if (_multisampled)
    {
      glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, _samples, attachmentType, m_screenWidth, m_screenHeight, GL_TRUE);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glBindTexture(GL_TEXTURE_2D, 0);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_textureBuffer, 0);
    }
    else
    {
      if (attachmentType == GL_DEPTH24_STENCIL8)
      {
        glTexImage2D(GL_TEXTURE_2D, 0, attachmentType, m_screenWidth, m_screenHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
      }
      else
      {
        glTexImage2D(GL_TEXTURE_2D, 0, attachmentType, m_screenWidth, m_screenHeight, 0, attachmentType, GL_UNSIGNED_BYTE, nullptr);
      }
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glBindTexture(GL_TEXTURE_2D, 0);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureBuffer, 0);
    }

  }

  void Framebuffer::AttachRenderbuffer(GLboolean _depth, GLboolean _stencil, GLboolean _multisampled, GLint _samples)
  {
    GLenum attachmentType;
    if (_depth && _stencil)
    {
      attachmentType = GL_DEPTH24_STENCIL8;
    }
    else if (!_depth && !_stencil)
    {
      attachmentType = GL_RGB;
    }
    else if (_depth && !_stencil)
    {
      attachmentType = GL_DEPTH_COMPONENT;
    }
    else if (!_depth && _stencil)
    {
      attachmentType = GL_STENCIL_INDEX;
    }
    GLuint rbo;
    glGenRenderbuffers(1, &m_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);

    if (_multisampled)
    {
      glRenderbufferStorageMultisample(GL_RENDERBUFFER, _samples, attachmentType, m_screenWidth, m_screenHeight);
    }
    else
    {
      glRenderbufferStorage(GL_RENDERBUFFER, attachmentType, m_screenWidth, m_screenHeight);
    }

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    if (_depth && _stencil)
    {
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
    }
    else if (_depth && !_stencil)
    {
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
    }
    else if (!_depth && _stencil)
    {
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
    }
    else if (!_depth && !_stencil)
    {
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_rbo);
    }
  }

  void Framebuffer::Render()
  {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureBuffer);
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
  }

  bool Framebuffer::CheckFramebufferStatus()
  {
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
      return false;
    }
    return true;
  }

  void Framebuffer::InitRenderData()
  {
    GLuint vbo;
    GLfloat vertices[] = {
      // Pos        // Tex
      -1.0f, -1.0f, 0.0f, 0.0f,
       1.0f, 1.0f,  1.0f, 1.0f,
      -1.0f, 1.0f,  0.0f, 1.0f,

      -1.0f, -1.0f, 0.0f, 0.0f,
      1.0f, -1.0f,  1.0f, 0.0f,
      1.0f, 1.0f,   1.0f, 1.0f
    };  
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices[0]);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }
}
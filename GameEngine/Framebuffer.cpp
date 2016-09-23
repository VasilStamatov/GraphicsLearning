#include "Framebuffer.h"

namespace GameEngine
{
  Framebuffer::Framebuffer()
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
    if (m_fboID == 0)
    {
      glGenFramebuffers(1, &m_fboID);
    }
    m_quad.Init();
  }

  void Framebuffer::Destroy()
  {
    if (m_fboID != 0)
    {
      glDeleteFramebuffers(1, &m_fboID);
      m_fboID = 0;
    }
    if (m_textureBuffer != 0)
    {
      glDeleteTextures(1, &m_textureBuffer);
      m_textureBuffer = 0;
    }
    if (m_rboID != 0)
    {
      glDeleteRenderbuffers(1, &m_rboID);
      m_rboID = 0;
    }
    m_quad.Dispose();
  }

  void Framebuffer::Bind(GLenum _target)
  {
    if (m_fboID != 0)
    {
      glBindFramebuffer(_target, m_fboID);
    }
  }

  void Framebuffer::Unbind(GLenum _target)
  {
    glBindFramebuffer(_target, 0);
  }

  void Framebuffer::Blit(GLbitfield _mask, GLenum _filter)
  {
    glBlitFramebuffer(0, 0, m_screenWidth, m_screenHeight, 0, 0, m_screenWidth, m_screenHeight, _mask, _filter);
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

    if (_multisampled)
    {
      glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_textureBuffer);
      glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, _samples, attachmentType, m_screenWidth, m_screenHeight, GL_TRUE);
      glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_textureBuffer, 0);
    }
    else
    {
      glBindTexture(GL_TEXTURE_2D, m_textureBuffer);
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
    GLuint rbo = 0;
    glGenRenderbuffers(1, &m_rboID);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rboID);

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
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rboID);
    }
    else if (_depth && !_stencil)
    {
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rboID);
    }
    else if (!_depth && _stencil)
    {
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rboID);
    }
    else if (!_depth && !_stencil)
    {
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_rboID);
    }
  }

  void Framebuffer::Render()
  {
    glDisable(GL_DEPTH_TEST);

    m_quad.Render(m_textureBuffer);

    glEnable(GL_DEPTH_TEST);
  }

  bool Framebuffer::CheckFramebufferStatus()
  {
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
      return false;
    }
    return true;
  }
}
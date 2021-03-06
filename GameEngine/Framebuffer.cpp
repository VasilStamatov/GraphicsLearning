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

  void Framebuffer::Init()
  {
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
    if (m_textureBuffer.id != 0)
    {
      m_textureBuffer.Dispose();
    }
    if (m_rboID != 0)
    {
      glDeleteRenderbuffers(1, &m_rboID);
      m_rboID = 0;
    }
    m_quad.Dispose();
  }

  void Framebuffer::Bind(GLenum _target, int _bufferWidth, int _bufferHeight)
  {
    if (m_fboID != 0)
    {
      glBindFramebuffer(_target, m_fboID);
    }
  }

  void Framebuffer::Unbind(GLenum _target, int _bufferWidth, int _bufferHeight)
  {
    glBindFramebuffer(_target, 0);
  }

  void Framebuffer::Blit(GLbitfield _mask, GLenum _filter, int _bufferWidth, int _bufferHeight)
  {
    glBlitFramebuffer(0, 0, _bufferWidth, _bufferHeight, 0, 0, _bufferWidth, _bufferHeight, _mask, _filter);
  }

  void Framebuffer::AttachTexture2D(int _bufferWidth, int _bufferHeight, bool _depth, bool _stencil, bool _multisampled, int _samples/* = 4*/)
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

    glGenTextures(1, &m_textureBuffer.id);

    if (_multisampled)
    {
      glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_textureBuffer.id);
      glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, _samples, attachmentType, _bufferWidth, _bufferHeight, GL_TRUE);
      glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_textureBuffer.id, 0);
    }
    else
    {
      glBindTexture(GL_TEXTURE_2D, m_textureBuffer.id);
      if (attachmentType == GL_DEPTH24_STENCIL8)
      {
        glTexImage2D(GL_TEXTURE_2D, 0, attachmentType, _bufferWidth, _bufferHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
      }
      else
      {
        glTexImage2D(GL_TEXTURE_2D, 0, attachmentType, _bufferWidth, _bufferHeight, 0, attachmentType, GL_UNSIGNED_BYTE, nullptr);
      }
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glBindTexture(GL_TEXTURE_2D, 0);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureBuffer.id, 0);
    }
  }

  void Framebuffer::AttachRenderbuffer(int _bufferWidth, int _bufferHeight, bool _depth, bool _stencil, bool _multisampled, int _samples/* = 4*/)
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
      glRenderbufferStorageMultisample(GL_RENDERBUFFER, _samples, attachmentType, _bufferWidth, _bufferHeight);
    }
    else
    {
      glRenderbufferStorage(GL_RENDERBUFFER, attachmentType, _bufferWidth, _bufferHeight);
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

    m_quad.Render(m_textureBuffer.id);

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
#include "DepthMapFBO.h"

namespace GameEngine
{
  DepthMapFBO::DepthMapFBO()
  {
  }


  DepthMapFBO::~DepthMapFBO()
  {
  }

  void DepthMapFBO::Bind(GLenum _target)
  {
    glViewport(0, 0, m_shadowWidth, m_shadowHeight);
    if (m_fboID != 0)
    {
      glBindFramebuffer(_target, m_fboID);
    }
    glClear(GL_DEPTH_BUFFER_BIT);
  }

  void DepthMapFBO::Unbind(GLenum _target)
  {
    glBindFramebuffer(_target, 0);
    glViewport(0, 0, m_screenWidth, m_screenHeight);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  }

  void DepthMapFBO::AttachTexture2D(GLboolean _depth, GLboolean _stencil, GLboolean _multisampled, GLint _samples)
  {
    glGenTextures(1, &m_textureBuffer.id);

    if (_multisampled)
    {
      glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_textureBuffer.id);
      glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, _samples, GL_DEPTH_COMPONENT, m_shadowWidth, m_shadowHeight, GL_TRUE);
      glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, m_textureBuffer.id, 0);
    }
    else
    {
      glBindTexture(GL_TEXTURE_2D, m_textureBuffer.id);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_shadowWidth, m_shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
      GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
      glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
      glBindTexture(GL_TEXTURE_2D, 0);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_textureBuffer.id, 0);
    }
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
  }
  void DepthMapFBO::AttachDepthCubemap()
  {
    glGenTextures(1, &m_depthCubemap.id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubemap.id);
    for (size_t i = 0; i < 6; i++)
    {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
        m_shadowWidth, m_shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthCubemap.id, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
  }
  void DepthMapFBO::AttachRenderbuffer(GLboolean _depth, GLboolean _stencil, GLboolean _multisampled, GLint _samples)
  {
  }
}
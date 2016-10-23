#pragma once
#include "Framebuffer.h"

namespace GameEngine
{
  class DepthMapFBO : public Framebuffer
  {
  public:
    DepthMapFBO();
    ~DepthMapFBO();

    /** Binds this fbo to be used.*/
    void Bind(GLenum _target) override;

    /** Unbinds the fbo and the default framebuffer is used until another one is bound*/
    void Unbind(GLenum _target) override;

    /** Attaches a texture to the framebuffer which can be a RGB/depth/stencil/depth + stencil buffer.*/
    void AttachTexture2D(GLboolean _depth = true, GLboolean _stencil = false, GLboolean _multisampled = false, GLint _samples = 4) override;

    void AttachDepthCubemap();
    /** Attaches a renderbuffer to the framebuffer which can be a RGB/depth/stencil/depth + stencil buffer.
    The difference betweehn a renderbuffer and a 2D texture attachment is that the renderbuffer cannot be sampled/read.
    Therefore, this can be used when you don't want to sample from a certain part of the framebuffer*/
    void AttachRenderbuffer(GLboolean _depth, GLboolean _stencil = false, GLboolean _multisampled = false, GLint _samples = 4) override;

    GLuint GetDepthMapWidth() const noexcept { return m_shadowWidth; }
    GLuint GetDepthMapHeight() const noexcept { return m_shadowHeight; }
    GLCubemap GetCubemap() const noexcept { return m_depthCubemap; }
  private:
    GLuint m_shadowWidth{ 1024 };
    GLuint m_shadowHeight{ 1024 };
    GLCubemap m_depthCubemap;
  };

}


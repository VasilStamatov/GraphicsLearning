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
    void Bind(GLenum _target, int _bufferWidth = 1024, int _bufferHeight = 1024) override;

    /** Unbinds the fbo and the default framebuffer is used until another one is bound*/
    void Unbind(GLenum _target, int _bufferWidth, int _bufferHeight) override;

    /** Attaches a texture to the framebuffer which can be a RGB/depth/stencil/depth + stencil buffer.*/
    void AttachTexture2D(int _bufferWidth, int _bufferHeight, bool _depth = true, bool _stencil = false, bool _multisampled = false, int _samples = 4) override;

    void AttachDepthCubemap(int _bufferWidth = 1024, int _bufferHeight = 1024);

    GLCubemap GetCubemap() const noexcept { return m_depthCubemap; }
  private:
    GLCubemap m_depthCubemap;
  };

}


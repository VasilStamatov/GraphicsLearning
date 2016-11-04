#pragma once
#include <GL\glew.h>
#include "ScreenQuad.h"
#include "GLTexture.h"

namespace GameEngine
{
  class Framebuffer
  {
  public:
    Framebuffer();
    virtual ~Framebuffer();

    /** Sets the SW/SH values and creates an ID for the fbo.*/
    void Init();

    /** Deletes the fbo.*/
    void Destroy();

    /** Binds this fbo to be used.*/
    virtual void Bind(GLenum _target, int _bufferWidth, int _bufferHeight);

    /** Unbinds the fbo and the default framebuffer is used until another one is bound*/
    virtual void Unbind(GLenum _target, int _bufferWidth, int _bufferHeight);

    void Blit(GLbitfield _mask, GLenum _filter, int _bufferWidth, int _bufferHeight);

    /** Attaches a texture to the framebuffer which can be a RGB/depth/stencil/depth + stencil buffer.*/
    virtual void AttachTexture2D(int _bufferWidth, int _bufferHeight, bool _depth, bool _stencil, bool _multisampled,  int _samples = 4);

    /** Attaches a renderbuffer to the framebuffer which can be a RGB/depth/stencil/depth + stencil buffer.
        The difference betweehn a renderbuffer and a 2D texture attachment is that the renderbuffer cannot be sampled/read.
         Therefore, this can be used when you don't want to sample from a certain part of the framebuffer*/
    virtual void AttachRenderbuffer(int _bufferWidth, int _bufferHeight, bool _depth, bool _stencil, bool _multisampled, int _samples = 4);

    void Render();
     
    /** Checks if the framebuffer is valid
      \return true if the framebuffer status is complete
      \return false if the renderbuffer status isn't complete */
    bool CheckFramebufferStatus();

    /** Accessors */
    GLuint GetFBO()              const noexcept { return m_fboID; }
    GLTexture GetTextureBuffer() const noexcept { return m_textureBuffer; }
  protected:
    GLuint m_fboID{ 0 }, m_rboID{ 0 };
    GLTexture m_textureBuffer;
    ScreenQuad m_quad;
  };
}



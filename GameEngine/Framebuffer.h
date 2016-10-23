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
    void Init(int _screenWidth, int _screenHeight);

    /** Deletes the fbo.*/
    void Destroy();

    /** Binds this fbo to be used.*/
    virtual void Bind(GLenum _target);

    /** Unbinds the fbo and the default framebuffer is used until another one is bound*/
    virtual void Unbind(GLenum _target);

    void Blit(GLbitfield _mask, GLenum _filter);

    /** Attaches a texture to the framebuffer which can be a RGB/depth/stencil/depth + stencil buffer.*/
    virtual void AttachTexture2D(GLboolean _depth, GLboolean _stencil, GLboolean _multisampled, GLint _samples = 4);

    /** Attaches a renderbuffer to the framebuffer which can be a RGB/depth/stencil/depth + stencil buffer.
        The difference betweehn a renderbuffer and a 2D texture attachment is that the renderbuffer cannot be sampled/read.
         Therefore, this can be used when you don't want to sample from a certain part of the framebuffer*/
    virtual void AttachRenderbuffer(GLboolean _depth, GLboolean _stencil, GLboolean _multisampled, GLint _samples = 4);

    void Render();
     
    /** Checks if the framebuffer is valid
      \return true if the framebuffer status is complete
      \return false if the renderbuffer status isn't complete */
    bool CheckFramebufferStatus();

    /** Accessors */
    GLuint GetFBO()              const noexcept { return m_fboID; }
    GLTexture GetTextureBuffer() const noexcept { return m_textureBuffer; }
    int GetBufferWidth()         const noexcept { return m_screenWidth; }
    int GetBufferHeight()        const noexcept { return m_screenHeight; }
  protected:
    int m_screenWidth{ 500 }, m_screenHeight{ 500 };
    GLuint m_fboID{ 0 }, m_rboID{ 0 };
    GLTexture m_textureBuffer;
    ScreenQuad m_quad;
  };
}



#include "Window.h"
#include "GameEngineErrors.h"

namespace GameEngine
{
  Window::Window()
  {
  }


  Window::~Window()
  {
  }

  int Window::Create(std::string _windowName, int _screenWidth, int _screenHeight, unsigned int _currentFlags)
  {
    Uint32 flags = SDL_WINDOW_OPENGL;
    m_screenWidth = _screenWidth;
    m_screenHeight = _screenHeight;
    //if any of the bitwise operations aren't 0, then it's true
    if (_currentFlags & INVISIBLE)
    {
      flags |= SDL_WINDOW_HIDDEN;
    }
    if (_currentFlags & FULLSCREEN)
    {
      flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
    if (_currentFlags & BORDERLESS)
    {
      flags |= SDL_WINDOW_BORDERLESS;
    }
    //Open an SDL window
    m_sdlWindow = SDL_CreateWindow(_windowName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _screenWidth, _screenHeight, flags);
    if (m_sdlWindow == nullptr)
    {
      FatalError("SDL Window could not be created! ");
    }
    //Set up our OpenGL context
    SDL_GLContext glContext = SDL_GL_CreateContext(m_sdlWindow);
    if (glContext == nullptr)
    {
      FatalError("SDL_GL context could not be created!");
    }
    //Set up glew (optional but recommended)
    GLenum error = glewInit();
    if (error != GLEW_OK)
    {
      FatalError("Could not initialize Glew!");
    }

    //check the openGL version
    printf("*** OpenGL Version: %s ***\n", glGetString(GL_VERSION));

    //Set the background color to blue
    glClearColor(0.0f, 0.0f, 1.0f, 1.0);

    //set vsync
    SDL_GL_SetSwapInterval(0);
      
    //Enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return 0;
  }
  void Window::SwapBuffer()
  {
    SDL_GL_SwapWindow(m_sdlWindow);
  }
}


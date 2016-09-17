#include "Window.h"
#include "GameEngineErrors.h"
#include <iostream>

namespace GameEngine
{
  Window::Window()
  {
  }


  Window::~Window()
  {
  }

  int Window::Create(const std::string& _windowName, int _screenWidth, int _screenHeight, unsigned int _currentFlags)
  {
    Uint32 flags = SDL_WINDOW_OPENGL;
    m_screenWidth = _screenWidth;
    m_screenHeight = _screenHeight;
    //if any of the bitwise operations aren't 0, then it's true
    if (_currentFlags & WindowFlags::INVISIBLE)
    {
      flags |= SDL_WINDOW_HIDDEN;
    }
    if (_currentFlags & WindowFlags::FULLSCREEN)
    {
      flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
    if (_currentFlags & WindowFlags::BORDERLESS)
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
    m_glContext = SDL_GL_CreateContext(m_sdlWindow);
    if (m_glContext == nullptr)
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
    std::cout << "INFO: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;

    std::cout << "INFO: OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "INFO: OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "INFO: OpenGL Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    //Set the background color to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0);

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
  void Window::ChangeWindowType(const WindowFlags& _flags)
  {
    Uint32 screenType;

    //if any of the bitwise operations aren't 0, then it's true
    if (_flags == WindowFlags::FULLSCREEN)
    {
      screenType == SDL_WINDOW_FULLSCREEN;
    }
    else if (_flags == WindowFlags::BORDERLESS)
    {
      screenType = SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
    else if (_flags == WindowFlags::WINDOWED)
    {
      screenType = 0;
    }
    SDL_SetWindowFullscreen(m_sdlWindow, screenType);
    SDL_GetWindowSize(m_sdlWindow, &m_screenWidth, &m_screenHeight);

  }
}


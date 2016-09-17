#pragma once

#include <SDL\SDL.h>
#include <GL\glew.h>
#include <string>

namespace GameEngine
{
  enum WindowFlags { INVISIBLE = 0x1, FULLSCREEN = 0x2, BORDERLESS = 0x4, WINDOWED = 0x8 };

  class Window
  {
  public:
    Window();
    ~Window();
    //create the window
    int Create(const std::string& _windowName, int _screenWidth, int _screenHeight, unsigned int _currentFlags);
    //swap the buffer
    void SwapBuffer();
    //
    void ChangeWindowType(const WindowFlags& _flags);

    //Getters
    int GetScreenWidth() { return m_screenWidth; };
    int GetScreenHeight() { return m_screenHeight; };
    SDL_GLContext GetGLContext() { return m_glContext; }
    SDL_Window* GetWindow() { return m_sdlWindow; }
  private:
    SDL_Window* m_sdlWindow;
    SDL_GLContext m_glContext;
    int m_screenWidth, m_screenHeight;
  };
}



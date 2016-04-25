#pragma once

#include <SDL\SDL.h>
#include <GL\glew.h>
#include <string>

namespace GameEngine
{
  enum WindowFlags { INVISIBLE = 0x1, FULLSCREEN = 0x2, BORDERLESS = 0x4 };

  class Window
  {
  public:
    Window();
    ~Window();
    //create the window
    int Create(std::string _windowName, int _screenWidth, int _screenHeight, unsigned int _currentFlags);
    //swap the buffer
    void SwapBuffer();

    //Getters
    int GetScreenWidth() { return m_screenWidth; };
    int GetScreenHeight() { return m_screenHeight; };

  private:
    SDL_Window* m_sdlWindow;
    int m_screenWidth, m_screenHeight;
  };
}



#pragma once

#include <SDL\SDL.h>
#include <GL\glew.h>
#include <string>
#include <memory>
namespace GameEngine
{
  enum WindowCreationFlags : unsigned int
		{
				NONE = 0,
				INVISIBLE = 1,								  //window is not visible
				FULLSCREEN = 2,									//fullscreen window
				FULLSCREEN_DESKTOP = 4, //fullscreen window at the current desktop resolution
				BORDERLESS = 8,							  //no window decoration
				RESIZABLE = 16,							  //window can be resized
				MINIMIZED = 32,							  //window is minimized
				MAXIMIZED = 64,							  //window is maximized
				INPUT_GRABBED = 128,				//window has grabbed input focus
		};

		enum class FullscreenState : unsigned int
		{
				FULLSCREEN = 1,
				BORDERLESS = 2,
				WINDOWED = 4
		};

  class Window
  {
  public:
    Window();
    ~Window();
    //create the window
    int Create(const std::string& _windowName, int _screenWidth, int _screenHeight, const WindowCreationFlags& _windowFlags);
				//
				void Close();
				//
				void HandleEvent(SDL_Event& _event);
    //swap the buffer
    void SwapBuffer();
    //
    void ChangeFullscreenState(const FullscreenState& _flags);

				//Setters
				void ResizeHandled()																								noexcept { m_wasResized = false; }

    //Getters
				float GetAspectRatio()																const noexcept { return (float)m_screenWidth / (float)m_screenHeight; }
				SDL_Window* GetWindow()															const noexcept { return m_sdlWindow;	 	 }
				int GetScreenWidth()																  const noexcept { return m_screenWidth;  }
    int GetScreenHeight()																	const noexcept { return m_screenHeight; }
				bool HasMouseFocus()																		const noexcept { return m_mouseFocus;   }
				bool HasKeyboardFocus()															const noexcept { return m_keyboardFocus;}
				bool IsMinimized()																				const noexcept { return m_minimized;    }
				bool WasResized()									 											const noexcept { return m_wasResized;   }
		private:
				void InitGL();

		private:
    SDL_Window* m_sdlWindow = nullptr;

    SDL_GLContext m_glContext;
				int m_screenWidth { 0 };
				int m_screenHeight{ 0 };

				bool m_mouseFocus			{ false };
				bool m_keyboardFocus{ false };
				bool m_fullscreen			{ false };
				bool m_minimized				{ false };
				bool m_wasResized			{ false };
  };
}



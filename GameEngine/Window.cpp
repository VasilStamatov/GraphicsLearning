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

		int Window::Create(const std::string& _windowName, int _screenWidth, int _screenHeight, const WindowCreationFlags& _windowFlags)
		{
				Uint32 flags = SDL_WINDOW_OPENGL; //base needed flag
				m_screenWidth = _screenWidth;
				m_screenHeight = _screenHeight;
				//if any of the bitwise operations aren't 0, then it's true
				if (!_windowFlags == WindowCreationFlags::NONE)
				{
						if (_windowFlags & WindowCreationFlags::INVISIBLE)
						{
								flags |= SDL_WINDOW_HIDDEN;
						}
						if (_windowFlags & WindowCreationFlags::FULLSCREEN)
						{
								flags |= SDL_WINDOW_FULLSCREEN;
						}
						if (_windowFlags & WindowCreationFlags::BORDERLESS)
						{
								flags |= SDL_WINDOW_BORDERLESS;
						}
						if (_windowFlags & WindowCreationFlags::FULLSCREEN_DESKTOP)
						{
								flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
						}
						if (_windowFlags & WindowCreationFlags::INPUT_GRABBED)
						{
								flags |= SDL_WINDOW_INPUT_GRABBED;
						}
						if (_windowFlags & WindowCreationFlags::MAXIMIZED)
						{
								flags |= SDL_WINDOW_MAXIMIZED;
						}
						if (_windowFlags & WindowCreationFlags::MINIMIZED)
						{
								flags |= SDL_WINDOW_MINIMIZED;
						}
						if (_windowFlags & WindowCreationFlags::RESIZABLE)
						{
								flags |= SDL_WINDOW_RESIZABLE;
						}
				}
				//Create an SDL window
				m_sdlWindow = SDL_CreateWindow(_windowName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _screenWidth, _screenHeight, flags);

				if (m_sdlWindow == nullptr)
				{
						FatalError("SDL Window could not be created! ");
				}

				m_mouseFocus = true;
				m_keyboardFocus = true;

				InitGL();

				return 0; //Success!
		}
		void Window::Close()
		{
				SDL_GL_DeleteContext(m_glContext);
				SDL_DestroyWindow(m_sdlWindow);
				SDL_Quit();
		}
		void Window::HandleEvent(SDL_Event & _event)
		{
				switch (_event.window.event)
				{
						case SDL_WINDOWEVENT_SHOWN:
						{
								//window is shown
								std::cout << "SDL_WINDOWEVENT_SHOWN" << std::endl;
								break;
						}
						case SDL_WINDOWEVENT_HIDDEN:
						{
								//window is hidden
								std::cout << "SDL_WINDOWEVENT_HIDDEN" << std::endl;
								break;
						}
						case SDL_WINDOWEVENT_EXPOSED:
						{
								//the window is visible again, make it render !
								std::cout << "SDL_WINDOWEVENT_EXPOSED" << std::endl;
								break;
						}
						case SDL_WINDOWEVENT_MOVED:
						{
								//window has been moved
								std::cout << "SDL_WINDOWEVENT_MOVED" << std::endl;
								break;
						}
						case SDL_WINDOWEVENT_RESIZED:
						{
								//window has been resized
								m_screenWidth = _event.window.data1;
								m_screenHeight = _event.window.data2;
								glViewport(0, 0, m_screenWidth, m_screenHeight);
								m_wasResized = true;
								std::cout << "SDL_WINDOWEVENT_RESIZED" << std::endl;
								printf("width: %d, height %d \n", m_screenWidth, m_screenHeight);
								break;
						}
						case SDL_WINDOWEVENT_SIZE_CHANGED:
						{
								//window's size has changed
								m_screenWidth = _event.window.data1;
								m_screenHeight = _event.window.data2;
								glViewport(0, 0, m_screenWidth, m_screenHeight);
								m_wasResized = true;
								std::cout << "SDL_WINDOWEVENT_SIZE_CHANGED" << std::endl;
								printf("width: %d, height %d \n", m_screenWidth, m_screenHeight);
								break;
						}
						case SDL_WINDOWEVENT_MINIMIZED:
						{
								//window was minimized
								m_minimized = true;
								std::cout << "SDL_WINDOWEVENT_MINIMIZED" << std::endl;
								break;
						}
						case SDL_WINDOWEVENT_MAXIMIZED:
						{
								//window was maximized
								m_minimized = false;
								std::cout << "SDL_WINDOWEVENT_MAXIMIZED" << std::endl;
								break;
						}
						case SDL_WINDOWEVENT_RESTORED:
						{
								//window has been restored
								m_minimized = false;
								std::cout << "SDL_WINDOWEVENT_RESTORED" << std::endl;
								break;
						}
						case SDL_WINDOWEVENT_ENTER:
						{
								//mouse entered the window
								m_mouseFocus = true;
								std::cout << "SDL_WINDOWEVENT_ENTER" << std::endl;
								break;
						}
						case SDL_WINDOWEVENT_LEAVE:
						{
								//mouse left the window
								m_mouseFocus = false;
								std::cout << "SDL_WINDOWEVENT_LEAVE" << std::endl;
								break;
						}
						case SDL_WINDOWEVENT_FOCUS_GAINED:
						{
								//window gained keyboard access/focus
								m_keyboardFocus = true;
								std::cout << "SDL_WINDOWEVENT_FOCUS_GAINED" << std::endl;
								break;
						}
						case SDL_WINDOWEVENT_FOCUS_LOST:
						{
								//window lost keyboard access/focus
								m_keyboardFocus = false;
								std::cout << "SDL_WINDOWEVENT_FOCUS_LOST" << std::endl;
								break;
						}
						case SDL_WINDOWEVENT_CLOSE:
						{
								//window has been closed
								std::cout << "SDL_WINDOWEVENT_CLOSE" << std::endl;
								break;
						}
#if SDL_VERSION_ATLEAST(2, 0, 5)
						case SDL_WINDOWEVENT_TAKE_FOCUS:
						{
								//window has been offered a focus
								std::cout << "SDL_WINDOWEVENT_TAKE_FOCUS" << std::endl;
								break;
						}
						case SDL_WINDOWEVENT_HIT_TEST:
						{
								//window has a special hit test
								std::cout << "SDL_WINDOWEVENT_HIT_TEST" << std::endl;
								break;
						}
#endif
						default:
						{
								//IMPOSSIBLE!
								std::cout << "default" << std::endl;
								break;
						}
				}
		}
		void Window::SwapBuffer()
		{
				SDL_GL_SwapWindow(m_sdlWindow);
		}
		void Window::ChangeFullscreenState(const FullscreenState& _flags)
		{
				Uint32 screenType;

				//if any of the bitwise operations aren't 0, then it's true
				if (_flags == FullscreenState::FULLSCREEN)
				{
						screenType = SDL_WINDOW_FULLSCREEN_DESKTOP;
						m_fullscreen = true;
				}
				else if (_flags == FullscreenState::BORDERLESS)
				{
						screenType = SDL_WINDOW_FULLSCREEN_DESKTOP;
						m_fullscreen = false;
				}
				else if (_flags == FullscreenState::WINDOWED)
				{
						screenType = 0;
						m_fullscreen = false;
				}

				SDL_SetWindowFullscreen(m_sdlWindow, screenType);
				SDL_GetWindowSize(m_sdlWindow, &m_screenWidth, &m_screenHeight);
				glViewport(0, 0, m_screenWidth, m_screenHeight);
				m_wasResized = true;
		}

		void Window::InitGL()
		{
				//Set up our OpenGL context
				m_glContext = SDL_GL_CreateContext(m_sdlWindow);
				if (m_glContext == nullptr)
				{
						FatalError("SDL_GL context could not be created!");
				}

				//Set up glew
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

				//Enable face culling
				glEnable(GL_CULL_FACE);
				glFrontFace(GL_CCW);
				glCullFace(GL_BACK);

				//Enable depth testing
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LESS);

				//Enable multisampling
				glEnable(GL_MULTISAMPLE);

				//Enable gamma correction
				//glEnable(GL_FRAMEBUFFER_SRGB);

				//Enable stencil testing
				//glEnable(GL_STENCIL_TEST);
				//glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
				//glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
				//glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_REPLACE);
				//glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_KEEP);

				// Enable depth clamping
				// glEnable(GL_DEPTH_CLAMP);
		}
}


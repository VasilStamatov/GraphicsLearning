#include "IMainGame.h"
#include "Timing.h"
#include "ScreenList.h"
#include "IGameScreen.h"

namespace GameEngine
{
		IMainGame::IMainGame()
		{
				m_screenList = std::make_unique<ScreenList>(this);
		}
		IMainGame::~IMainGame()
		{
				//empty
		}

		void IMainGame::Run()
		{
				//Initialize the game and error check if it managed to do it successfully
				if (!Init()) return;
				//set the max fps
				FpsLimiter limiter;
				limiter.SetMaxFPS(60.0f);

				// Game loop
				m_isRunning = true;
				while (m_isRunning)
				{
						//begins the frame
						limiter.BeginFrame();

						//updates the key map
						inputManager.Update();
						// Call the custom update and draw method
						if (!m_paused)
						{
								Update();
						}
						else
						{
								SDL_Event evnt;
								while (SDL_PollEvent(&evnt))
								{
										OnSDLEvent(evnt);
								}
								if (inputManager.IsKeyPressed(SDLK_F9))
								{
										m_paused = !m_paused;
								}
								if (inputManager.IsKeyPressed(SDLK_ESCAPE))
								{
										ExitGame();
								}
						}
						if (m_isRunning && !m_window.IsMinimized())
						{
								Draw();
								//ends the fps limiter at the end of the frame
								m_fps = limiter.End();
								//swaps the buffer
								m_window.SwapBuffer();
						}
						m_deltaTime = limiter.GetCurrentDT();
				}

				m_window.Close();
		}

		void IMainGame::ExitGame()
		{
				//call the custom OnExit function for the current screen
				m_currentScreen.lock()->OnExit();
				if (m_screenList)
				{
						//if the screenlist isn't 0 (NONE) call the custom destroy function and reselt the screenlist vector
						m_screenList->Destroy();
						//deallcate the memory with this unique pointer function
						m_screenList.reset();
				}
				//exit the game loop
				m_isRunning = false;
		}

		void IMainGame::OnSDLEvent(SDL_Event& _evnt)
		{
				//handles SDL events
				switch (_evnt.type)
				{
						case SDL_QUIT:
						{
								ExitGame();
								break;
						}
						case SDL_MOUSEMOTION:
						{
								inputManager.SetMouseCoords((float)_evnt.motion.x, (float)_evnt.motion.y);
								inputManager.SetRelativeMouseMotion((float)_evnt.motion.xrel, (float)_evnt.motion.yrel);
								break;
						}
						case SDL_KEYDOWN:
						{
								inputManager.PressKey(_evnt.key.keysym.sym);
								break;
						}
						case SDL_KEYUP:
						{
								inputManager.ReleaseKey(_evnt.key.keysym.sym);
								break;
						}
						case SDL_MOUSEBUTTONDOWN:
						{
								inputManager.PressKey(_evnt.button.button);
								break;
						}
						case SDL_MOUSEBUTTONUP:
						{
								inputManager.ReleaseKey(_evnt.button.button);
								break;
						}
						case SDL_MOUSEWHEEL:
						{
								inputManager.SetMouseWheel(_evnt.wheel.x, _evnt.wheel.y);
								break;
						}
						case SDL_WINDOWEVENT:
						{
								m_window.HandleEvent(_evnt);
						}
				}
		}

		bool IMainGame::Init()
		{
				//Initialize SDL and sets pre-window properties
				GameEngine::Init();

				//Call on init at the start of the game
				OnInit();

				/*try ti initialize the systems, and if it fails then InitSystems() returns false,
						so invert it and in the if-statement return false to show that the initialization failed*/
				if (!InitSystems()) return false;

				//add the screens
				AddScreens();
				//Get the first default screen ( main menu screen ) and do it's OnEntry custom function and set the game state to running
				m_currentScreen = m_screenList->GetCurrent();
				m_currentScreen.lock()->OnEntry();
				m_currentScreen.lock()->SetRunning();

				return true;
		}

		bool IMainGame::InitSystems()
		{
				//create the Window
				m_window.Create(m_gameName, m_screenWidth, m_screenHeight, m_windowFlags);
				return true;
		}


		void IMainGame::Update()
		{
				//update gets called every frame
				//check if the current screen isn't NONE (0)
				if (m_currentScreen.lock())
				{
						switch (m_currentScreen.lock()->GetState())
						{
						case ScreenState::RUNNING:
								//if the screenstate is running just do the custom update function
								m_currentScreen.lock()->Update();
								break;
						case ScreenState::CHANGE_NEXT:
								//if CHange Next is called, then call the custom OnExit function and Move to the next screen
								m_currentScreen.lock()->OnExit();
								m_currentScreen = m_screenList->MoveNext();
								//check if the next screen isn't NONE by any chance
								if (m_currentScreen.lock())
								{
										//if it's a correct screen set the state to running and do the custom OnEntry function
										m_currentScreen.lock()->SetRunning();
										m_currentScreen.lock()->OnEntry();
								}
								break;
						case ScreenState::CHANGE_PREVIOUS:
								//if CHange Previous is called, then call the custom OnExit function and Move to the previous screen
								m_currentScreen.lock()->OnExit();
								m_currentScreen = m_screenList->MovePrevious();
								//check if the next screen isn't NONE by any chance
								if (m_currentScreen.lock())
								{
										//if it's a correct screen set the state to running and do the custom OnEntry function
										m_currentScreen.lock()->SetRunning();
										m_currentScreen.lock()->OnEntry();
								}
								break;
						case ScreenState::EXIT_APPLICATION:
								//exits the game
								ExitGame();
								break;
						default:
								break;
						}
				}
				else
				{
						//if the current screen state is NONE, then exit the game
						ExitGame();
				}
		}

		void IMainGame::Draw()
		{
				if (m_currentScreen.lock() && m_currentScreen.lock()->GetState() == ScreenState::RUNNING)
				{
						//if the current screen isn't NONE and its state is running, then call its custom draw function
						m_currentScreen.lock()->Draw();
				}
		}
}


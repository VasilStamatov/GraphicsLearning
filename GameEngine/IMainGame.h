#pragma once

#include "GameEngine.h"
#include "Window.h"
#include "InputManager.h"
#include <memory>

namespace GameEngine
{
  class ScreenList;
  class IGameScreen;

  class IMainGame
  {
  public:
    IMainGame();
    virtual ~IMainGame();

    //runs and initializes the game
    void Run();
    //exits the game
    void ExitGame();
    //called on entry
    virtual void OnInit() = 0;
    //used when adding screens
    virtual void AddScreens() = 0;
    //called when exiting
    virtual void OnExit() = 0;
    //handle SDL events
    void OnSDLEvent(SDL_Event& _evnt);

    const float GetFPS() const noexcept { return m_fps; }
    const float GetDT() const noexcept { return m_deltaTime; }

    InputManager inputManager;

  protected:
    //custom update function
    virtual void Update();
    //custom render function
    virtual void Draw();
    //init the game
    bool Init();
    //create the window
    bool InitSystems();
    //list of screens
    std::unique_ptr<ScreenList> m_screenList{ nullptr };
    //current screen
    IGameScreen* m_currentScreen{ nullptr };
    //The renderer this game will use

    bool m_isRunning{ false };
    bool m_paused{ false };

    float m_deltaTime{ 1.0f };
    float m_fps{ 0.0f };
    //the window
    Window m_window;
    std::string m_gameName{ "Default" };
    int m_screenHeight{ 500 };
    int m_screenWidth{ 500 };
    unsigned int m_currentFlags{ 0 };
  };
}



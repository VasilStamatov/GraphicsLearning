#include "App.h"
#include <GameEngine\ScreenList.h>


App::App()
{
}


App::~App()
{
}

// called on initialization
void App::OnInit()
{
  //set up the variables for the window creation
  m_gameName = "Platformer Maker";
  m_screenWidth = 1080;
  m_screenHeight = 768;
  m_currentFlags = 0;
}
// called when exiting
void App::OnExit()
{
  //empty
}
// used to add screens
void App::AddScreens()
{
  m_mainMenuScreen = std::make_unique<MainMenuScreen>(&m_window);
  m_gamePlayScreen = std::make_unique<GameplayScreen>(&m_window);
  m_editorScreen   = std::make_unique<EditorScreen>(&m_window);
  m_gameOverScreen = std::make_unique<GameOverScreen>(&m_window);
  m_levelClearedScreen = std::make_unique<LevelClearedScreen>(&m_window);

  m_screenList->AddScreen(m_mainMenuScreen.get());
  m_screenList->AddScreen(m_gamePlayScreen.get());
  m_screenList->AddScreen(m_editorScreen.get());
  m_screenList->AddScreen(m_gameOverScreen.get());
  m_screenList->AddScreen(m_levelClearedScreen.get());

  m_screenList->SetScreen(m_mainMenuScreen->GetScreenIndex());
}

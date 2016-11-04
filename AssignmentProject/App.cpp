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
  m_windowFlags = GameEngine::WindowCreationFlags::NONE;
}
// called when exiting
void App::OnExit()
{
  //empty
}
// used to add screens
void App::AddScreens()
{
  m_mainMenuScreen = std::make_shared<MainMenuScreen>(&m_window);
  m_gamePlayScreen = std::make_shared<GameplayScreen>(&m_window);
  m_editorScreen   = std::make_shared<EditorScreen>(&m_window);
  m_gameOverScreen = std::make_shared<GameOverScreen>(&m_window);
  m_levelClearedScreen = std::make_shared<LevelClearedScreen>(&m_window);

  m_screenList->SetScreen(m_mainMenuScreen->GetScreenIndex());

  m_screenList->AddScreen(std::move(m_mainMenuScreen));
  m_screenList->AddScreen(std::move(m_gamePlayScreen));
  m_screenList->AddScreen(std::move(m_editorScreen));
  m_screenList->AddScreen(std::move(m_gameOverScreen));
  m_screenList->AddScreen(std::move(m_levelClearedScreen));
}

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
  m_gameName = "3D Playground";
  m_screenWidth = 640;
  m_screenHeight = 480;
  m_currentFlags = 0;
  
}
// called when exiting
void App::OnExit()
{

}
// used to add screens
void App::AddScreens()
{
  m_gamePlayScreen = std::make_unique<GameplayScreen>(&m_window);

  m_screenList->AddScreen(m_gamePlayScreen.get());

  m_screenList->SetScreen(m_gamePlayScreen->GetScreenIndex());
}
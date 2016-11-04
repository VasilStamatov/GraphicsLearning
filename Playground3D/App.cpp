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
  m_screenWidth = 960;
  m_screenHeight = 540;
  m_windowFlags = GameEngine::WindowCreationFlags::RESIZABLE;
  
}
// called when exiting
void App::OnExit()
{

}
// used to add screens
void App::AddScreens()
{
  m_gamePlayScreen = std::make_shared<GameplayScreen>(&m_window);
		m_screenList->SetScreen(m_gamePlayScreen->GetScreenIndex());
  m_screenList->AddScreen(std::move(m_gamePlayScreen));
}
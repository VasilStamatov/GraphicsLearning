#include "ScreenList.h"

#include "IGameScreen.h"

namespace GameEngine
{
  ScreenList::ScreenList(IMainGame* _game) : m_game(_game)
  {
    //empty
  }
  ScreenList::~ScreenList()
  {
    Destroy();
  }

  //make the current screen index move to the next one
  IGameScreen* ScreenList::MoveNext()
  {
    IGameScreen* currentScreen = GetCurrent();
    //check if there is actually a next screen index
    if (currentScreen->GetNextScreenIndex() != SCREEN_INDEX_NO_SCREEN)
    {
      //set the next screen index
      m_currentScreenIndex = currentScreen->GetNextScreenIndex();
    }
    //return either the next screen index, or the current one if there is no next
    return GetCurrent();
  }

  //move to the previous screen index
  IGameScreen* ScreenList::MovePrevious() 
  {
    IGameScreen* currentScreen = GetCurrent();
    //check if there is actually a previous screen index
    if (currentScreen->GetPreviousScreenIndex() != SCREEN_INDEX_NO_SCREEN) 
    {
      //set the previous screen index
      m_currentScreenIndex = currentScreen->GetPreviousScreenIndex();
    }
    //return either the previous screen index, or the current one if there is no previous
    return GetCurrent();
  }

  void ScreenList::SetScreen(int _nextScreen) 
  {
    //set the screen to a specific screen index
    m_currentScreenIndex = _nextScreen;
  }

  //add a new screen to the game
  void ScreenList::AddScreen(IGameScreen* _newScreen)
  {
    //set its screen index
    _newScreen->m_screenIndex = m_screens.size();
    //push it bck to the vector of screens
    m_screens.push_back(_newScreen);
    //do the custom build function
    _newScreen->Build();
    //set the parent game
    _newScreen->SetParentGame(m_game);
  }

  //destroy the screens
  void ScreenList::Destroy() 
  {
    for (size_t i = 0; i < m_screens.size(); i++)
    {
      //call every screens destrou function
      m_screens[i]->Destroy();
    }
    //resize the screens vector to 0 and make the current screen index to no screen
    m_screens.resize(0);
    m_currentScreenIndex = SCREEN_INDEX_NO_SCREEN;
  }

  IGameScreen* ScreenList::GetCurrent() 
  {
    //check if the current screen isn't SCREEN_INDEX_NO_SCREEN and if not, return the current screen index
    if (m_currentScreenIndex == SCREEN_INDEX_NO_SCREEN) return nullptr;
    return m_screens[m_currentScreenIndex];
  }
}


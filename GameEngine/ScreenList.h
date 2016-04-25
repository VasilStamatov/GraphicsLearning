#pragma once

#include <vector>

namespace GameEngine
{
  class IMainGame;
  class IGameScreen;

  class ScreenList
  {
  public:
    ScreenList(IMainGame* _game);
    ~ScreenList();
    //goes to the next screen index
    IGameScreen* MoveNext();
    //goes to the previous screen index
    IGameScreen* MovePrevious();
    //sets the current screen index
    void SetScreen(int _nextScreen);
    //adds a screen
    void AddScreen(IGameScreen* _newScreen);
    //destrous all the screens and resizes the screens vector to 0
    void Destroy();

    //gets the current screen index
    IGameScreen* GetCurrent();

  protected:
    IMainGame* m_game = nullptr;
    std::vector<IGameScreen*> m_screens;
    int m_currentScreenIndex = -1;
  };
}
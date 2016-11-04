#pragma once

#include <vector>
#include <memory>

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
    std::weak_ptr<IGameScreen> MoveNext();
    //goes to the previous screen index
				std::weak_ptr<IGameScreen> MovePrevious();
    //sets the current screen index
    void SetScreen(int _nextScreen);
    //adds a screen
    void AddScreen(std::shared_ptr<IGameScreen> _newScreen);
    //destrous all the screens and resizes the screens vector to 0
    void Destroy();

    //gets the current screen index
				std::shared_ptr<IGameScreen> GetCurrent();

  protected:
    IMainGame* m_game{ nullptr };
    std::vector<std::shared_ptr<IGameScreen>> m_screens;
    int m_currentScreenIndex{ -1 };
  };
}
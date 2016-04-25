#ifndef _APP_
#define _APP_

#include <GameEngine\IMainGame.h>
#include "GameplayScreen.h"
#include "MainMenuScreen.h"
#include "EditorScreen.h"
#include "GameOverScreen.h"
#include "LevelClearedScreen.h"

// The custom app that inherits from IMainGame
class App : public GameEngine::IMainGame
{
public:
  App();
  ~App();
  // called on initialization
  virtual void OnInit() override;
  // called when exiting
  virtual void OnExit() override;
  // used to add screens
  virtual void AddScreens() override;

private:
  //unique pointer of all the screens
  std::unique_ptr<GameplayScreen> m_gamePlayScreen = nullptr;
  std::unique_ptr<MainMenuScreen> m_mainMenuScreen = nullptr;
  std::unique_ptr<EditorScreen> m_editorScreen = nullptr;
  std::unique_ptr<GameOverScreen> m_gameOverScreen = nullptr;
  std::unique_ptr<LevelClearedScreen> m_levelClearedScreen = nullptr;
};

#endif
#pragma once

#include <GameEngine/IGameScreen.h>
#include <GameEngine/Window.h>
#include <GameEngine/Camera2D.h>
#include <GameEngine/GUI.h>
#include <GameEngine/GLSLProgram.h>
#include <GameEngine/AudioEngine.h>
#include <GameEngine/SpriteFont.h>
#include <GameEngine\SpriteBatch.h>

#include "ScreenIndices.h"

class LevelClearedScreen : public GameEngine::IGameScreen
{
public:
  LevelClearedScreen(GameEngine::Window* _window);
  ~LevelClearedScreen();

  virtual int GetNextScreenIndex() const override;

  virtual int GetPreviousScreenIndex() const override;

  virtual void Build() override;

  virtual void Destroy() override;

  virtual void OnEntry() override;

  virtual void OnExit() override;

  virtual void Update() override;

  virtual void Draw() override;

private:
  void InitUI();
  void CheckInput();
  /************************************************************************/
  /* Event Handlers                                                       */
  /************************************************************************/
  bool OnRetryClicked(const CEGUI::EventArgs& _e);
  bool OnBackClicked(const CEGUI::EventArgs& _e);
  bool OnExitClicked(const CEGUI::EventArgs& _e);

  /************************************************************************/
  /* Member Variables                                                     */
  /************************************************************************/
  CEGUI::PushButton* m_exitButton = nullptr;
  CEGUI::PushButton* m_backButton = nullptr;
  CEGUI::PushButton* m_retryButton = nullptr;

  GameEngine::Camera2D m_camera;
  GameEngine::Window* m_window;
  GameEngine::GUI m_gui;

  GameEngine::GLSLProgram m_textureProgram;
  GameEngine::AudioEngine m_audio;
  GameEngine::SpriteFont m_spriteFont;
  GameEngine::SpriteBatch m_spriteBatch;

  int m_previousScreen = SCREEN_INDEX_GAMEPLAY;
};


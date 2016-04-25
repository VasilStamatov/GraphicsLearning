#ifndef _MAINMENUSCREEN_
#define _MAINMENUSCREEN_

#include "Box.h"
#include "Player.h"
#include <GameEngine/IGameScreen.h>
#include <vector>
#include <string>
#include <GameEngine/SpriteBatch.h>
#include <GameEngine/GLSLProgram.h>
#include <GameEngine/Camera2D.h>
#include <GameEngine/GLTexture.h>
#include <GameEngine/Window.h>
#include <GameEngine/DebugRenderer.h>
#include <GameEngine\AudioEngine.h>
#include <GameEngine\SpriteFont.h>

#include <GameEngine/GUI.h>

#include "ScreenIndices.h"

class MainMenuScreen : public GameEngine::IGameScreen
{
public:
  MainMenuScreen(GameEngine::Window* _window);
  ~MainMenuScreen();

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
  bool OnNewGameClicked(const CEGUI::EventArgs& e);
  bool OnExitClicked(const CEGUI::EventArgs& e);
  bool OnLoadMouseClick(const CEGUI::EventArgs& e);
  bool OnLoadCancelClick(const CEGUI::EventArgs& e);
  bool OnLoad(const CEGUI::EventArgs& e);
  bool OnEditorClicked(const CEGUI::EventArgs& e);
  bool OnMusicVolumeChanged(const CEGUI::EventArgs& e);

  /************************************************************************/
  /* Member Variables                                                     */
  /************************************************************************/
  std::vector<CEGUI::ListboxTextItem*> m_loadListBoxItems;

  CEGUI::FrameWindow* m_loadWindow = nullptr;
  CEGUI::Combobox*    m_loadWindowCombobox = nullptr;
  CEGUI::PushButton*  m_loadWindowLoadButton = nullptr;
  CEGUI::PushButton*  m_playButton = nullptr;
  CEGUI::PushButton*  m_exitButton = nullptr;
  CEGUI::PushButton*  m_editorButton = nullptr;
  CEGUI::Spinner*     m_volumeSpinner = nullptr;

  int m_nextScreenIndex = SCREEN_INDEX_GAMEPLAY;
  GameEngine::Camera2D m_camera;
  GameEngine::Window* m_window;
  GameEngine::GUI m_gui;
  GameEngine::AudioEngine m_audio;
  GameEngine::Music m_music;
  GameEngine::SpriteBatch m_spriteBatch;
  GameEngine::SpriteFont m_spriteFont;
  GameEngine::GLSLProgram m_textureProgram;
};

#endif
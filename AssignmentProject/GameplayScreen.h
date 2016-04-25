#ifndef _GAMEPLAYSCREEN_
#define _GAMEPLAYSCREEN_

#include "Box.h"
#include "Player.h"
#include "Light.h"
#include "Projectile.h"
#include "EnemyRobot.h"
#include "Coins.h"
#include "ScreenIndices.h"

#include <GameEngine/IGameScreen.h>
#include <Box2D/Box2D.h>
#include <vector>
#include <GameEngine/SpriteBatch.h>
#include <GameEngine/GLSLProgram.h>
#include <GameEngine/Camera2D.h>
#include <GameEngine/GLTexture.h>
#include <GameEngine/Window.h>
#include <GameEngine/DebugRenderer.h>
#include <GameEngine\SpriteFont.h>
#include <GameEngine/AudioEngine.h>
#include "ParallaxBackground.h"

#include <GameEngine/GUI.h>

// Our custom gameplay screen that inherits from the IGameScreen

class GameplayScreen : public GameEngine::IGameScreen
{
public:
  GameplayScreen(GameEngine::Window* _window);
  ~GameplayScreen();

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
  void ClearLevel();
  
  void DrawHUD();

  //events
  bool OnLoadMouseClick(const CEGUI::EventArgs& e);
  bool OnLoadCancelClick(const CEGUI::EventArgs& e);
  bool OnLoad(const CEGUI::EventArgs& e);

  bool OnSaveMouseClick(const CEGUI::EventArgs& e);
  bool OnSaveCancelClick(const CEGUI::EventArgs& e);
  bool OnSave(const CEGUI::EventArgs& e);

  bool OnExitClicked(const CEGUI::EventArgs& e);
  bool OnBackClicked(const CEGUI::EventArgs& e);

  bool OnGameOver(const CEGUI::EventArgs& e);
  bool OnLevelClear(const CEGUI::EventArgs& e);
  bool OnMusicVolumeChanged(const CEGUI::EventArgs& e);

  //save widgets
  CEGUI::FrameWindow* m_saveWindow = nullptr;
  CEGUI::PushButton* m_saveWindowSaveButton = nullptr;
  CEGUI::Combobox* m_saveWindowCombobox = nullptr;
  //load widgets
  CEGUI::FrameWindow* m_loadWindow = nullptr;
  CEGUI::PushButton* m_loadWindowLoadButton = nullptr;
  CEGUI::Combobox* m_loadWindowCombobox = nullptr;
  //the buttons
  CEGUI::PushButton* m_exitButton = nullptr;
  CEGUI::PushButton* m_saveButton = nullptr;
  CEGUI::PushButton* m_loadButton = nullptr;
  CEGUI::PushButton* m_backButton = nullptr;
  //volume spinner
  CEGUI::Spinner* m_volumeSpinner = nullptr;
  //the save/load lists
  std::vector<CEGUI::ListboxTextItem*> m_saveListBoxItems;
  std::vector<CEGUI::ListboxTextItem*> m_loadListBoxItems;
  //the spritebatch
  GameEngine::SpriteBatch m_spriteBatch;
  //the spritefont
  GameEngine::SpriteFont m_spriteFont;
  //the texturing program for sprites and lights
  GameEngine::GLSLProgram m_textureProgram;
  GameEngine::GLSLProgram m_lightProgram;
  //the cameras
  GameEngine::Camera2D m_camera;
  GameEngine::Camera2D m_HUDCamera;
  //the pointer to the game window
  GameEngine::Window* m_window;
  //the debug renderer
  GameEngine::DebugRenderer m_debugRenderer;
  //the gui
  GameEngine::GUI m_gui;
  //the audio engine
  GameEngine::AudioEngine m_audio;
  GameEngine::Music m_music;
  //a bunch of booleans for the game
  bool m_renderDebug = false;
  bool m_hasPlayer = false;
  bool m_hasExit = false;
  bool m_hasTrigger = false;
  bool m_isUnlocked = false;
  //the player object
  Player m_player;
  //specifying what the next screen is (can be changed)
  int m_nextScreen = SCREEN_INDEX_GAMEOVER;
  //the vectors with game objects
  std::vector<Box> m_boxes;
  std::vector<Light> m_lights;
  std::vector<Box> m_obstacles;
  std::vector<Coins> m_coins;
  std::vector<EnemyRobot> m_enemies;
  std::vector<Projectile> m_projectiles;
  std::vector<ParallaxBackground> m_backgroundLayers;

  // a single exit and trigger
  Box m_exit;
  Box m_trigger;
  // a unique pointer to the box2d world
  std::unique_ptr<b2World> m_world;
};

#endif
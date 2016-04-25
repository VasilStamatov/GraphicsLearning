#ifndef _EDITORSCREEN_
#define _EDITORSCREEN_

#include "Box.h"
#include "Player.h"
#include "Light.h"
#include "EnemyRobot.h"
#include "Coins.h"
#include <GameEngine/Camera2D.h>
#include <GameEngine/DebugRenderer.h>
#include <GameEngine/GLSLProgram.h>
#include <GameEngine/GLTexture.h>
#include <GameEngine/IGameScreen.h>
#include <GameEngine/InputManager.h>
#include <GameEngine/SpriteBatch.h>
#include <GameEngine/SpriteFont.h>
#include <GameEngine/Window.h>
#include <GameEngine/GUI.h>
#include <GameEngine\AudioEngine.h>
#include <vector>

#include "ScreenIndices.h"

enum class PhysicsMode {DYNAMIC, RIGID};
enum class ObjectMode {PLAYER, PLATFORM, TRIGGER, LIGHT, FINISH, OBSTACLE, ENEMY, COIN};
enum class SelectionMode {SELECT, PLACE};

const int NO_BOX = -1;
const int NO_LIGHT = -1;
const int NO_OBSTACLE = -1;
const int NO_ENEMY = -1;
const int NO_COIN = -1;

class WidgetLabel
{
public:
  WidgetLabel() {};
  WidgetLabel(CEGUI::Window* _w, const std::string& _text, float _scale) :
    widget(_w), text(_text), scale(_scale) {/*empty*/}

  void Draw(GameEngine::SpriteBatch& _sb, GameEngine::SpriteFont& _sf, GameEngine::Window* _w);

  CEGUI::Window* widget = nullptr;
  std::string text = "";
  GameEngine::ColorRGBA8 color = GameEngine::ColorRGBA8(255, 255, 255, 255);
  float scale = 0.7f;
};

class EditorScreen : public GameEngine::IGameScreen
{
public:
  EditorScreen(GameEngine::Window* _window);
  ~EditorScreen();

  /*************************************
  **    IGameScreen Implementation     *
  **************************************/
    
  virtual int GetNextScreenIndex() const override;

  virtual int GetPreviousScreenIndex() const override;

  virtual void Build() override;

  virtual void Destroy() override;

  virtual void OnEntry() override;

  virtual void OnExit() override;

  virtual void Update() override;

  virtual void Draw() override;

private:
  /*************************************
  **         Private Funtions          *
  **************************************/
  void DrawUI();
  void DrawWorld();
  void ClearLevel();

  void InitUI();
  void CheckInput();
  void UpdateMouseDown(const SDL_Event& _evnt);
  void UpdateMouseUp(const SDL_Event& _evnt);
  void UpdateMouseMotion(const SDL_Event& _evnt);
  void RefreshSelectedBox();
  void RefreshSelectedBox(const glm::vec2& _newPos);
  void RefreshSelectedLight();
  void RefreshSelectedLight(const glm::vec2& _newPos);
  void RefreshSelectedObstacle();
  void RefreshSelectedObstacle(const glm::vec2& _newPos);
  void RefreshSelectedEnemy();
  void RefreshSelectedEnemy(const glm::vec2& _newPos);
  void RefreshSelectedCoin();
  void RefreshSelectedCoin(const glm::vec2& _newPos);
  bool isMouseInUI();

  void SetPlatformWidgetVisibility(bool _visible);
  void SetLightWidgetVisibility(bool _visible);

  /*************************************
  **           Event Handlers          *
  **************************************/

  bool OnExitClicked(const CEGUI::EventArgs& e);
  bool OnColorPickerRedChanged(const CEGUI::EventArgs& e);
  bool OnColorPickerGreenChanged(const CEGUI::EventArgs& e);
  bool OnColorPickerBlueChanged(const CEGUI::EventArgs& e);
  bool OnColorPickerAlphaChanged(const CEGUI::EventArgs& e);

  bool OnRigidMouseClick(const CEGUI::EventArgs& e);
  bool OnDynamicMouseClick(const CEGUI::EventArgs& e);
  bool OnPlayerMouseClick(const CEGUI::EventArgs& e);
  bool OnPlatformMouseClick(const CEGUI::EventArgs& e);
  bool OnTriggerMouseClick(const CEGUI::EventArgs& e);
  bool OnFinishMouseClick(const CEGUI::EventArgs& e);
  bool OnLightMouseClick(const CEGUI::EventArgs& e);
  bool OnObstaleMouseClick(const CEGUI::EventArgs& e);
  bool OnEnemyMouseClick(const CEGUI::EventArgs& e);
  bool OnCoinMouseClick(const CEGUI::EventArgs& e);
  bool OnSelectMouseClick(const CEGUI::EventArgs& e);
  bool OnPlaceMouseClick(const CEGUI::EventArgs& e);
  bool OnSaveMouseClick(const CEGUI::EventArgs& e);
  bool OnLoadMouseClick(const CEGUI::EventArgs& e);
  bool OnBackMouseClick(const CEGUI::EventArgs& e);
  bool OnRotationValueChange(const CEGUI::EventArgs& e);
  bool OnSizeValueChange(const CEGUI::EventArgs& e);
  bool OnWidthValueChange(const CEGUI::EventArgs& e);
  bool OnHeightValueChange(const CEGUI::EventArgs& e);
  bool OnDebugToggleClick(const CEGUI::EventArgs& e);
  bool OnSaveCancelClick(const CEGUI::EventArgs& e);
  bool OnLoadCancelClick(const CEGUI::EventArgs& e);
  bool OnSave(const CEGUI::EventArgs& e);
  bool OnLoad(const CEGUI::EventArgs& e);
  bool OnMusicVolumeChanged(const CEGUI::EventArgs& e);

  /*************************************
  **          Member Variables         *
  **************************************/
  //the rgba colors
  float m_colorPickerRed = 255.0f;
  float m_colorPickerGreen = 255.0f;
  float m_colorPickerBlue = 255.0f;
  float m_colorPickerAlpha = 255.0f;
  //rotation of objects
  float m_rotation = 0.0f;
  //size of the lights (radius)
  float m_lightSize = 10.0f;
  //dimensions of box objects
  glm::vec2 m_boxDims = glm::vec2(1.0f);

  //the 2 mouse buttons
  bool m_mouseButtons[2];

  bool m_debugRender = false;
  //CEGUI object pointers
  CEGUI::GroupBox* m_groupBox = nullptr;

  CEGUI::Spinner* m_rSpinner = nullptr;
  CEGUI::Spinner* m_gSpinner = nullptr;
  CEGUI::Spinner* m_bSpinner = nullptr;
  CEGUI::Spinner* m_aSpinner = nullptr;

  CEGUI::RadioButton* m_rigidRadioButton = nullptr;
  CEGUI::RadioButton* m_dynamicRadioButton = nullptr;

  CEGUI::RadioButton* m_playerRadioButton = nullptr;
  CEGUI::RadioButton* m_platformRadioButton = nullptr;
  CEGUI::RadioButton* m_lightRadioButton = nullptr;
  CEGUI::RadioButton* m_triggerRadioButton = nullptr;
  CEGUI::RadioButton* m_finishRadioButton = nullptr;
  CEGUI::RadioButton* m_obstacleRadioButton = nullptr;
  CEGUI::RadioButton* m_enemyRadioButton = nullptr;
  CEGUI::RadioButton* m_coinRadioButton = nullptr;

  CEGUI::RadioButton* m_selectRadioButton = nullptr;
  CEGUI::RadioButton* m_placeRadioButton = nullptr;

  CEGUI::ToggleButton* m_debugToggle = nullptr;

  CEGUI::Spinner* m_rotationSpinner = nullptr;
  CEGUI::Spinner* m_widthSpinner = nullptr;
  CEGUI::Spinner* m_heightSpinner = nullptr;
  CEGUI::Spinner* m_sizeSpinner = nullptr;
  CEGUI::Spinner* m_volumeSpinner = nullptr;

  CEGUI::FrameWindow* m_saveWindow = nullptr;
  CEGUI::PushButton* m_saveWindowSaveButton = nullptr;
  CEGUI::Combobox* m_saveWindowCombobox = nullptr;

  CEGUI::FrameWindow* m_loadWindow = nullptr;
  CEGUI::PushButton* m_loadWindowLoadButton = nullptr;
  CEGUI::Combobox* m_loadWindowCombobox = nullptr;

  std::vector<CEGUI::ListboxTextItem*> m_saveListBoxItems;
  std::vector<CEGUI::ListboxTextItem*> m_loadListBoxItems;

  CEGUI::PushButton* m_saveButton = nullptr;
  CEGUI::PushButton* m_loadButton = nullptr;
  CEGUI::PushButton* m_backButton = nullptr;
  //the labels for the CEGUI objects
  std::vector<WidgetLabel> m_widgetLabels;
  //sprite batch and spritefont
  GameEngine::SpriteBatch m_spriteBatch;
  GameEngine::SpriteFont m_spriteFont;
  //the shader program for the sprites and lights
  GameEngine::GLSLProgram m_textureProgram;
  GameEngine::GLSLProgram m_lightProgram;
  //the debug renderer
  GameEngine::DebugRenderer m_debugRenderer;
  //a blank texture
  GameEngine::GLTexture m_blankTexture;
  //the audio engine
  GameEngine::AudioEngine m_audio;
  //the music (it's kept here so the volume can be changed)
  GameEngine::Music m_music;
  //Enumerator modes
  PhysicsMode m_physicsMode = PhysicsMode::RIGID;
  ObjectMode m_objectMode = ObjectMode::PLAYER;
  SelectionMode m_selectionMode = SelectionMode::SELECT;
  //the input manager
  GameEngine::InputManager m_inputManager;
  //booleans to check if the necessary objects for a level are met
  bool m_hasPlayer;
  bool m_hasTrigger;
  bool m_hasExit;
  // the objects which every level need 1 of
  Player m_player;
  Box m_trigger;
  Box m_exit;
  //vectors of objects
  std::vector<Box> m_boxes;
  std::vector<Box> m_obstacles;
  std::vector<Light> m_lights;
  std::vector<EnemyRobot> m_enemies;
  std::vector<Coins> m_coins;
  //integers checking which object is selected (set to no by default)
  int m_selectedBox = NO_BOX;
  int m_selectedLight = NO_LIGHT;
  int m_selectedObstacle = NO_OBSTACLE;
  int m_selectedEnemy = NO_ENEMY;
  int m_selectedCoin = NO_COIN;
  //check if the player is dragging objects around
  bool m_isDragging = false;
  //a vector of the selected offset when dragging
  glm::vec2 m_selectedOffset;
  //the cameras
  GameEngine::Camera2D m_camera;
  GameEngine::Camera2D m_uiCamera;
  //the pointer to the game window
  GameEngine::Window* m_window;
  //the GUI
  GameEngine::GUI m_gui;
  //unique pointer to the box2d world
  std::unique_ptr<b2World> m_world;
};

#endif
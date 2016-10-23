#include "MainMenuScreen.h"
#include "LevelReaderWriter.h"
#include "LevelPathContainer.h"

#include <GameEngine/ResourceManager.h>
#include <GameEngine/IOManager.h>


MainMenuScreen::MainMenuScreen(GameEngine::Window* _window) : m_window(_window)
{
  m_screenIndex = SCREEN_INDEX_MAINMENU;
}


MainMenuScreen::~MainMenuScreen()
{
}

int MainMenuScreen::GetNextScreenIndex() const
{
  //go to a specific screen index depending on the user's choice
  return m_nextScreenIndex;
}
int MainMenuScreen::GetPreviousScreenIndex() const
{
  return SCREEN_INDEX_NO_SCREEN;
}

void MainMenuScreen::Build()
{

}

void MainMenuScreen::Destroy()
{

}

void MainMenuScreen::OnEntry()
{
  //Init camera
  m_camera.Init(m_window->GetScreenWidth(), m_window->GetScreenHeight());
  m_camera.SetScale(32.0f);

  //Init audio
  m_audio.Init();

  //Init UI
  InitUI();

  //Init spritebatch
  m_spriteBatch.Init();
  //Init spritefont
  m_spriteFont.init("Fonts/chintzy.ttf", 32);

  // Shader init
  // Compile our texture shader
  m_textureProgram.CompileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");

  //load the main menu music
  m_music = m_audio.LoadMusic("Sound/Post-Punk-1.ogg");
  m_music.Play(-1);

}

void MainMenuScreen::OnExit()
{
  m_gui.DestroyGUI();
  m_audio.Destroy();
  m_spriteBatch.Dispose();
  m_spriteFont.dispose();
  m_textureProgram.Dispose();
}

void MainMenuScreen::Update()
{
  //update the camera
  m_camera.Update();

  //check for user inputs
  CheckInput();
}

void MainMenuScreen::Draw()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.0f, 0.0f, 0.2f, 1.0f);

  //begin using the texture program
  m_textureProgram.Use();

  //camera matrix
  glm::mat4 projectionMatrix = m_camera.GetCameraMatrix();
  GLint pUniform = m_textureProgram.GetUniformLocation("P");
  glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

  //start up the spritebatch
  m_spriteBatch.Begin();

  std::string volumeText = "Music Volume";
  std::string mainMenuText = "Main Menu";
  std::string instructionText = "Use W,A,D to move and SPACE to shoot!";
  //draw the music volume text, which is located above the volume spinner
  m_spriteFont.draw(m_spriteBatch, volumeText.c_str(), glm::vec2(10.0f, -7.0f), glm::vec2(0.02f), 0.0f, GameEngine::ColorRGBA8(255, 255, 255, 255), GameEngine::Justification::MIDDLE);
  m_spriteFont.draw(m_spriteBatch, mainMenuText.c_str(), glm::vec2(0.0f, 2.0f), glm::vec2(0.1f), 0.0f, GameEngine::ColorRGBA8(255, 255, 255, 255), GameEngine::Justification::MIDDLE);
  m_spriteFont.draw(m_spriteBatch, instructionText.c_str(), glm::vec2(0.0f, -6.0f), glm::vec2(0.02f), 0.0f, GameEngine::ColorRGBA8(255, 255, 255, 255), GameEngine::Justification::MIDDLE);

  //end the spritebatch usage
  m_spriteBatch.End();
  //render the batches ( just the text )
  m_spriteBatch.RenderBatch();
  //draw the gui
  m_gui.Draw();
  //unuse the texture program
  m_textureProgram.UnUse();
}

void MainMenuScreen::InitUI()
{
  //Init the UI
  m_gui.Init("GUI");
  m_gui.LoadScheme("TaharezLook.scheme");
  m_gui.SetFont("DejaVuSans-10");

  //play new game and load game button
  m_playButton = static_cast<CEGUI::PushButton*>(m_gui.CreateWidget("TaharezLook/Button", glm::vec4(0.45f, 0.45f, 0.1f, 0.05f), glm::vec4(0.0f), "PlayGameButton"));
  m_playButton->setText("New Game");
  //set up the even to be called when the button is clicked
  m_playButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenuScreen::OnNewGameClicked, this));

  m_playButton = static_cast<CEGUI::PushButton*>(m_gui.CreateWidget("TaharezLook/Button", glm::vec4(0.45f, 0.50f, 0.1f, 0.05f), glm::vec4(0.0f), "LoadGameButton"));
  m_playButton->setText("Load Game");
  m_playButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenuScreen::OnLoadMouseClick, this));

  m_editorButton = static_cast<CEGUI::PushButton*>(m_gui.CreateWidget("TaharezLook/Button", glm::vec4(0.45f, 0.55f, 0.1f, 0.05f), glm::vec4(0.0f), "EditorButton"));
  m_editorButton->setText("Editor");
  m_editorButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenuScreen::OnEditorClicked, this));

  //load window widgets
  m_loadWindow = static_cast<CEGUI::FrameWindow*>(m_gui.CreateWidget("TaharezLook/FrameWindow", glm::vec4(0.02f, 0.2f, 0.4f, 0.4f), glm::vec4(0.0f), "LoadWindow"));
  m_loadWindow->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber(&MainMenuScreen::OnLoadCancelClick, this));
  m_loadWindow->setText("Load Game");
  //don't let the user draw the window around
  m_loadWindow->setDragMovingEnabled(false);

  //children of loadWindow
  m_loadWindowCombobox = static_cast<CEGUI::Combobox*>(m_gui.CreateWidget(m_loadWindow, "TaharezLook/Combobox", glm::vec4(0.1f, 0.26f, 0.8f, 0.4f), glm::vec4(0.0f), "LoadCombobox"));
  m_loadWindowLoadButton = static_cast<CEGUI::PushButton*>(m_gui.CreateWidget(m_loadWindow, "TaharezLook/Button", glm::vec4(0.3f, 0.6f, 0.3f, 0.1f), glm::vec4(0.0f), "LoadCancelButton"));
  m_loadWindowLoadButton->setText("Load");
  m_loadWindowLoadButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenuScreen::OnLoad, this));

  //has to start disabled
  m_loadWindow->setAlpha(0.0f);
  m_loadWindow->disable();

  //exit button
  m_exitButton = static_cast<CEGUI::PushButton*>(m_gui.CreateWidget("TaharezLook/Button", glm::vec4(0.45f, 0.60f, 0.1f, 0.05f), glm::vec4(0.0f), "ExitGameButton"));
  m_exitButton->setText("Exit");
  //set up the even to be called when the button is clicked
  m_exitButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenuScreen::OnExitClicked, this));

  //music volume spinner
  m_volumeSpinner = static_cast<CEGUI::Spinner*>(m_gui.CreateWidget("TaharezLook/Spinner", glm::vec4(0.75f, 0.8f, 0.1f, 0.05f), glm::vec4(0.0f), "VolumeSpinner"));
  m_volumeSpinner->setMinimumValue(0.0f);
  m_volumeSpinner->setMaximumValue(128.0f);
  m_volumeSpinner->setCurrentValue(128.0f);
  m_volumeSpinner->setTextInputMode(CEGUI::Spinner::Integer);
  m_volumeSpinner->subscribeEvent(CEGUI::Spinner::EventValueChanged, CEGUI::Event::Subscriber(&MainMenuScreen::OnMusicVolumeChanged, this));
  m_volumeSpinner->setStepSize(1.0f);

  //set the custom mouse cursor and hide the old boring one
  m_gui.SetMouseCursor("TaharezLook/MouseArrow");
  m_gui.ShowMouseCursor();
  SDL_ShowCursor(0);
}
void MainMenuScreen::CheckInput()
{
  SDL_Event evnt;
  while (SDL_PollEvent(&evnt))
  {
    m_gui.OnSDLEvent(evnt);
    switch (evnt.type)
    {
    case SDL_QUIT:
    {
      OnExitClicked(CEGUI::EventArgs());
      break;
    }
    default:
      break;
    }
  }
}


bool MainMenuScreen::OnNewGameClicked(const CEGUI::EventArgs& _e)
{
  m_nextScreenIndex = SCREEN_INDEX_GAMEPLAY;
  m_currentState = GameEngine::ScreenState::CHANGE_NEXT;
  return true;
}
bool MainMenuScreen::OnExitClicked(const CEGUI::EventArgs& _e)
{
  m_currentState = GameEngine::ScreenState::EXIT_APPLICATION;
  return true;
}
bool MainMenuScreen::OnLoadMouseClick(const CEGUI::EventArgs& e)
{
  GameEngine::SoundEffect soundEffect = m_audio.LoadSoundEffect("Sound/Blip_Select11.ogg");
  soundEffect.Play();

  m_loadWindowCombobox->clearAllSelections();

  //Remove all items
  for (auto& item : m_loadListBoxItems)
  {
    //delete doesnt need to be called since removeItem does this for me
    m_loadWindowCombobox->removeItem(item);
  }
  m_loadListBoxItems.clear();

  std::vector<GameEngine::DirEntry> entries;
  GameEngine::IOManager::GetDirectoryEntries("Levels", entries);

  //add all files to list box
  for (auto& e : entries)
  {
    //Don't add directories
    if (!e.isDirectory)
    {
      //Remove "Levels/" substring
      e.path.erase(0, std::string("Levels/").size());
      m_loadListBoxItems.push_back(new CEGUI::ListboxTextItem(e.path));
      m_loadWindowCombobox->addItem(m_loadListBoxItems.back());
    }
  }
  m_loadWindow->enable();
  m_loadWindow->setAlpha(1.0f);
  return true;
}
bool MainMenuScreen::OnLoadCancelClick(const CEGUI::EventArgs& e)
{
  GameEngine::SoundEffect soundEffect = m_audio.LoadSoundEffect("Sound/Blip_Select11.ogg");
  soundEffect.Play();

  m_loadWindow->disable();
  m_loadWindow->setAlpha(0.0f);
  return true;
}
bool MainMenuScreen::OnLoad(const CEGUI::EventArgs& e)
{
  m_nextScreenIndex = SCREEN_INDEX_GAMEPLAY;

  std::string path = "Levels/" + std::string(m_loadWindowCombobox->getText().c_str());

  levelPath = path;

  m_currentState = GameEngine::ScreenState::CHANGE_NEXT;

  m_loadWindow->disable();
  m_loadWindow->setAlpha(0.0f);
  return true;
}
bool MainMenuScreen::OnEditorClicked(const CEGUI::EventArgs& e)
{
  m_nextScreenIndex = SCREEN_INDEX_EDITOR;
  m_currentState = GameEngine::ScreenState::CHANGE_NEXT;
  return true;
}

bool MainMenuScreen::OnMusicVolumeChanged(const CEGUI::EventArgs& e)
{
  int volumeValue = 0;
  volumeValue = (int)m_volumeSpinner->getCurrentValue();

  m_music.EditVolume(volumeValue);

  return true;
}
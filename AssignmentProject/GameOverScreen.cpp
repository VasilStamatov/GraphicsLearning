#include "GameOverScreen.h"
#include "GameStats.h"

GameOverScreen::GameOverScreen(GameEngine::Window* _window) : m_window(_window)
{
  m_screenIndex = SCREEN_INDEX_GAMEOVER;
}


GameOverScreen::~GameOverScreen()
{
}

int GameOverScreen::GetNextScreenIndex() const
{
  return SCREEN_INDEX_NO_SCREEN;
}

int GameOverScreen::GetPreviousScreenIndex() const
{
  //return custom previous screen (depends on player decision)
  return m_previousScreen;
}
void GameOverScreen::Build()
{

}
void GameOverScreen::Destroy()
{

}
void GameOverScreen::OnEntry()
{
  //init audio
  m_audio.Init();
  GameEngine::Music music = m_audio.LoadMusic("Sound/NoHope.ogg");
  music.Play();

  //init the spritefont
  m_spriteFont.init("Fonts/chintzy.ttf", 32);

  //init the spritebatch
  m_spriteBatch.Init();

  //init the camera
  m_camera.Init(m_window->GetScreenWidth(), m_window->GetScreenHeight());
  m_camera.SetScale(32.0f);

  //compile the texture shader
  m_textureProgram.CompileShaders("Shaders/textureShading.vert","Shaders/textureShading.frag" );

  //initialize the ui
  InitUI();

}
void GameOverScreen::OnExit()
{
  m_audio.Destroy();
  m_spriteBatch.Dispose();
  m_spriteFont.dispose();
  m_gui.DestroyGUI();
  m_textureProgram.Dispose();
}
void GameOverScreen::Update()
{
  //update the camera
  m_camera.Update();
  //update the gui
  m_gui.Update();
  //Check for player input
  CheckInput();
  //int x, y;

  //SDL_GetMouseState(&x, &y);
  //glm::vec2 pos = m_camera.ConvertScreenToWorld(glm::vec2(x, y));
  ////std::cout << "x: " << x << " y: " << y << std::endl;
  //std::cout << "pos.x: " << pos.x << "pos.y: " << pos.y << std::endl;
}
void GameOverScreen::Draw()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  m_textureProgram.Use();

  //camera matrix
  glm::mat4 projectionMatrix = m_camera.GetCameraMatrix();
  GLint pUniform = m_textureProgram.GetUniformLocation("P");
  glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

  m_spriteBatch.Begin();

  std::string gameOverText = "Game Over !"; //<game over text
  std::string elapsedTime = std::to_string((int)m_elapsedTime); //convert the int to string
  std::string gamePlayTime = "It took you " + elapsedTime + " seconds to die"; //<gameplay time text
  std::string enemiesKilled = "Enemies killed: " + std::to_string((m_enemiesKilled)); //< enemies killed text
  std::string coinsCollected = std::to_string(m_coinsCollected) + " coins collected !"; //< how many coins are collected

  //draw the game over text in the middle of the screen
  m_spriteFont.draw(m_spriteBatch, gameOverText.c_str(), glm::vec2(0.0f, 5.0f), glm::vec2(0.07f), 0.0f, GameEngine::ColorRGBA8(255, 255, 255, 255), GameEngine::Justification::MIDDLE);
  //draw the gameplay time below it
  m_spriteFont.draw(m_spriteBatch, gamePlayTime.c_str(), glm::vec2(0.0f, 3.0f), glm::vec2(0.030f), 0.0f, GameEngine::ColorRGBA8(255, 255, 255, 255), GameEngine::Justification::MIDDLE);
  //Tell the player how many enemies he has killed
  m_spriteFont.draw(m_spriteBatch, enemiesKilled.c_str(), glm::vec2(0.0f, 1.0f), glm::vec2(0.030f), 0.0f, GameEngine::ColorRGBA8(255, 255, 255, 255), GameEngine::Justification::MIDDLE);
  //Tell the player how many coins he has collected
  m_spriteFont.draw(m_spriteBatch, coinsCollected.c_str(), glm::vec2(0.0f, -1.0f), glm::vec2(0.030f), 0.0f, GameEngine::ColorRGBA8(255, 255, 255, 255), GameEngine::Justification::MIDDLE);

  m_spriteBatch.End();
  m_spriteBatch.RenderBatch();
  m_textureProgram.UnUse();

  //draw the gui
  m_gui.Draw();
}
void GameOverScreen::InitUI()
{
  m_gui.Init("GUI");
  m_gui.LoadScheme("TaharezLook.scheme");
  m_gui.SetFont("DejaVuSans-10");

  // add the buttons

  m_backButton = static_cast<CEGUI::PushButton*>(m_gui.CreateWidget("TaharezLook/Button", glm::vec4(0.29f, 0.64f, 0.1f, 0.05f), glm::vec4(0.0f), "BackButton"));
  m_backButton->setText("Go Back");
  m_backButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameOverScreen::OnBackClicked, this));

  m_exitButton = static_cast<CEGUI::PushButton*>(m_gui.CreateWidget("TaharezLook/Button", glm::vec4(0.44f, 0.64f, 0.1f, 0.05f), glm::vec4(0.0f), "ExitButton"));
  m_exitButton->setText("Exit");
  m_exitButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameOverScreen::OnExitClicked, this));

  m_retryButton = static_cast<CEGUI::PushButton*>(m_gui.CreateWidget("TaharezLook/Button", glm::vec4(0.59f, 0.64f, 0.1f, 0.05f), glm::vec4(0.0f), "RetryButton"));
  m_retryButton->setText("Retry");
  m_retryButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameOverScreen::OnRetryClicked, this));

  //change the mouse cursor
  m_gui.SetMouseCursor("TaharezLook/MouseArrow");
  m_gui.ShowMouseCursor();
  SDL_ShowCursor(0);
}

void GameOverScreen::CheckInput()
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
//retry the same level
bool GameOverScreen::OnRetryClicked(const CEGUI::EventArgs& _e)
{
  m_previousScreen = SCREEN_INDEX_GAMEPLAY;
  m_currentState = GameEngine::ScreenState::CHANGE_PREVIOUS;
  return true;
}
//go back to main menu
bool GameOverScreen::OnBackClicked(const CEGUI::EventArgs& _e)
{
  m_previousScreen = SCREEN_INDEX_MAINMENU;
  m_currentState = GameEngine::ScreenState::CHANGE_PREVIOUS;
  return true;
}
//exit the game
bool GameOverScreen::OnExitClicked(const CEGUI::EventArgs& _e)
{
  m_currentState = GameEngine::ScreenState::EXIT_APPLICATION;
  return true;
}
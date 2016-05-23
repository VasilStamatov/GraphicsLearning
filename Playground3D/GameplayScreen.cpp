#include "GameplayScreen.h"
#include "ScreenIndices.h"
#include <GameEngine\IMainGame.h>
#include <GameEngine\ResourceManager.h>

GameplayScreen::GameplayScreen(GameEngine::Window* _window) : m_window(_window)
{
  m_screenIndex = SCREEN_INDEX_GAMEPLAY;
}


GameplayScreen::~GameplayScreen()
{
}

int GameplayScreen::GetNextScreenIndex() const
{
  return SCREEN_INDEX_NO_SCREEN;
}

int GameplayScreen::GetPreviousScreenIndex() const
{
  return SCREEN_INDEX_NO_SCREEN;
}

void GameplayScreen::Build()
{

}

void GameplayScreen::Destroy()
{

}

void GameplayScreen::OnEntry()
{
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  m_camera.Init(45.0f, m_window->GetScreenWidth(), m_window->GetScreenHeight(), SDL_TRUE);

  std::vector<GameEngine::Shader> shaders =
  {
    { GL_VERTEX_SHADER, "Shaders/SimpleTransform.vert", "Vertex Shader" },
    { GL_FRAGMENT_SHADER, "Shaders/SingleColor.frag", "Fragment Shader" },
  };

  m_shaderProgram.CompileShaders(shaders);
  m_shaderProgram.AddAttribute("vertexPosition_modelSpace");
  m_shaderProgram.AddAttribute("vertexUV");
  m_shaderProgram.LinkShaders();

  glClearColor(0.0f, 0.0f, 1.0f, 0.0f);

  static const GLfloat g_vertex_buffer_data[] = {
    -1.0f, -1.0f, -1.0f, // triangle 1 : begin
    -1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, // triangle 1 : end
    1.0f, 1.0f, -1.0f, // triangle 2 : begin
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f, // triangle 2 : end
    1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f
  };
  static const GLfloat g_uv_buffer_data[] = {
    0.000059f, 1.0f - 0.000004f,
    0.000103f, 1.0f - 0.336048f,
    0.335973f, 1.0f - 0.335903f,
    1.000023f, 1.0f - 0.000013f,
    0.667979f, 1.0f - 0.335851f,
    0.999958f, 1.0f - 0.336064f,
    0.667979f, 1.0f - 0.335851f,
    0.336024f, 1.0f - 0.671877f,
    0.667969f, 1.0f - 0.671889f,
    1.000023f, 1.0f - 0.000013f,
    0.668104f, 1.0f - 0.000013f,
    0.667979f, 1.0f - 0.335851f,
    0.000059f, 1.0f - 0.000004f,
    0.335973f, 1.0f - 0.335903f,
    0.336098f, 1.0f - 0.000071f,
    0.667979f, 1.0f - 0.335851f,
    0.335973f, 1.0f - 0.335903f,
    0.336024f, 1.0f - 0.671877f,
    1.000004f, 1.0f - 0.671847f,
    0.999958f, 1.0f - 0.336064f,
    0.667979f, 1.0f - 0.335851f,
    0.668104f, 1.0f - 0.000013f,
    0.335973f, 1.0f - 0.335903f,
    0.667979f, 1.0f - 0.335851f,
    0.335973f, 1.0f - 0.335903f,
    0.668104f, 1.0f - 0.000013f,
    0.336098f, 1.0f - 0.000071f,
    0.000103f, 1.0f - 0.336048f,
    0.000004f, 1.0f - 0.671870f,
    0.336024f, 1.0f - 0.671877f,
    0.000103f, 1.0f - 0.336048f,
    0.336024f, 1.0f - 0.671877f,
    0.335973f, 1.0f - 0.335903f,
    0.667969f, 1.0f - 0.671889f,
    1.000004f, 1.0f - 0.671847f,
    0.667979f, 1.0f - 0.335851f
  };

  m_texture = GameEngine::ResourceManager::GetTexture("Assets/uvtemplate.png");

  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

  glGenBuffers(1, &uvBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);

  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);
  // Cull triangles which normal is not towards the camera
  glEnable(GL_CULL_FACE);
}

void GameplayScreen::OnExit()
{

}

void GameplayScreen::Update()
{
  m_camera.Update();

  CheckInput();
}
void GameplayScreen::Draw()
{
  // Clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_shaderProgram.Use();

  GLuint viewMatrixID = m_shaderProgram.GetUniformLocation("viewMatrix");
  GLuint projMatrixID = m_shaderProgram.GetUniformLocation("projMatrix");

  GLuint textureID = m_shaderProgram.GetUniformLocation("textureSampler");


  glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &m_camera.GetViewMatrix()[0][0]);
  glUniformMatrix4fv(projMatrixID, 1, GL_FALSE, &m_camera.GetProjectionMatrix()[0][0]);

  //Bind the texture in texture unit 0
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_texture.id);
  //Set our "myTextureSampler" sampler to user Texture Unit 0
  glUniform1i(textureID, 0);

  // 1rst attribute buffer : vertices
  glEnableVertexAttribArray(0); //< done in shaderProgram.Use

  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glVertexAttribPointer(
    0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
    );
  // 2nd attribute buffer : UVs
  glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
  glVertexAttribPointer(
    1, //after pos 0
    2, // size
    GL_FLOAT,
    GL_FALSE,
    0,
    (void*)0 //offset
    );
  // Draw the triangle !
  glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // 3 indices starting at 0 -> 1 triangle

  glDisableVertexAttribArray(0); //shaderProgram.Unuse();
  glDisableVertexAttribArray(1); //shaderProgram.Unuse();
}

void GameplayScreen::CheckInput()
{
  //handle user inputs
  SDL_Event evnt;
  while (SDL_PollEvent(&evnt))
  {
    m_game->OnSDLEvent(evnt);
  }

  if (m_game->inputManager.IsKeyDown(SDLK_w))
  {
    m_camera.Move(GameEngine::MoveState::FORWARD, m_game->GetFPS());
  }
  if (m_game->inputManager.IsKeyDown(SDLK_a))
  {
    m_camera.Move(GameEngine::MoveState::LEFT, m_game->GetFPS());
  }
  if (m_game->inputManager.IsKeyDown(SDLK_s))
  {
    m_camera.Move(GameEngine::MoveState::BACKWARD, m_game->GetFPS());
  }
  if (m_game->inputManager.IsKeyDown(SDLK_d))
  {
    m_camera.Move(GameEngine::MoveState::RIGHT, m_game->GetFPS());
  }
  if (m_game->inputManager.IsKeyDown(SDLK_r))
  {
    m_camera.Move(GameEngine::MoveState::UP, m_game->GetFPS());
  }
  if (m_game->inputManager.IsKeyDown(SDLK_f))
  {
    m_camera.Move(GameEngine::MoveState::DOWN, m_game->GetFPS());
  }
  if (m_game->inputManager.IsKeyDown(SDLK_q))
  {
    m_camera.IncreaseMouseSensitivity(0.01f);
  }
  if (m_game->inputManager.IsKeyDown(SDLK_e))
  {
    m_camera.IncreaseMouseSensitivity(-0.01f);
  }
  if (m_game->inputManager.GetRelativeMouseMotion().x != 0.0f || m_game->inputManager.GetRelativeMouseMotion().y != 0.0f)
  {
    m_camera.Rotate(m_game->inputManager.GetRelativeMouseMotion().x, m_game->inputManager.GetRelativeMouseMotion().y, m_game->GetFPS());
    m_game->inputManager.SetRelativeMouseMotion(0.0f, 0.0f);
  }
  if (m_game->inputManager.GetMouseWheelValue().y != 0)
  {
    m_camera.ChangeFoV(5.0f * m_game->inputManager.GetMouseWheelValue().y);
    m_game->inputManager.SetMouseWheel(0.0f, 0.0f);
  }
}
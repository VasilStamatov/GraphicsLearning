#include "GameplayScreen.h"
#include "ScreenIndices.h"
#include <GameEngine\IMainGame.h>

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
  m_camera.Init(45.0f, m_window->GetScreenWidth(), m_window->GetScreenHeight());

  m_shaderProgram.CompileShaders("Shaders/SimpleTransform.vert", "Shaders/SingleColor.frag");
  m_shaderProgram.LinkShaders();

  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);


  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);


  static const GLfloat g_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
  };
  static const GLushort g_element_buffer_data[] = { 0, 1, 2 };

  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

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
  m_shaderProgram.Use();

  glm::mat4 MVP = m_camera.GetMVP();

  GLuint matrixID = m_shaderProgram.GetUniformLocation("MVP");

  // Clear the screen
  glClear(GL_COLOR_BUFFER_BIT);

  glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);

  // 1rst attribute buffer : vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glVertexAttribPointer(
    0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
    );

  // Draw the triangle !
  glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

  m_shaderProgram.UnUse();
}

void GameplayScreen::CheckInput()
{
  const float CAMERA_MOVEMENT = 0.1f;
  //handle user inputs
  SDL_Event evnt;
  while (SDL_PollEvent(&evnt))
  {
    m_game->OnSDLEvent(evnt);

    if (m_game->inputManager.IsKeyDown(SDLK_a))
    {
      m_camera.SetPosition(m_camera.GetPosition() + glm::vec3(-CAMERA_MOVEMENT, 0.0f, 0.0f));
    }
    if (m_game->inputManager.IsKeyDown(SDLK_d))
    {
      m_camera.SetPosition(m_camera.GetPosition() + glm::vec3(CAMERA_MOVEMENT, 0.0f, 0.0f));
    }
    if (m_game->inputManager.IsKeyDown(SDLK_w))
    {
      m_camera.SetPosition(m_camera.GetPosition() + glm::vec3(0.0f, CAMERA_MOVEMENT, 0.0f));
    }
    if (m_game->inputManager.IsKeyDown(SDLK_s))
    {
      m_camera.SetPosition(m_camera.GetPosition() + glm::vec3(0.0f, -CAMERA_MOVEMENT, 0.0f));
    }
    if (m_game->inputManager.IsKeyDown(SDLK_q))
    {
      m_camera.SetPosition(m_camera.GetPosition() + glm::vec3(0.0f, 0.0f, CAMERA_MOVEMENT));
    }
    if (m_game->inputManager.IsKeyDown(SDLK_e))
    {
      m_camera.SetPosition(m_camera.GetPosition() + glm::vec3(0.0f, 0.0f, -CAMERA_MOVEMENT));
    }
    int x, y;
    SDL_GetMouseState(&x, &y);
    printf("x: %d, y: %d \n", x, y);
  }
}
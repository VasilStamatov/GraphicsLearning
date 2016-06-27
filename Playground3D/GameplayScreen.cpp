#include "GameplayScreen.h"
#include "ScreenIndices.h"
#include <GameEngine\IMainGame.h>
#include <GameEngine\ResourceManager.h>
#include <glm/gtc/type_ptr.hpp>

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
  m_camera.Init(45.0f, m_window->GetScreenWidth(), m_window->GetScreenHeight(), SDL_TRUE);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  /*glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
  glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);*/
  //glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_REPLACE);
  //glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_KEEP);
  /*glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);*/

  std::vector<GameEngine::Shader> lightShaders =
  {
    { GL_VERTEX_SHADER, "Shaders/Advanced.vert", "Lighting Vertex Shader" },
    { GL_FRAGMENT_SHADER, "Shaders/Advanced.frag", "Lighting Fragment Shader" },
  };
  m_lightProgram.CompileShaders(lightShaders);
  m_lightProgram.AddAttribute("vertexPosition_modelSpace");
  m_lightProgram.AddAttribute("UV");
  m_lightProgram.LinkShaders();

  std::vector<GameEngine::Shader> skyboxShader =
  {
    { GL_VERTEX_SHADER, "Shaders/Skybox.vert", "Simple Vertex Shader" },
    { GL_FRAGMENT_SHADER, "Shaders/Skybox.frag", "Single Fragment Shader" },
  };

  m_skyboxShader.CompileShaders(skyboxShader);
  m_skyboxShader.AddAttribute("position");
  m_skyboxShader.LinkShaders();

#pragma region "object_initialization"

  GLfloat skyboxVertices[] = {
    // Positions          
    -1.0f, 1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    -1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f
  };


  //Setup the skybox ----------
  glGenVertexArrays(1, &skyboxVAO);
  glGenBuffers(1, &skyboxVBO);

  glBindVertexArray(skyboxVAO);
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

  glBindVertexArray(0);

  std::vector<std::string> skyboxFaces;
  skyboxFaces.push_back("Assets/Skybox/right.png");
  skyboxFaces.push_back("Assets/Skybox/left.png");
  skyboxFaces.push_back("Assets/Skybox/top.png");
  skyboxFaces.push_back("Assets/Skybox/bottom.png");
  skyboxFaces.push_back("Assets/Skybox/back.png");
  skyboxFaces.push_back("Assets/Skybox/front.png");

  m_skybox = GameEngine::ResourceManager::GetCubemap(skyboxFaces);
  
  m_playerModel.LoadModel("Assets/nanosuit/nanosuit.obj");
#pragma endregion

  //Draw as wireframe
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void GameplayScreen::OnExit()
{
  // Clean up
  glDeleteFramebuffers(1, &frameBuffer);
}

void GameplayScreen::Update()
{
  m_camera.Update();

  CheckInput();
}
void GameplayScreen::Draw()
{
  // Clear buffers
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_lightProgram.Use();

  glm::mat4 model(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, 1.75f, 1.0f));
  model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
  glUniformMatrix4fv(m_lightProgram.GetUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(m_lightProgram.GetUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(m_camera.GetViewMatrix()));
  glUniformMatrix4fv(m_lightProgram.GetUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(m_camera.GetProjectionMatrix()));

  glUniform3f(m_lightProgram.GetUniformLocation("cameraPos"), m_camera.GetPosition().x, m_camera.GetPosition().y, m_camera.GetPosition().z);

  glActiveTexture(GL_TEXTURE3); /* We already have 3 texture units active (in this shader)
                                so set the skybox as the 4th texture unit (texture units are 0 based so index number 3) */
  glUniform1i(m_lightProgram.GetUniformLocation("skybox"), 3);

  //Draw the model
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_skybox.id);
  m_playerModel.Draw(m_lightProgram);

  m_lightProgram.UnUse();

  //Render the skybox
  glDepthFunc(GL_LEQUAL);

  m_skyboxShader.Use();

  //Set view and proj matrix
  GLint viewID = m_skyboxShader.GetUniformLocation("view");
  GLint projectionID = m_skyboxShader.GetUniformLocation("projection");

  glUniformMatrix4fv(viewID, 1, GL_FALSE, glm::value_ptr(glm::mat4(glm::mat3(m_camera.GetViewMatrix()))));
  glUniformMatrix4fv(projectionID, 1, GL_FALSE, glm::value_ptr(m_camera.GetProjectionMatrix()));

  // skybox cube
  glBindVertexArray(skyboxVAO);
  glActiveTexture(GL_TEXTURE0);
  glUniform1i(m_skyboxShader.GetUniformLocation("skybox"), 0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_skybox.id);

  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
  glDepthFunc(GL_LESS);

  m_skyboxShader.UnUse();
}

void GameplayScreen::CheckInput()
{
  //handle user inputs
  SDL_Event evnt;
  while (SDL_PollEvent(&evnt))
  {
    m_game->OnSDLEvent(evnt);
  }

  if (m_game->inputManager.IsKeyDown(SDLK_ESCAPE))
  {
    m_currentState = GameEngine::ScreenState::EXIT_APPLICATION;
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
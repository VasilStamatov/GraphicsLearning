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
  m_camera.SetPosition(glm::vec3(0.0f, 0.0f, -15.0f));
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  //glEnable(GL_STENCIL_TEST);
  //glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
  //glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
  //glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_REPLACE);
  //glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_KEEP);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  std::vector<GameEngine::Shader> planetShaders =
  {
    { GL_VERTEX_SHADER, "Shaders/Advanced.vert", "Planet Vertex Shader" },
    { GL_FRAGMENT_SHADER, "Shaders/Advanced.frag", "Planet Fragment Shader" },
  };
  m_planetShader.CompileShaders(planetShaders);
  m_planetShader.AddAttribute("position");
  m_planetShader.AddAttribute("normal");
  m_planetShader.AddAttribute("UV");
  m_planetShader.LinkShaders();

  std::vector<GameEngine::Shader> InstancedShaders =
  {
    { GL_VERTEX_SHADER, "Shaders/InstanceShader.vert", "Asteroid Vertex Shader" },
    { GL_FRAGMENT_SHADER, "Shaders/InstanceShader.frag", "Asteroid Fragment Shader" },
  };
  m_asteroidShader.CompileShaders(InstancedShaders);
  m_asteroidShader.AddAttribute("position");
  m_asteroidShader.AddAttribute("normal");
  m_asteroidShader.AddAttribute("UV");
  m_asteroidShader.AddAttribute("modelInstanced");
  m_asteroidShader.LinkShaders();

  std::vector<GameEngine::Shader> skyboxShader =
  {
    { GL_VERTEX_SHADER, "Shaders/Skybox.vert", "Simple Vertex Shader" },
    { GL_FRAGMENT_SHADER, "Shaders/Skybox.frag", "Single Fragment Shader" },
  };

  m_skyboxShader.CompileShaders(skyboxShader);
  m_skyboxShader.AddAttribute("position");
  m_skyboxShader.LinkShaders();

#pragma region "object_initialization"

  // Generate a large list of semi-random model transformation matrices
  const GLuint ASTEROID_COUNT = 10000;
  srand(NULL); // initialize random seed	
  GLfloat radius = 45.0f;
  GLfloat offset = 10.0f;
  for (GLuint i = 0; i < ASTEROID_COUNT; i++)
  {
    glm::mat4 model;
    // 1. Translation: Randomly displace along circle with radius 'radius' in range [-offset, offset]
    GLfloat angle = (GLfloat)i / (GLfloat)ASTEROID_COUNT * 360.0f;
    GLfloat displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
    GLfloat x = sin(angle) * radius + displacement;
    displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
    GLfloat y = -2.5f + displacement * 0.4f; // Keep height of asteroid field smaller compared to width of x and z
    displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
    GLfloat z = cos(angle) * radius + displacement;
    model = glm::translate(model, glm::vec3(x, y, z));

    // 2. Scale: Scale between 0.05 and 0.25f
    GLfloat scale = (rand() % 20) / 100.0f + 0.05;
    model = glm::scale(model, glm::vec3(scale));

    // 3. Rotation: add random rotation around a (semi)randomly picked rotation axis vector
    GLfloat rotAngle = (rand() % 360);
    model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

    // 4. Now add to list of matrices
    m_asteroidMatrices.push_back(model);
  }

  glm::mat4 model(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, -5.0f, 0.0f));
  model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
  m_planetMatrices.push_back(model);

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

  skyboxFaces.push_back("Assets/Skybox/space/sor_cwd/cwd_bk.JPG");
  skyboxFaces.push_back("Assets/Skybox/space/sor_cwd/cwd_dn.JPG");
  skyboxFaces.push_back("Assets/Skybox/space/sor_cwd/cwd_ft.JPG");
  skyboxFaces.push_back("Assets/Skybox/space/sor_cwd/cwd_lf.JPG");
  skyboxFaces.push_back("Assets/Skybox/space/sor_cwd/cwd_rt.JPG");
  skyboxFaces.push_back("Assets/Skybox/space/sor_cwd/cwd_up.JPG");

  m_skybox = GameEngine::ResourceManager::GetCubemap(skyboxFaces);
  
  m_planet = GameEngine::ResourceManager::GetModel("Assets/planet/planet.obj");
  m_asteroids = GameEngine::ResourceManager::GetModel("Assets/rock/rock.obj");


#pragma endregion

  //Draw as wireframe
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void GameplayScreen::OnExit()
{
  // Clean up
  glDeleteFramebuffers(1, &frameBuffer);
  m_planet.Dispose();
  m_asteroids.Dispose();
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

  m_asteroidShader.Use();

  glUniformMatrix4fv(m_planetShader.GetUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(m_camera.GetViewMatrix()));
  glUniformMatrix4fv(m_planetShader.GetUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(m_camera.GetProjectionMatrix()));

  //glUniform3f(m_planetShader.GetUniformLocation("cameraPos"), m_camera.GetPosition().x, m_camera.GetPosition().y, m_camera.GetPosition().z);

  //glActiveTexture(GL_TEXTURE3); /* We already have 3 texture units active (in this shader)
  //                              so set the skybox as the 4th texture unit (texture units are 0 based so index number 3) */
  //glUniform1i(m_planetShader.GetUniformLocation("skybox"), 3);

  //Draw the model
  //glBindTexture(GL_TEXTURE_CUBE_MAP, m_skybox.id);
  m_planet.Draw(m_asteroidShader, m_planetMatrices);

  m_asteroids.Draw(m_asteroidShader, m_asteroidMatrices);

  m_asteroidShader.UnUse();

  m_skyboxShader.Use();

  //Render the skybox
  glDepthFunc(GL_LEQUAL);

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

  if (m_game->inputManager.IsKeyDown(SDLK_1))
  {
    m_window->ChangeWindowType(GameEngine::WindowFlags::FULLSCREEN);
    m_camera.SetScreenDims(m_window->GetScreenWidth(), m_window->GetScreenHeight());
  }
  if (m_game->inputManager.IsKeyDown(SDLK_2))
  {
    m_window->ChangeWindowType(GameEngine::WindowFlags::BORDERLESS);
    m_camera.SetScreenDims(m_window->GetScreenWidth(), m_window->GetScreenHeight());
  }
  if (m_game->inputManager.IsKeyDown(SDLK_3))
  {
    m_window->ChangeWindowType(GameEngine::WindowFlags::WINDOWED);
    m_camera.SetScreenDims(m_window->GetScreenWidth(), m_window->GetScreenHeight());
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
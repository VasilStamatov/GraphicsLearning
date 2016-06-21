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

  std::vector<GameEngine::Shader> lightShaders =
  {
    { GL_VERTEX_SHADER, "Shaders/Lighting.vert", "Lighting Vertex Shader" },
    { GL_FRAGMENT_SHADER, "Shaders/Lighting.frag", "Lighting Fragment Shader" },
  };

  std::vector<GameEngine::Shader> lampShaders =
  {
    { GL_VERTEX_SHADER, "Shaders/Lamp.vert", "Lamp Vertex Shader" },
    { GL_FRAGMENT_SHADER, "Shaders/Lamp.frag", "Lamp Fragment Shader" },
  };

  m_lightProgram.CompileShaders(lightShaders);
  m_lightProgram.AddAttribute("vertexPosition_modelSpace");
  m_lightProgram.AddAttribute("normal");
  m_lightProgram.AddAttribute("vertexUV");
  m_lightProgram.LinkShaders();

  m_lampProgram.CompileShaders(lampShaders);
  m_lampProgram.AddAttribute("vertexPosition_modelSpace");
  m_lampProgram.LinkShaders();

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  gameModel.LoadModel("Assets/nanosuit/nanosuit.obj");

  // Set up vertex data (and buffer(s)) and attribute pointers
  //GLfloat vertices[] = 
  //{
  //  // Positions          // Normals           // Texture Coords
  //  -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
  //  0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
  //  0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
  //  0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
  //  -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
  //  -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

  //  -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
  //  0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
  //  0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
  //  0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
  //  -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
  //  -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

  //  -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
  //  -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
  //  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
  //  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
  //  -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
  //  -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

  //  0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
  //  0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
  //  0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
  //  0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
  //  0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
  //  0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

  //  -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
  //  0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
  //  0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
  //  0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
  //  -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
  //  -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

  //  -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
  //  0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
  //  0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
  //  0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
  //  -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
  //  -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
  //};

  //cubePositions = {
  //  glm::vec3(0.0f, 0.0f, 0.0f),
  //  glm::vec3(2.0f, 5.0f, -15.0f),
  //  glm::vec3(-1.5f, -2.2f, -2.5f),
  //  glm::vec3(-3.8f, -2.0f, -12.3f),
  //  glm::vec3(2.4f, -0.4f, -3.5f),
  //  glm::vec3(-1.7f, 3.0f, -7.5f),
  //  glm::vec3(1.3f, -2.0f, -2.5f),
  //  glm::vec3(1.5f, 2.0f, -2.5f),
  //  glm::vec3(1.5f, 0.2f, -1.5f),
  //  glm::vec3(-1.3f, 1.0f, -1.5f)
  //};

  m_pointLights.emplace_back(glm::vec3(-1.7f, 0.2f, 2.0f),
    glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f),
    1.0f, 0.09f, 0.032f);
  m_pointLights.emplace_back(glm::vec3(2.3f, -3.3f, -4.0f)
    , glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f),
    1.0f, 0.09f, 0.032f);
  m_pointLights.emplace_back(glm::vec3(-4.0f, 2.0f, -12.0f)
    , glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f),
    1.0f, 0.09f, 0.032f);
  m_pointLights.emplace_back(glm::vec3(0.0f, 0.0f, -3.0f)
    , glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f),
    1.0f, 0.09f, 0.032f);

  m_flashLight.Init(m_camera.GetPosition(), m_camera.GetDirection(),
    glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f),
    1.0f, 0.09f, 0.032f, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.5f)));

  m_directionalLight.Init(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(0.5f, 0.5f, 0.5f));

  //glGenVertexArrays(1, &m_containerVAO);

  //glGenBuffers(1, &m_VBO);
  //glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  //glBindVertexArray(m_containerVAO);
  //// Position attribute
  //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
  //glEnableVertexAttribArray(0);

  //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
  //glEnableVertexAttribArray(1);

  //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
  //glEnableVertexAttribArray(2);

  //glBindVertexArray(0);

  // Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
  //glGenVertexArrays(1, &m_lightVAO);
  //glBindVertexArray(m_lightVAO);
  //// We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
  //glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  //// Set the vertex attributes (only position data for the lamp))
  //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
  //glEnableVertexAttribArray(0);
  //glBindVertexArray(0);

  //m_diffuseMap = GameEngine::ResourceManager::GetTexture("Assets/container2.png");
  //m_specularMap = GameEngine::ResourceManager::GetTexture("Assets/container2_specular.png");
  
  // Set texture units
  /*m_lightProgram.Use();

  GLint matDiffuseLoc = m_lightProgram.GetUniformLocation("material.diffuseMap");
  GLint matSpecularLoc = m_lightProgram.GetUniformLocation("material.specularMap");

  glUniform1i(matDiffuseLoc, 0);
  glUniform1i(matSpecularLoc, 1);

  m_lightProgram.UnUse();*/

  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  // glDepthFunc(GL_LESS);
  // Cull triangles which normal is not towards the camera
  // glEnable(GL_CULL_FACE);
}

void GameplayScreen::OnExit()
{

}

void GameplayScreen::Update()
{
  m_camera.Update();
  m_flashLight.SetPosition(m_camera.GetPosition());
  m_flashLight.SetDirection(m_camera.GetDirection());

  CheckInput();
}
void GameplayScreen::Draw()
{
  // Clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_lightProgram.Use();

  /*m_lightPos.x = 1.0f + sin(SDL_GetTicks() / 1000.0f );
  m_lightPos.y = sin(SDL_GetTicks() / 1000.0f);*/

  GLint viewPosLoc = m_lightProgram.GetUniformLocation("viewPos");
  glUniform3f(viewPosLoc, m_camera.GetPosition().x, m_camera.GetPosition().y, m_camera.GetPosition().z);
  //Get material locations
  GLint matShineLoc = m_lightProgram.GetUniformLocation("material.shininess");
  //Set material properties
  glUniform1f(matShineLoc, 32.0f);

  //Directional Lights
  m_directionalLight.UploadToShader(m_lightProgram, "dirLight");

  //Point lights
  for (GLint i = 0; i < m_pointLights.size(); i++)
  {
    std::string number = std::to_string(i);

    m_pointLights.at(i).UploadToShader(m_lightProgram, "pointLights[" + number + "]");
  }

  //Spot Light (flash light)
  m_flashLight.UploadToShader(m_lightProgram, "spotLight");

  GLint modelMatrixID = m_lightProgram.GetUniformLocation("modelMatrix");
  GLint viewMatrixID = m_lightProgram.GetUniformLocation("viewMatrix");
  GLint projMatrixID = m_lightProgram.GetUniformLocation("projMatrix");

  glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, glm::value_ptr(m_camera.GetViewMatrix()));
  glUniformMatrix4fv(projMatrixID, 1, GL_FALSE, glm::value_ptr(m_camera.GetProjectionMatrix()));

  glm::mat4 model(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
  model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

  glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, glm::value_ptr(model));

  gameModel.Draw(m_lightProgram);

  //// Bind diffuse map
  //glActiveTexture(GL_TEXTURE0);
  //glBindTexture(GL_TEXTURE_2D, m_diffuseMap.id);
  //// Bind specular map
  //glActiveTexture(GL_TEXTURE1);
  //glBindTexture(GL_TEXTURE_2D, m_specularMap.id);

  // Draw the container (using container's vertex attributes)
  //glBindVertexArray(m_containerVAO);

  /*for (GLuint i = 0; i < 10; i++)
  {
    glm::mat4 modelMatrix(1.0f);
    modelMatrix = glm::translate(modelMatrix, cubePositions[i]);
    GLfloat angle = 20.0f * i;
    modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(1.0f, 0.3f, 0.5f));

    glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    glDrawArrays(GL_TRIANGLES, 0, 36);
  }*/
  
  //glBindVertexArray(0);

  m_lightProgram.UnUse();

  // Also draw the lamp object, again binding the appropriate shader
  m_lampProgram.Use();

  modelMatrixID = m_lampProgram.GetUniformLocation("modelMatrix");
  viewMatrixID = m_lampProgram.GetUniformLocation("viewMatrix");
  projMatrixID = m_lampProgram.GetUniformLocation("projMatrix");

  glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, glm::value_ptr(m_camera.GetViewMatrix()));
  glUniformMatrix4fv(projMatrixID, 1, GL_FALSE, glm::value_ptr(m_camera.GetProjectionMatrix()));

  // We now draw as many light bulbs as we have point lights.
  //glBindVertexArray(m_lightVAO);

  for (GLint i = 0; i < m_pointLights.size(); i++)
  {
    glm::mat4 modelMatrix(1.0f);
    modelMatrix = glm::translate(modelMatrix, m_pointLights.at(i).GetPosition());
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.02f)); //make it smaller
    glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    gameModel.Draw(m_lampProgram);
    //glDrawArrays(GL_TRIANGLES, 0, 36);
  }
  //glBindVertexArray(0);

  m_lampProgram.UnUse();
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
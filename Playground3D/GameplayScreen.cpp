#include "GameplayScreen.h"
#include "ScreenIndices.h"

#include <GameEngine\ResourceManager.h>
#include <GameEngine\IMainGame.h>
#include <iostream>

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
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  //Init the camera
  m_camera = std::make_shared<GameEngine::Camera3D>();
  m_camera->Init(45.0f, m_window->GetScreenWidth(), m_window->GetScreenHeight(), SDL_TRUE);
  //m_camera.SetPosition(glm::vec3(0.0f, 0.0f, -15.0f));

  ////////////Set up all the Shaders
  //m_villagerShader.CompileShaders("Shaders/Animation.vert", "Shaders/Animation.frag");

  //m_asteroidShader.CompileShaders("Shaders/InstanceShader.vert", "Shaders/InstanceShader.frag");

  std::unique_ptr<GameEngine::GLSLProgram> m_skyboxShader = std::make_unique<GameEngine::GLSLProgram>();
  m_skyboxShader->CompileShaders("Shaders/Skybox.vert", "Shaders/Skybox.frag");

  //m_screenShader.CompileShaders("Shaders/SimpleTransform.vert", "Shaders/SingleColor.frag");

  //m_depthShader.CompileShaders("Shaders/SimpleDepth.vert", "Shaders/SimpleDepth.frag");

  //m_shadowShader.CompileShaders("Shaders/ShadowShader.vert", "Shaders/ShadowShader.frag");

  m_lampShader.CompileShaders("Shaders/Lamp.vert", "Shaders/Lamp.frag");

  m_cubemapShader.CompileShaders("Shaders/ShadowMap.vert", "Shaders/ShadowMap.frag", "Shaders/ShadowMap.gs");

  m_pointLightShader.CompileShaders("Shaders/PointLighting.vert", "Shaders/PointLighting.frag");

  /////////Set up the framebuffers for some post-processing
  m_framebuffer.Init(m_window->GetScreenWidth(), m_window->GetScreenHeight());
  m_framebuffer.Bind(GL_FRAMEBUFFER);
  m_framebuffer.AttachTexture2D(false, false, true);
  m_framebuffer.AttachRenderbuffer(true, true, true);
  if (!m_framebuffer.CheckFramebufferStatus())
  {
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
  }
  m_framebuffer.Unbind(GL_FRAMEBUFFER);

  m_intermediateFB.Init(m_window->GetScreenWidth(), m_window->GetScreenHeight());
  m_intermediateFB.Bind(GL_FRAMEBUFFER);
  m_intermediateFB.AttachTexture2D(false, false, false);
  if (!m_intermediateFB.CheckFramebufferStatus())
  {
    std::cout << "ERROR::FRAMEBUFFER:: Intermediate Framebuffer is not complete!" << std::endl;
  }
  m_intermediateFB.Unbind(GL_FRAMEBUFFER);

  m_depthMap.Init(m_window->GetScreenWidth(), m_window->GetScreenHeight());
  m_depthMap.Bind(GL_FRAMEBUFFER);
  m_depthMap.AttachDepthCubemap();
  if (!m_depthMap.CheckFramebufferStatus())
  {
    std::cout << "ERROR::FRAMEBUFFER:: Intermediate Framebuffer is not complete!" << std::endl;
  }
  m_depthMap.Unbind(GL_FRAMEBUFFER);

  //GameEngine::ResourceManager::GetSkinnedModel("Assets/MD5/Bob.md5mesh", &m_villager);
  GameEngine::ResourceManager::GetStaticModel("Assets/Quad/quad2.obj", &m_quad);
  GameEngine::ResourceManager::GetStaticModel("Assets/Box/box.obj", &m_cube);

  m_pointLight.Init(glm::vec3(0.0f, 0.0f, 0.0f), 0.3f, 0.8f, 1.0f, 1.0f, 0.09f, 0.032f);
  m_pointLight.SetColor(glm::vec3(1.0f));
  m_lightCamera.InitForPointLight(90.0f, m_depthMap.GetDepthMapWidth(), m_depthMap.GetDepthMapHeight(), 1.0f, 25.0f, m_pointLight.GetPosition());

  std::unique_ptr<GameEngine::GLCubemap> skyboxCube = std::make_unique<GameEngine::GLCubemap>(GameEngine::ResourceManager::GetCubemap("Assets/Skybox/",
    "right.png",
    "left.png",
    "top.png",
    "bottom.png",
    "back.png",
    "front.png",
    "skybox1"));

  m_skybox.Init(std::move(skyboxCube), std::move(m_skyboxShader), m_camera);

  //Draw as wireframe
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void GameplayScreen::OnExit()
{
  // Clean up
  m_skybox.Dispose();
  m_framebuffer.Destroy();
  m_intermediateFB.Destroy();
  GameEngine::ResourceManager::Clear();
}

void GameplayScreen::Update()
{
  CheckInput();
  SDL_SetWindowTitle(m_window->GetWindow(), std::to_string(m_game->GetDT()).c_str());

  m_camera->Update();
  //m_villager.Update(m_timer.Seconds());
 /* m_flashLight.SetDirection(m_camera.GetDirection());
  m_flashLight.SetPosition(m_camera.GetPosition());*/
}
void GameplayScreen::Draw()
{
  // Move light position over time
  // m_pointLight.SetPosition(glm::vec3(m_pointLight.GetPosition().x, m_pointLight.GetPosition().y, sinf(m_timer.Seconds() * 0.5f) * 3.0f));

  // 1. Render scene to depth cubemap
  m_depthMap.Bind(GL_FRAMEBUFFER);
  m_cubemapShader.Use();
  for (GLuint i = 0; i < 6; ++i)
  {
    m_cubemapShader.UploadValue("shadowMatrices[" + std::to_string(i) + "]", m_lightCamera.GetLightTransforms().at(i));
  }
  m_cubemapShader.UploadValue("farPlane", m_lightCamera.GetFarPlane());
  m_cubemapShader.UploadValue("lightPos", m_pointLight.GetPosition());

  // Room cube
  m_cube.SetScale(glm::vec3(10.0f));
  m_cube.SetPosition(glm::vec3(0.0f));
  m_cube.SetRotation(glm::vec3(0.0f));
  glDisable(GL_CULL_FACE);
  m_cube.Draw(m_cubemapShader);
  glEnable(GL_CULL_FACE);
  //Cubes
  m_cube.SetScale(glm::vec3(1.0f));
  m_cube.SetPosition(glm::vec3(4.0f, -3.5f, 0.0));
  m_cube.Draw(m_cubemapShader);
  m_cube.SetScale(glm::vec3(1.5f));
  m_cube.SetPosition(glm::vec3(2.0f, 3.0f, 1.0));
  m_cube.Draw(m_cubemapShader);
  m_cube.SetScale(glm::vec3(1.0f));
  m_cube.SetPosition(glm::vec3(-3.0f, -1.0f, 0.0));
  m_cube.Draw(m_cubemapShader);
  m_cube.SetScale(glm::vec3(1.0f));
  m_cube.SetPosition(glm::vec3(-1.5f, 1.0f, 1.5));
  m_cube.Draw(m_cubemapShader);
  m_cube.SetScale(glm::vec3(1.5f));
  m_cube.SetPosition(glm::vec3(-1.5f, 2.0f, -3.0));
  m_cube.SetRotation(glm::vec3(60.0f, 0.0f, 60.0f));
  m_cube.Draw(m_cubemapShader);
  m_depthMap.Unbind(GL_FRAMEBUFFER);
  m_cubemapShader.UnUse();

  // 2. Render scene as normal 
  m_pointLightShader.Use();
  m_pointLightShader.UploadValue("projection", m_camera->GetProjectionMatrix());
  m_pointLightShader.UploadValue("view", m_camera->GetViewMatrix());
  m_pointLightShader.UploadValue("cameraPos", m_camera->GetPosition());
  m_pointLightShader.UploadValue("farPlane", m_lightCamera.GetFarPlane());
  m_pointLightShader.UploadValue("shadowMap", 2, m_depthMap.GetCubemap());
  //m_pointLightShader.UploadValue("pointLightPos[0]", m_pointLight.GetPosition());
  m_pointLight.UploadToShader(m_pointLightShader, "pointLights[0]");

  // Room cube
  m_cube.SetScale(glm::vec3(10.0f));
  m_cube.SetPosition(glm::vec3(0.0f));
  m_cube.SetRotation(glm::vec3(0.0f));
  glDisable(GL_CULL_FACE);
  m_pointLightShader.UploadValue("reverseNormals", 1);
  m_cube.Draw(m_pointLightShader);
  m_pointLightShader.UploadValue("reverseNormals", 0);
  glEnable(GL_CULL_FACE);
  //Cubes
  m_cube.SetScale(glm::vec3(1.0f));
  m_cube.SetPosition(glm::vec3(4.0f, -3.5f, 0.0));
  m_cube.Draw(m_pointLightShader);
  m_cube.SetScale(glm::vec3(1.5f));
  m_cube.SetPosition(glm::vec3(2.0f, 3.0f, 1.0));
  m_cube.Draw(m_pointLightShader);
  m_cube.SetScale(glm::vec3(1.0f));
  m_cube.SetPosition(glm::vec3(-3.0f, -1.0f, 0.0));
  m_cube.Draw(m_pointLightShader);
  m_cube.SetScale(glm::vec3(1.0f));
  m_cube.SetPosition(glm::vec3(-1.5f, 1.0f, 1.5));
  m_cube.Draw(m_pointLightShader);
  m_cube.SetScale(glm::vec3(1.5f));
  m_cube.SetPosition(glm::vec3(-1.5f, 2.0f, -3.0));
  m_cube.SetRotation(glm::vec3(60.0f, 0.0f, 60.0f));
  m_cube.Draw(m_pointLightShader);
  m_pointLightShader.UnUse();

  m_lampShader.Use();
  m_lampShader.UploadValue("projection", m_camera->GetProjectionMatrix());
  m_lampShader.UploadValue("view", m_camera->GetViewMatrix());

  m_cube.SetPosition(m_pointLight.GetPosition());
  m_cube.SetRotation(glm::vec3(0.0f));
  m_cube.Draw(m_lampShader);
  m_lampShader.UnUse();

  m_skybox.Render();

  /*m_villagerShader.Use();

  m_villagerShader.UploadValue("projection", m_camera.GetProjectionMatrix());
  m_villagerShader.UploadValue("view", m_camera.GetViewMatrix());
  m_villager.SetPosition(glm::vec3(0.0f, 10.0f, 3.0f));
  m_villager.SetScale(glm::vec3(0.1f, 0.1f, 0.1f));
  m_villager.Draw(m_villagerShader);

  m_villagerShader.UnUse();*/

  //m_screenShader.Use();

  //glUniform1i(m_screenShader.GetUniformLocation("screenTexture"), 0);
  ////m_depthMap.Render();

  //m_screenShader.UnUse();

  //m_framebuffer.Bind(GL_READ_FRAMEBUFFER);
  //m_intermediateFB.Bind(GL_DRAW_FRAMEBUFFER);
  //m_framebuffer.Blit(GL_COLOR_BUFFER_BIT, GL_NEAREST);

  //m_framebuffer.Unbind(GL_FRAMEBUFFER);
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //m_screenShader.Use();

  //m_intermediateFB.Render();

  //m_screenShader.UnUse();
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
    m_camera->SetScreenDims(m_window->GetScreenWidth(), m_window->GetScreenHeight());
  }
  if (m_game->inputManager.IsKeyDown(SDLK_2))
  {
    m_window->ChangeWindowType(GameEngine::WindowFlags::BORDERLESS);
    m_camera->SetScreenDims(m_window->GetScreenWidth(), m_window->GetScreenHeight());
  }
  if (m_game->inputManager.IsKeyDown(SDLK_3))
  {
    m_window->ChangeWindowType(GameEngine::WindowFlags::WINDOWED);
    m_camera->SetScreenDims(m_window->GetScreenWidth(), m_window->GetScreenHeight());
  }
  if (m_game->inputManager.IsKeyDown(SDLK_ESCAPE))
  {
    m_currentState = GameEngine::ScreenState::EXIT_APPLICATION;
  }
  if (m_game->inputManager.IsKeyDown(SDLK_w))
  {
    m_camera->Move(GameEngine::MoveState::FORWARD, m_game->GetDT());
  }
  if (m_game->inputManager.IsKeyDown(SDLK_a))
  {
    m_camera->Move(GameEngine::MoveState::LEFT, m_game->GetDT());
  }
  if (m_game->inputManager.IsKeyDown(SDLK_s))
  {
    m_camera->Move(GameEngine::MoveState::BACKWARD, m_game->GetDT());
  }
  if (m_game->inputManager.IsKeyDown(SDLK_d))
  {
    m_camera->Move(GameEngine::MoveState::RIGHT, m_game->GetDT());
  }
  if (m_game->inputManager.IsKeyDown(SDLK_r))
  {
    m_camera->Move(GameEngine::MoveState::UP, m_game->GetDT());
  }
  if (m_game->inputManager.IsKeyDown(SDLK_f))
  {
    m_camera->Move(GameEngine::MoveState::DOWN, m_game->GetDT());
  }
  if (m_game->inputManager.IsKeyPressed(SDLK_q))
  {
    m_villager.OffsetScale(glm::vec3(0.03f));
  }
  if (m_game->inputManager.IsKeyPressed(SDLK_e))
  {
    m_villager.OffsetRotation(glm::vec3(0.01f));
  }
  if (m_game->inputManager.GetRelativeMouseMotion().x != 0.0f || m_game->inputManager.GetRelativeMouseMotion().y != 0.0f)
  {
    m_camera->Rotate(m_game->inputManager.GetRelativeMouseMotion().x, m_game->inputManager.GetRelativeMouseMotion().y, m_game->GetDT());
    m_game->inputManager.SetRelativeMouseMotion(0.0f, 0.0f);
  }
  if (m_game->inputManager.GetMouseWheelValue().y != 0)
  {
    m_camera->ChangeFoV(5.0f * m_game->inputManager.GetMouseWheelValue().y);
    m_game->inputManager.SetMouseWheel(0.0f, 0.0f);
  }
}
#pragma once

#include <GameEngine/IGameScreen.h>
#include <GameEngine/Window.h>
#include <GameEngine\GLSLProgram.h>
#include <GameEngine\GLTexture.h>
#include <GameEngine\Camera3D.h>
#include <GameEngine\Model.h>
#include <GameEngine\Lights.h>
#include <GameEngine\Random.h>
#include <GameEngine\Framebuffer.h>
#include <GameEngine\Skybox.h>
#include <map>

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
  void CheckInput();

  GameEngine::Framebuffer m_framebuffer;
  GameEngine::Framebuffer m_intermediateFB;

  GameEngine::Skybox m_skybox;

  std::vector<glm::mat4> m_asteroidMatrices;
  std::vector<glm::mat4> m_villagerMatrices;

  //the texturing program for sprites and lights
  GameEngine::GLSLProgram m_villagerShader;
  GameEngine::GLSLProgram m_asteroidShader;
  GameEngine::GLSLProgram m_skyboxShader;
  GameEngine::GLSLProgram m_screenShader;

  //the cameras
  GameEngine::Camera3D m_camera;
  //the pointer to the game window
  GameEngine::Window* m_window;
  //The models
  GameEngine::Model m_villager;
  GameEngine::Model m_asteroids;
  
  float m_startTime;

  GameEngine::Random m_random;
};
#pragma once

#include <GameEngine/IGameScreen.h>
#include <GameEngine/Window.h>
#include <GameEngine\GLSLProgram.h>
#include <GameEngine\GLTexture.h>
#include <GameEngine\Camera3D.h>
#include <GameEngine\Model.h>
#include <GameEngine\Lights.h>

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

  GLuint m_containerVAO;
  GLuint m_lightVAO;

  GLuint m_VBO;

  std::vector<GameEngine::PointLight> m_pointLights;
  GameEngine::SpotLight m_flashLight;
  GameEngine::DirectionalLight m_directionalLight;

  std::vector<glm::vec3> cubePositions;

  GameEngine::GLTexture m_diffuseMap;
  GameEngine::GLTexture m_specularMap;

  //the texturing program for sprites and lights
  GameEngine::GLSLProgram m_lightProgram;
  GameEngine::GLSLProgram m_lampProgram;

  //the cameras
  GameEngine::Camera3D m_camera;
  //the pointer to the game window
  GameEngine::Window* m_window;

  GameEngine::Model gameModel;
};


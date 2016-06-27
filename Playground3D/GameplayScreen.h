#pragma once

#include <GameEngine/IGameScreen.h>
#include <GameEngine/Window.h>
#include <GameEngine\GLSLProgram.h>
#include <GameEngine\GLTexture.h>
#include <GameEngine\Camera3D.h>
#include <GameEngine\Model.h>
#include <GameEngine\Lights.h>
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

  GLuint cubeVAO, cubeVBO;
  GLuint planeVAO, planeVBO;
  GLuint vegetationVAO, vegetationVBO;
  GLuint screenQuadVAO, screenQuadVBO;
  GLuint skyboxVAO, skyboxVBO;

  GLuint frameBuffer, texColorBuffer, RBO;


  std::vector<glm::vec3> m_vegetation;
  std::map<float, glm::vec3> sorted;

  GameEngine::GLTexture m_cubeTexture;
  GameEngine::GLTexture m_floorTexture;
  GameEngine::GLTexture m_grassTexture;
  GameEngine::GLCubemap m_skybox;

  //the texturing program for sprites and lights
  GameEngine::GLSLProgram m_lightProgram;
  GameEngine::GLSLProgram m_outlineColor;
  GameEngine::GLSLProgram m_skyboxShader;

  //the cameras
  GameEngine::Camera3D m_camera;
  //the pointer to the game window
  GameEngine::Window* m_window;
  GameEngine::Model m_playerModel;
};


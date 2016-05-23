#pragma once

#include <GameEngine/IGameScreen.h>
#include <GameEngine/Window.h>
#include <GameEngine\GLSLProgram.h>
#include <GameEngine\GLTexture.h>
#include <GameEngine\Camera3D.h>

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

  GLuint VertexArrayID;
  GLuint vertexBuffer;
  GLuint uvBuffer;

  GameEngine::GLTexture m_texture;
  //the texturing program for sprites and lights
  GameEngine::GLSLProgram m_shaderProgram;
  //the cameras
  GameEngine::Camera3D m_camera;
  //the pointer to the game window
  GameEngine::Window* m_window;
};


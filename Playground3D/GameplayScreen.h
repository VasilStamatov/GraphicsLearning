#pragma once

#include <GameEngine/IGameScreen.h>
#include <GameEngine/Window.h>
#include <GameEngine\GLSLProgram.h>
#include <GameEngine\Camera3D.h>
#include <GameEngine\Model.h>
#include <GameEngine\Lights.h>
#include <GameEngine\Random.h>
#include <GameEngine\Skybox.h>
#include <GameEngine\DepthMapFBO.h>
#include <GameEngine\Timing.h>
#include <GameEngine\LightCamera.h>
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

		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		GLfloat near_plane = 1.0f, far_plane = 7.5f;
		glm::vec3 lightPos{ -2.0f, 4.0f, -1.0f };

		GameEngine::PointLight m_pointLight;
		GameEngine::LightCamera m_lightCamera;

		GameEngine::DepthMapFBO m_depthMap;
		GameEngine::Framebuffer m_framebuffer;
		GameEngine::Framebuffer m_intermediateFB;

		GameEngine::Skybox m_skybox;

		//the texturing program for sprites and lights
		GameEngine::GLSLProgram m_villagerShader;
		GameEngine::GLSLProgram m_asteroidShader;
		GameEngine::GLSLProgram m_screenShader;
		GameEngine::GLSLProgram m_depthShader;
		GameEngine::GLSLProgram m_shadowShader;
		GameEngine::GLSLProgram m_lampShader;
		GameEngine::GLSLProgram m_cubemapShader;
		GameEngine::GLSLProgram m_pointLightShader;

		GameEngine::SkinnedModel m_villager;
		GameEngine::StaticModel m_quad;
		GameEngine::StaticModel m_cube;

		GameEngine::HRTimer m_timer;

		//the cameras
		std::shared_ptr<GameEngine::Camera3D> m_camera;

		//the pointer to the game window
		GameEngine::Window* m_window;

		GameEngine::Random m_random;
};
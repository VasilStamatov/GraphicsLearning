#include "GameScreen.h"
#include "ScreenIndices.h"

#include <GameEngine\IMainGame.h>
#include <GameEngine\ResourceManager.h>
#include <iostream>

GameScreen::GameScreen(GameEngine::Window * _window) : m_window(_window)
{
		m_screenIndex = SCREEN_INDEX_GAMEPLAY;
}

GameScreen::~GameScreen()
{
}

int GameScreen::GetNextScreenIndex() const
{
		return SCREEN_INDEX_NO_SCREEN;
}

int GameScreen::GetPreviousScreenIndex() const
{
		return SCREEN_INDEX_NO_SCREEN;
}

void GameScreen::Build()
{
}

void GameScreen::Destroy()
{
}

void GameScreen::OnEntry()
{
		/* Initialize the camera */
		m_camera.Init(m_window->GetScreenWidth(), m_window->GetScreenHeight());

		/* Initialize the spritebatch */
		m_spriteBatch.Init();

		/* Initialize the first level */
		m_levels.push_back(std::make_unique<Level>("Levels/level1.txt"));
		m_currentLevel = 0;

		/* Initialize the shaders */
		m_shader.CompileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");

		/* Initialize the player */
		m_player = std::make_shared<Player>(3.0f, 100.0f, m_levels.at(m_currentLevel)->GetStartPlayerPos(),
				&m_game->inputManager, &m_camera, GameEngine::ResourceManager::GetTexture("Textures/player.png"),
				GameEngine::ColorRGBA8(255, 255), m_levels.at(m_currentLevel)->GetLevelGrid());
		// Add the zombies
		const std::vector<glm::vec2>& zombiePositions = m_levels[m_currentLevel]->GetZombieStartPositions();
		for (size_t i = 0; i < zombiePositions.size(); i++)
		{
				m_zombies.push_back(std::make_unique<Zombie>(4.5f, 150.0f, zombiePositions.at(i), GameEngine::ResourceManager::GetTexture("Textures/zombie.png"),
						GameEngine::ColorRGBA8(255, 255), m_levels.at(m_currentLevel)->GetLevelGrid()));
		}
		for (size_t i = 0; i < m_zombies.size(); i++)
		{
				m_timer.Start();
				m_zombies.at(i)->FindPlayer(m_player->GetCenterPos());
				m_timer.Stop();
				std::cout << "Elapsed milli: " << m_timer.Seconds() * 1000 << std::endl;
		}
		pathFinder = std::make_unique<AStar>(m_levels.at(0)->GetLevelGrid());
}

void GameScreen::OnExit()
{
		m_spriteBatch.Dispose();
}

void GameScreen::Update()
{
		if (m_window->WasResized())
		{
				m_camera.Resize(m_window->GetScreenWidth(), m_window->GetScreenHeight());
				m_window->ResizeHandled();
		}
		CheckInput();
		end = m_camera.ConvertScreenToWorld(m_game->inputManager.GetMouseCoords());
		m_player->Update(m_game->GetDT());
		for (size_t i = 0; i < m_zombies.size(); i++)
		{
				m_zombies.at(i)->Update(m_game->GetDT());
		}
		m_camera.SetPosition(m_player->GetCenterPos());
		m_camera.Update();
		SDL_SetWindowTitle(m_window->GetWindow(), std::to_string(m_game->GetFPS()).c_str());
		
}

void GameScreen::Draw()
{
		// Set the base depth to 1.0
		glClearDepth(1.0);
		// Clear the color and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//m_levels.at(m_currentLevel)->GetLevelGrid()->DrawDebug(m_camera.GetCameraMatrix());
		/*if (findPath)
		{
				m_timer.Start();
				std::vector<glm::vec2> path = pathFinder->FindPath(start, end);
				m_timer.Stop();
				std::cout << "Elapsed milli: " << m_timer.Seconds() * 1000 << std::endl;
				m_levels.at(m_currentLevel)->GetLevelGrid()->DrawPath(path, m_camera.GetCameraMatrix());
		}*/
		for (size_t i = 0; i < m_zombies.size(); i++)
		{
				m_levels.at(m_currentLevel)->GetLevelGrid().lock()->DrawPath(m_zombies.at(i)->GetPath(), m_camera.GetCameraMatrix());
		}
		m_shader.Use();

		glActiveTexture(GL_TEXTURE0);

		// Make sure the shader uses texture 0
		GLint textureUniform = m_shader.GetUniformLocation("diffuseTexture");
		glUniform1i(textureUniform, 0);

		// upload the camera matrix
		m_shader.UploadValue("P", m_camera.GetCameraMatrix());

		m_spriteBatch.Begin();

		m_player->Draw(m_spriteBatch);
		for (size_t i = 0; i < m_zombies.size(); i++)
		{
				m_zombies.at(i)->Draw(m_spriteBatch);
		}

		m_spriteBatch.End();
		m_spriteBatch.RenderBatch();

		// Draw the level
		m_levels[m_currentLevel]->Draw();

		m_shader.UnUse();
}

void GameScreen::CheckInput()
{
		SDL_Event evnt;
		while (SDL_PollEvent(&evnt))
		{
				m_game->OnSDLEvent(evnt);
		}

		if (m_game->inputManager.IsKeyDown(SDLK_ESCAPE))
		{
				m_currentState = GameEngine::ScreenState::EXIT_APPLICATION;
		}
		/*if (m_game->inputManager.IsKeyDown(SDLK_w))
		{
				m_camera.OffsetPosition(glm::vec2(0.0f, m_game->GetDT() * 5.0f));
		}
		if (m_game->inputManager.IsKeyDown(SDLK_a))
		{
				m_camera.OffsetPosition(glm::vec2(m_game->GetDT() * -5.0f, 0.0f));
		}
		if (m_game->inputManager.IsKeyDown(SDLK_s))
		{
				m_camera.OffsetPosition(glm::vec2(0.0f, m_game->GetDT() * -5.0f));
		}
		if (m_game->inputManager.IsKeyDown(SDLK_d))
		{
				m_camera.OffsetPosition(glm::vec2(m_game->GetDT() * 5.0f, 0.0f));
		}
		if (m_game->inputManager.IsKeyPressed(SDL_BUTTON_LEFT))
		{
				glm::ivec2 screenCoords;
				SDL_GetMouseState(&screenCoords.x, &screenCoords.y);
				glm::vec2 worldCoords = m_camera.ConvertScreenToWorld(screenCoords);
				start = worldCoords;
				Node* node = m_levels[m_currentLevel]->GetLevelGrid()->GetNodeAt(worldCoords);
				std::cout << "Clicked node info: " << std::endl;
				node->color = GameEngine::ColorRGBA8(0, 0, 255, 255);
				m_camera.SetPosition(node->worldPos);
				if (node->walkable)
				{
						std::cout << "\twalkable: " << "true" << std::endl;
				}
				else
				{
						std::cout << "\twalkable: " << "false" << std::endl;
				}
				std::cout << "\tcoordinate x: " << node->worldPos.x << std::endl;
				std::cout << "\tcoordinate y: " << node->worldPos.y << std::endl;
				std::cout << "\tindex x: " << node->nodeIndex.x << std::endl;
				std::cout << "\tindex y: " << node->nodeIndex.y << std::endl;
				std::cout << "\tworld coordinate x: " << worldCoords.x << std::endl;
				std::cout << "\tworld coordinate y: " << worldCoords.y << std::endl;
		}*/
		if (m_game->inputManager.IsKeyPressed(SDLK_SPACE))
		{
				//findPath = !findPath;
				for (size_t i = 0; i < m_zombies.size(); i++)
				{
						m_timer.Start();
						m_zombies.at(i)->FindPlayer(m_player->GetCenterPos());
						m_timer.Stop();
						std::cout << "Elapsed milli: " << m_timer.Seconds() * 1000 << std::endl;
				}
		}
}

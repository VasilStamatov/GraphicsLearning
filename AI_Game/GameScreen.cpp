#include "GameScreen.h"
#include "ScreenIndices.h"
#include "SmartZombie.h"

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
		m_random.GenSeed(GameEngine::SeedType::CLOCK_TICKS);
		/* Initialize the camera */
		m_camera.Init(m_window->GetScreenWidth(), m_window->GetScreenHeight());
		m_hudCamera.Init(m_window->GetScreenWidth(), m_window->GetScreenHeight());
		m_hudCamera.SetPosition(glm::vec2(m_window->GetScreenWidth() / 2.0f, m_window->GetScreenHeight() / 2.0f));

		/* Initialize the spritebatch */
		m_spriteBatch.Init();
		m_hudSpriteBatch.Init();
		/* Initialize the spritefont */
		m_spriteFont.init("Fonts/chintzy.ttf", 32);

		/* Initialize the shaders */
		m_shader.CompileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");

		/* Initialize the first level */
		m_gameWorlds.push_back(std::make_shared<World>());
		m_gameWorlds.back()->LoadTerrainFromFile("Levels/level1.txt");
		m_currentLevel = 0;
		m_pathRequestManger = std::make_shared<PathRequestManager>(m_gameWorlds.at(m_currentLevel)->GetWorldGrid());

		/* Initialize the player */
		m_player = std::make_shared<Player>(3.0f, 100.0f, m_gameWorlds.at(m_currentLevel)->GetStartPlayerPos(),
				&m_game->inputManager, &m_camera, GameEngine::ResourceManager::GetTexture("Textures/player.png"),
				GameEngine::ColorRGBA8(255, 255), m_gameWorlds.at(m_currentLevel));
		// Add the zombies
		const std::vector<glm::vec2>& zombiePositions = m_gameWorlds.at(m_currentLevel)->GetZombieStartPositions();
		for (size_t i = 0; i < zombiePositions.size(); i++)
		{
				if (m_random.GenRandInt(0, 1) == 0)
				{
						m_zombies.push_back(std::make_unique<Zombie>(4.5f, 150.0f, zombiePositions.at(i),
								GameEngine::ResourceManager::GetTexture("Textures/zombie.png"),
								GameEngine::ColorRGBA8(255, 255),
								m_gameWorlds.at(m_currentLevel),
								m_player, m_pathRequestManger,
								m_gameWorlds.at(m_currentLevel)->GetPatrolWaypoints()));
				}
				else
				{
						m_zombies.push_back(std::make_unique<SmartZombie>(4.5f, 1.0f, zombiePositions.at(i),
								GameEngine::ResourceManager::GetTexture("Textures/zombie.png"),
								GameEngine::ColorRGBA8(0, 255, 0, 255),
								m_gameWorlds.at(m_currentLevel),
								m_player, m_pathRequestManger,
								m_gameWorlds.at(m_currentLevel)->GetPatrolWaypoints()));
				}
		}
		//Generate the seed for rng
}

void GameScreen::OnExit()
{
		m_shader.Dispose();
		m_spriteBatch.Dispose();
		m_hudSpriteBatch.Dispose();
		m_spriteFont.dispose();
		m_zombies.clear();
		m_gameWorlds.clear();
}

void GameScreen::Update()
{
		if (m_window->WasResized())
		{
				m_camera.Resize(m_window->GetScreenWidth(), m_window->GetScreenHeight());
				m_window->ResizeHandled();
		}
		CheckInput();

		float deltaTime = m_game->GetDT();
		//if (deltaTime <= 0.1f)
		{
				deltaTime = 1.0f;
		}

		m_player->Update(deltaTime);
		m_pathRequestManger->Update();
		for (size_t i = 0; i < m_zombies.size(); i++)
		{
				m_zombies.at(i)->Update(deltaTime);
				//collide with all other zombies
				for (size_t j = i + 1; j < m_zombies.size(); j++)
				{
						m_zombies.at(i)->CollideWithAgent(m_zombies.at(j).get());
				}
		}
		m_camera.SetPosition(m_player->GetCenterPos());
		m_camera.Update();
		m_hudCamera.Update();
		SDL_SetWindowTitle(m_window->GetWindow(), std::to_string(m_game->GetFPS()).c_str());
}

void GameScreen::Draw()
{
		// Set the base depth to 1.0
		glClearDepth(1.0);
		// Clear the color and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (m_debugMode)
		{
				m_gameWorlds.at(m_currentLevel)->GetWorldGrid().lock()->DrawGrid(m_camera.GetCameraMatrix());
				for (size_t i = 0; i < m_zombies.size(); i++)
				{
						m_gameWorlds.at(m_currentLevel)->GetWorldGrid().lock()->DrawPath(m_zombies.at(i)->GetPath(), m_camera.GetCameraMatrix());
				}
		}


		m_shader.Use();

		glActiveTexture(GL_TEXTURE0);

		// Make sure the shader uses texture 0
		glUniform1i(m_shader.GetUniformLocation("diffuseTexture"), 0);

		DrawUI(m_shader);

		// upload the camera matrix
		m_shader.UploadValue("projection", m_camera.GetCameraMatrix());

		m_spriteBatch.Begin();

		m_player->Draw(m_spriteBatch);
		for (size_t i = 0; i < m_zombies.size(); i++)
		{
				m_zombies.at(i)->Draw(m_spriteBatch);
		}

		m_spriteBatch.End();
		m_spriteBatch.RenderBatch();

		// Draw the level
		m_gameWorlds.at(m_currentLevel)->Draw();
		m_shader.UnUse();
}

void GameScreen::CheckInput()
{
		SDL_Event evnt;
		while (SDL_PollEvent(&evnt))
		{
				m_game->OnSDLEvent(evnt);
		}
		if (m_game->inputManager.IsKeyPressed(SDLK_F9))
		{
				m_game->SetPause(!m_game->GetPaused());
		}
		if (m_game->inputManager.IsKeyDown(SDLK_ESCAPE))
		{
				m_currentState = GameEngine::ScreenState::EXIT_APPLICATION;
		}
		if (m_game->inputManager.IsKeyPressed(SDLK_F1))
		{
				m_debugMode = !m_debugMode;
		}
		if (m_game->inputManager.IsKeyPressed(SDLK_SPACE))
		{
				int rand = m_random.GenRandInt(0, 6);
				Algorithm algoToUse;
				switch (rand)
				{
				case 0:
				{
						algoToUse = Algorithm::ASTAR;
						m_currentAlgo = "ASTAR";
						break;
				}
				case 1:
				{
						algoToUse = Algorithm::ASTARe;
						m_currentAlgo = "ASTARe";
						break;
				}
				case 2:
				{
						algoToUse = Algorithm::BEST_FIRST;
						m_currentAlgo = "BEST.FIRST";
						break;
				}
				case 3:
				{
						algoToUse = Algorithm::BREADTH_FIRST;
						m_currentAlgo = "BREADTH.FIRST";
						break;
				}
				case 4:
				{
						algoToUse = Algorithm::DEPTH_FIRST;
						m_currentAlgo = "DEPTH.FIRST";
						break;
				}
				case 5:
				{
						algoToUse = Algorithm::DIJKSTRA;
						m_currentAlgo = "DIJKSTRA";
						break;
				}
				case 6:
				{
						algoToUse = Algorithm::GREEDY_BEST_FIRST;
						m_currentAlgo = "GREEDY.BEST.FIRST";
						break;
				}
				default:
				{
						break;
				}
				}
				for (auto& zombie : m_zombies)
				{
						zombie->SetPFAlgo(algoToUse);
				}
		}

		if (m_game->inputManager.IsKeyPressed(SDLK_LSHIFT))
		{
				const std::vector<glm::vec2>& zombiePositions = m_gameWorlds.at(m_currentLevel)->GetZombieStartPositions();
				if (m_random.GenRandInt(0, 1) == 0)
				{
						m_zombies.push_back(std::make_unique<Zombie>(4.5f, 150.0f, zombiePositions.at(m_random.GenRandInt(0, zombiePositions.size() - 1)),
								GameEngine::ResourceManager::GetTexture("Textures/zombie.png"),
								GameEngine::ColorRGBA8(255, 255),
								m_gameWorlds.at(m_currentLevel),
								m_player, m_pathRequestManger,
								m_gameWorlds.at(m_currentLevel)->GetPatrolWaypoints()));
				}
				else
				{
						m_zombies.push_back(std::make_unique<SmartZombie>(4.5f, 1.0f, zombiePositions.at(m_random.GenRandInt(0, zombiePositions.size() - 1)),
								GameEngine::ResourceManager::GetTexture("Textures/zombie.png"),
								GameEngine::ColorRGBA8(0, 255, 0, 255),
								m_gameWorlds.at(m_currentLevel),
								m_player, m_pathRequestManger,
								m_gameWorlds.at(m_currentLevel)->GetPatrolWaypoints()));
				}
		}

		if (m_game->inputManager.IsKeyPressed(SDL_BUTTON_LEFT))
		{
				if (m_debugMode)
				{
						glm::ivec2 screenCoords;
						SDL_GetMouseState(&screenCoords.x, &screenCoords.y);
						glm::vec2 worldCoords = m_camera.ConvertScreenToWorld(screenCoords);
						std::weak_ptr<Node> node = m_gameWorlds.at(m_currentLevel)->GetWorldGrid().lock()->GetNodeAt(worldCoords);
						std::cout << node << std::endl;
				}
		}
}

void GameScreen::DrawUI(GameEngine::GLSLProgram& _shader)
{
		_shader.UploadValue("projection", m_hudCamera.GetCameraMatrix());

		m_hudSpriteBatch.Begin();

		m_spriteFont.draw(m_hudSpriteBatch, m_currentAlgo.c_str(), glm::vec2(1, 1), glm::vec2(1.0f), 0.0, GameEngine::ColorRGBA8(255, 255, 255, 255));

		m_hudSpriteBatch.End();
		m_hudSpriteBatch.RenderBatch();
}

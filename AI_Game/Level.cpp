#include "Level.h"

#include <GameEngine\ResourceManager.h>
#include <GameEngine\GLTexture.h>
#include <fstream>
#include <iostream>

Level::Level(const std::string _filePath)
{
		std::ifstream file;

		file.open(_filePath.c_str());

		if (file.fail())
		{
				throw std::runtime_error("File " + _filePath + " failed to load");
		}

		std::string levelData;

		// Read the level data
		while (std::getline(file, levelData)) 
		{
				m_levelData.emplace_back(levelData);
		}

		m_spriteBatch.Init();
		m_spriteBatch.Begin();

		glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

		std::vector<bool> walkableFlags(m_levelData[0].size() * m_levelData.size());

		//Submit all the tiles to the spritebatch
		for (size_t y = 0; y < m_levelData.size(); y++)
		{
				for (size_t x = 0; x < m_levelData[y].size(); x++)
				{
						// Grab the tile
						char tile = m_levelData[y][x];

						// Get dest rect
						glm::vec4 destRect(x * TILE_WIDTH, y * TILE_WIDTH, TILE_WIDTH, TILE_WIDTH);

						// Process the tile
						switch (tile) 
						{
						case 'B':
						case 'R':
						{
								walkableFlags.at(y * m_levelData[0].size() + x) = false;
								m_spriteBatch.Draw(destRect, //destination rect
																uvRect, //uv rect
																GameEngine::ResourceManager::GetTexture("Textures/red_bricks.png").id, //texture
																0.0f, //depth (layer)
																GameEngine::ColorRGBA8(255, 255)); //color (white)
								break;
						}
						case 'G':
						{
								walkableFlags.at(y * m_levelData[0].size() + x) = false;
								m_spriteBatch.Draw(destRect,
										uvRect,
										GameEngine::ResourceManager::GetTexture("Textures/glass.png").id,
										0.0f,
										GameEngine::ColorRGBA8(255, 255));
								break;
						}
						case 'L':
						{
								walkableFlags.at(y * m_levelData[0].size() + x) = false;
								m_spriteBatch.Draw(destRect,
										uvRect,
										GameEngine::ResourceManager::GetTexture("Textures/light_bricks.png").id,
										0.0f,
										GameEngine::ColorRGBA8(255, 255));
								break;
						}
						case 'w':
						case 'W':
						{
								walkableFlags.at(y * m_levelData[0].size() + x) = false;
								m_spriteBatch.Draw(destRect,
										uvRect,
										GameEngine::ResourceManager::GetTexture("Textures/water.png").id,
										0.0f,
										GameEngine::ColorRGBA8(255, 255));
								break;
						}
						case '@':
						{
								walkableFlags.at(y * m_levelData[0].size() + x) = true;
								m_levelData[y][x] = '.'; /// So we dont collide with a @
								m_spriteBatch.Draw(destRect,
										uvRect,
										GameEngine::ResourceManager::GetTexture("Textures/grass.png").id,
										0.0f,
										GameEngine::ColorRGBA8(255, 255));
								m_startPlayerPos.x = float(x * TILE_WIDTH);
								m_startPlayerPos.y = float(y * TILE_WIDTH);
								break;
						}
						case 'Z':
						{
								walkableFlags.at(y * m_levelData[0].size() + x) = true;
								m_levelData[y][x] = '.'; /// So we dont collide with a Z
								m_spriteBatch.Draw(destRect,
										uvRect,
										GameEngine::ResourceManager::GetTexture("Textures/grass.png").id,
										0.0f,
										GameEngine::ColorRGBA8(255, 255));
								m_zombieSpawnPositions.emplace_back(x * TILE_WIDTH, y * TILE_WIDTH);
								break;
						}
						case '.':
						{
								walkableFlags.at(y * m_levelData[0].size() + x) = true;
								m_spriteBatch.Draw(destRect,
										uvRect,
										GameEngine::ResourceManager::GetTexture("Textures/grass.png").id,
										0.0f,
										GameEngine::ColorRGBA8(255, 255));
								break;
						}
						default:
								std::printf("Unexpected symbol %c at (%d,%d)", tile, x, y);
								break;
						}
				}
		}
		m_spriteBatch.End();

		m_levelGrid = std::make_shared<Grid>(m_levelData[0].size(), m_levelData.size(), TILE_WIDTH, walkableFlags);
}


Level::~Level()
{
}

void Level::Draw()
{
		//render the whole level in the saved spritebatch
		m_spriteBatch.RenderBatch();
}

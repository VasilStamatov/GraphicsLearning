#include "World.h"
#include <fstream>
#include <GameEngine\ResourceManager.h>

World::World() :																				//Terrain(cost, isWater, isWalkable, texture)
		m_grassTerrain(std::make_shared<Terrain>(1, false, true, GameEngine::ResourceManager::GetTexture("Textures/grass.png"))),
		m_redBrickTerrain(std::make_shared<Terrain>(1, false, false, GameEngine::ResourceManager::GetTexture("Textures/red_bricks.png"))),
		m_lightBrickTerrain(std::make_shared<Terrain>(1, false, false, GameEngine::ResourceManager::GetTexture("Textures/light_bricks.png"))),
		m_glassTerrain(std::make_shared<Terrain>(1, false, false, GameEngine::ResourceManager::GetTexture("Textures/glass.png"))),
		m_riverTerrain(std::make_shared<Terrain>(5, true, true, GameEngine::ResourceManager::GetTexture("Textures/water.png")))
{
		m_randomGenerator.GenSeed(GameEngine::SeedType::CLOCK_TICKS);
}
World::~World()
{
		m_terrainBatch.Dispose();
		m_tiles.clear();
		m_zombieSpawnPositions.clear();
}
void World::GenerateRandomTerrain()
{
		m_width = m_randomGenerator.GenRandInt(50, 150);
		m_height = m_randomGenerator.GenRandInt(25, 75);
		m_tiles.resize(m_width * m_height);

		m_worldGrid = std::make_shared<Grid>(m_width, m_height, TILE_WIDTH);

		for (size_t i = 0; i < m_tiles.size(); i++)
		{
				//get the 2d coordinates from the 1d array
				int x = i % m_width;
				int y = i / m_width;

				int randNum = m_randomGenerator.GenRandInt(0, 20);
				// Sprinkle some hills.
				switch (randNum)
				{
				case 0:
				{
						m_tiles.at(i) = m_redBrickTerrain;
						m_worldGrid->SetWalkableAt(glm::ivec2(x, y), m_tiles.at(i).lock()->IsWalkable());
						m_worldGrid->SetTerrainCost(glm::ivec2(x, y), m_tiles.at(i).lock()->MovementCost());
						break;
				}
				case 1:
				{
						m_tiles.at(i) = m_lightBrickTerrain;
						m_worldGrid->SetWalkableAt(glm::ivec2(x, y), m_tiles.at(i).lock()->IsWalkable());
						m_worldGrid->SetTerrainCost(glm::ivec2(x, y), m_tiles.at(i).lock()->MovementCost());
						break;
				}
				case 2:
				{
						m_tiles.at(i) = m_glassTerrain;
						m_worldGrid->SetWalkableAt(glm::ivec2(x, y), m_tiles.at(i).lock()->IsWalkable());
						m_worldGrid->SetTerrainCost(glm::ivec2(x, y), m_tiles.at(i).lock()->MovementCost());
						break;
				}
				default:
				{
						m_tiles.at(i) = m_grassTerrain;
						m_worldGrid->SetWalkableAt(glm::ivec2(x, y), m_tiles.at(i).lock()->IsWalkable());
						m_worldGrid->SetTerrainCost(glm::ivec2(x, y), m_tiles.at(i).lock()->MovementCost());
						break;
				}
				}
		}

		// Lay a river.
		{
				int x = m_randomGenerator.GenRandInt(0, m_width);
				for (int y = 0; y < m_height; y++)
				{
						int index = y * m_width + x;

						m_tiles.at(index) = m_riverTerrain;
						m_worldGrid->SetWalkableAt(glm::ivec2(x, y), m_tiles.at(index).lock()->IsWalkable());
						m_worldGrid->SetTerrainCost(glm::ivec2(x, y), m_tiles.at(index).lock()->MovementCost());
				}
		}

		//set the start player pos
		{
				int index = m_randomGenerator.GenRandInt(0, m_tiles.size());
				while (!m_tiles.at(index).lock()->IsWalkable())
				{
						index = m_randomGenerator.GenRandInt(0, m_tiles.size());
				}
				int x = index % m_width;
				int y = index / m_width;
				m_startPlayerPos = glm::vec2((x * TILE_WIDTH), (y * TILE_WIDTH));
		}

		//set a couple of zombie spawn points
		for (size_t i = 0; i < 3; i++)
		{
				int index = m_randomGenerator.GenRandInt(0, m_tiles.size());
				while (!m_tiles.at(index).lock()->IsWalkable() && index != (m_startPlayerPos.y * m_width + m_startPlayerPos.x))
				{
						index = m_randomGenerator.GenRandInt(0, m_tiles.size());
				}
				int x = index % m_width;
				int y = index / m_width;
				m_zombieSpawnPositions.emplace_back(x * TILE_WIDTH, y * TILE_WIDTH);
		}
}

void World::LoadTerrainFromFile(const std::string & _filePath)
{
		std::ifstream file;

		file.open(_filePath.c_str());

		if (file.fail())
		{
				throw std::runtime_error("File " + _filePath + " failed to load");
		}

		std::vector<std::string> terrainData;
		std::string terrainLine;

		// Read the level data
		while (std::getline(file, terrainLine))
		{
				terrainData.push_back(terrainLine);
		}

		m_width = terrainData.at(0).size();
		m_height = terrainData.size();
		m_tiles.resize(m_width * m_height);

		m_worldGrid = std::make_shared<Grid>(m_width, m_height, TILE_WIDTH);

		//Submit all the tiles to the spritebatch
		for (size_t i = 0; i < m_tiles.size(); i++)
		{
				//get the 2d coordinates from the 1d array
				int x = i % m_width;
				int y = i / m_width;

				// Grab the tile
				char tile = terrainData.at(y).at(x);

				// Process the tile
				switch (tile)
				{
				case 'b':
				case 'B':
				{
						m_tiles.at(i) = m_redBrickTerrain;
						m_worldGrid->SetWalkableAt(glm::ivec2(x, y), m_tiles.at(i).lock()->IsWalkable());
						m_worldGrid->SetTerrainCost(glm::ivec2(x, y), m_tiles.at(i).lock()->MovementCost());
						break;
				}
				case 'g':
				case 'G':
				{
						m_tiles.at(i) = m_glassTerrain;
						m_worldGrid->SetWalkableAt(glm::ivec2(x, y), m_tiles.at(i).lock()->IsWalkable());
						m_worldGrid->SetTerrainCost(glm::ivec2(x, y), m_tiles.at(i).lock()->MovementCost());
						break;
				}
				case 'l':
				case 'L':
				{
						m_tiles.at(i) = m_lightBrickTerrain;
						m_worldGrid->SetWalkableAt(glm::ivec2(x, y), m_tiles.at(i).lock()->IsWalkable());
						m_worldGrid->SetTerrainCost(glm::ivec2(x, y), m_tiles.at(i).lock()->MovementCost());
						break;
				}
				case 'w':
				case 'W':
				{
						m_tiles.at(i) = m_riverTerrain;
						m_worldGrid->SetWalkableAt(glm::ivec2(x, y), m_tiles.at(i).lock()->IsWalkable());
						m_worldGrid->SetTerrainCost(glm::ivec2(x, y), m_tiles.at(i).lock()->MovementCost());
						break;
				}
				case '@':
				{
						m_tiles.at(i) = m_grassTerrain;
						m_worldGrid->SetWalkableAt(glm::ivec2(x, y), m_tiles.at(i).lock()->IsWalkable());
						m_worldGrid->SetTerrainCost(glm::ivec2(x, y), m_tiles.at(i).lock()->MovementCost());
						m_startPlayerPos = glm::vec2((x * TILE_WIDTH), (y * TILE_WIDTH));
						break;
				}
				case 'z':
				case 'Z':
				{
						m_tiles.at(i) = m_grassTerrain;
						m_worldGrid->SetWalkableAt(glm::ivec2(x, y), m_tiles.at(i).lock()->IsWalkable());
						m_worldGrid->SetTerrainCost(glm::ivec2(x, y), m_tiles.at(i).lock()->MovementCost());
						m_zombieSpawnPositions.emplace_back(x * TILE_WIDTH, y * TILE_WIDTH);
						break;
				}
				case 'p':
				case 'P':
				{
						m_tiles.at(i) = m_grassTerrain;
						m_worldGrid->SetWalkableAt(glm::ivec2(x, y), m_tiles.at(i).lock()->IsWalkable());
						m_worldGrid->SetTerrainCost(glm::ivec2(x, y), m_tiles.at(i).lock()->MovementCost());
						m_patrolWaypoints.emplace_back(x * TILE_WIDTH, y * TILE_WIDTH);
						break;
				}
				case '.':
				{
						m_tiles.at(i) = m_grassTerrain;
						m_worldGrid->SetWalkableAt(glm::ivec2(x, y), m_tiles.at(i).lock()->IsWalkable());
						m_worldGrid->SetTerrainCost(glm::ivec2(x, y), m_tiles.at(i).lock()->MovementCost());
						break;
				}
				default:
						std::printf("Unexpected symbol %c at (%d,%d)", tile, x, y);
						break;
				}
		}
}

void World::DrawUnsaved()
{
		//Initialize the spritebatch
		m_terrainBatch.Init();
		//Clear it and set the sort type
		m_terrainBatch.Begin();

		//UV coordinates for all sprites
		glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

		//Sumbit all the sprites to the batch
		for (size_t i = 0; i < m_tiles.size(); i++)
		{
				//get the 2d coordinates from the 1d array
				int x = i % m_width;
				int y = i / m_width;

				// Get dest rect
				glm::vec4 destRect(x * TILE_WIDTH, y * TILE_WIDTH, TILE_WIDTH, TILE_WIDTH);

				m_terrainBatch.Draw(destRect,
						uvRect,
						m_tiles.at(i).lock()->GetTexture().id,
						0.0f,
						GameEngine::ColorRGBA8(255, 255));
		}
		//Sort the sprites and create the batches
		m_terrainBatch.End();

		//Render all the sprite batches
		m_terrainBatch.RenderBatch();

		/* No need to submit the sprites every frame considering the world is unchanging,
				 so save the spritebatch (by not clearing it in Begin() and just use the render call in the future */
		m_savedBatch = true;
}

void World::Draw()
{
		if (!m_savedBatch)
		{
				DrawUnsaved();
		}
		else
		{
				//render the whole world in the saved terrain batch
				m_terrainBatch.RenderBatch();
		}
}

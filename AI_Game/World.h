#pragma once
#include "Terrain.h"
#include "Grid.h"

#include <GameEngine\Random.h>
#include <GameEngine\SpriteBatch.h>

constexpr float TILE_WIDTH = 32.0f;

class World
{
public:
		World();
		~World();

		void GenerateRandomTerrain();
		void LoadTerrainFromFile(const std::string& _filePath);

		void Draw();

		int GetWidth() 																																									const { return m_width; }
		int GetHeight() 																																								const { return m_height; }
		const std::weak_ptr<Terrain> GetTile(int _x, int _y)				const { return m_tiles.at(_y * m_width + _x);}
		const std::weak_ptr<Terrain> GetTile(int _index)								const { return m_tiles.at(_index); }
		const glm::vec2& GetStartPlayerPos() 																			const { return m_startPlayerPos; }
		const std::vector<glm::vec2>& GetZombieStartPositions() const { return m_zombieSpawnPositions; }
		std::weak_ptr<Grid> GetWorldGrid()																						const { return m_worldGrid; }

private:
		void DrawUnsaved();
private:
		int m_width{ 0 };
		int m_height{ 0 };
		bool m_savedBatch{ false };

		std::shared_ptr<Terrain> m_grassTerrain;
		std::shared_ptr<Terrain> m_redBrickTerrain;
		std::shared_ptr<Terrain> m_lightBrickTerrain;
		std::shared_ptr<Terrain> m_glassTerrain;
		std::shared_ptr<Terrain> m_riverTerrain;
		std::shared_ptr<Grid>				m_worldGrid;

		std::vector<std::weak_ptr<Terrain>> m_tiles;
		std::vector<glm::vec2> m_zombieSpawnPositions;
		glm::vec2 m_startPlayerPos;
		GameEngine::Random m_randomGenerator;
		GameEngine::SpriteBatch m_terrainBatch;
};


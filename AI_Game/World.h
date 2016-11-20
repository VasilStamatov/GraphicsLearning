#pragma once
#include "Terrain.h"
#include "Grid.h"

#include <GameEngine\Random.h>
#include <GameEngine\SpriteBatch.h>

constexpr float TILE_WIDTH = 32.0f;

/** \brief World class for the world generation.*/
class World
{
public:
		World();
		~World();
		/** \brief Generates a random terrain */
		void GenerateRandomTerrain();

		/** \brief Loads a premade world from a file */
		void LoadTerrainFromFile(const std::string& _filePath);

		/** \brief Draws the world */
		void Draw();

		/** \brief Getters */
		int GetWidth() 																																									const { return m_width; }
		int GetHeight() 																																								const { return m_height; }
		const std::weak_ptr<Terrain> GetTile(int _x, int _y)				const { return m_tiles.at(_y * m_width + _x);}
		const std::weak_ptr<Terrain> GetTile(int _index)								const { return m_tiles.at(_index); }
		const glm::vec2& GetStartPlayerPos() 																			const { return m_startPlayerPos; }
		const std::vector<glm::vec2>& GetZombieStartPositions() const { return m_zombieSpawnPositions; }
		std::weak_ptr<Grid> GetWorldGrid()																						const { return m_worldGrid; }

private:
		/** \brief Draw the world normally and save it for faster drawing in the future */
		void DrawUnsaved();
private:
		int m_width{ 0 }; ///< width of the world (in tile-space)
		int m_height{ 0 }; ///< height of the world (in tile-space)
		bool m_savedBatch{ false }; ///< flag whether the spritebatch for drawing has been saved

		std::shared_ptr<Terrain> m_grassTerrain;						///< shared pointer for the grass terrain for the Flyweight pattern
		std::shared_ptr<Terrain> m_redBrickTerrain;			///< shared pointer for the red brick terrain for the Flyweight pattern
		std::shared_ptr<Terrain> m_lightBrickTerrain; ///< shared pointer for the light brick terrain for the Flyweight pattern
		std::shared_ptr<Terrain> m_glassTerrain;						///< shared pointer for the glass terrain for the Flyweight pattern
		std::shared_ptr<Terrain> m_riverTerrain;						///< shared pointer for the river terrain for the Flyweight pattern
		std::shared_ptr<Grid>				m_worldGrid;									///< shared pointer for the whole world

		std::vector<std::weak_ptr<Terrain>> m_tiles;   ///< set of weak pointers to the shared terrain tiles (the flyweight pattern)
		std::vector<glm::vec2> m_zombieSpawnPositions; ///< set of world space spawn positions for the zombies
		glm::vec2 m_startPlayerPos;																				///< world space spawn position for the player
		GameEngine::Random m_randomGenerator;										///< random number generator
		GameEngine::SpriteBatch m_terrainBatch;							 ///< the spritebatch for the terrain(world) rendering
};


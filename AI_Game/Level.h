#pragma once

#include <vector>
#include <string>
#include <memory>
#include <GameEngine\SpriteBatch.h>

#include "Grid.h"

constexpr float TILE_WIDTH = 32.0f;

class Level
{
public:
		Level(const std::string _filePath);
		~Level();

		void Draw();

		// Getters
		int GetWidth() 																																									const { return m_levelData[0].size(); }
		int GetHeight() 																																								const { return m_levelData.size(); }
		const std::vector<std::string>& GetLevelData() 									const { return m_levelData; }
		const glm::vec2& GetStartPlayerPos() 																			const { return m_startPlayerPos; }
		const std::vector<glm::vec2>& GetZombieStartPositions() const { return m_zombieSpawnPositions; }
	 std::weak_ptr<Grid> GetLevelGrid()																						const { return m_levelGrid; }

private:
		std::vector<std::string> m_levelData;
		std::shared_ptr<Grid> m_levelGrid;

		GameEngine::SpriteBatch m_spriteBatch;

		glm::vec2 m_startPlayerPos;
		std::vector<glm::vec2> m_zombieSpawnPositions;
};


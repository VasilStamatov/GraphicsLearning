#pragma once

#include <GameEngine\GLTexture.h>

class Terrain
{
public:
		Terrain(int movementCost, bool isWater, bool isWalkable, const GameEngine::GLTexture& texture) :
				m_movementCost(movementCost), m_isWater(isWater), m_isWalkable(isWalkable), m_texture(texture)
		{}

		//Getters
		int MovementCost()																								const noexcept { return m_movementCost; }
		bool IsWater()																												const noexcept { return m_isWater;						}
		bool IsWalkable()																							  const noexcept { return m_isWalkable;			}
		const GameEngine::GLTexture& GetTexture() const noexcept { return m_texture;						}

private:
		int m_movementCost{ 0 };												// The movement cost of the terrain
		bool m_isWater{ false };										 	// Flag whether the terrain is water or not (for special interraction)
		bool m_isWalkable{ true };										// Flag whether units can walk on this terrain
		GameEngine::GLTexture m_texture;				// The texture of this terrain
};
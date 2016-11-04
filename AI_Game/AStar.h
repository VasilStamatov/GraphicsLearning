#pragma once

#include "PathFinder.h"

class AStar : public PathFinder
{
public:
		AStar();
		AStar(std::weak_ptr<Grid> _grid, const Diagonal& _diagonal = Diagonal::IFNOWALLS);
		~AStar();

		virtual std::vector<glm::vec2> FindPath(const glm::vec2& _start, const glm::vec2& _end) override;
		
private:
		int ManhattanDistance(const glm::ivec2& _nodeAIndex, const glm::ivec2& _nodeBIndex);
};
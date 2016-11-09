#pragma once

#include "Grid.h"
#include "Heap.h"

#include <unordered_set>

class PathFinder
{
public:
		PathFinder(const Diagonal& _diagonal = Diagonal::IFNOWALLS);
	 ~PathFinder();

		std::vector<glm::vec2> AStar						 (const glm::vec2& _start, const glm::vec2& _end, std::weak_ptr<Grid> _grid);
	 std::vector<glm::vec2> BestFirst   (const glm::vec2& _start, const glm::vec2& _end, std::weak_ptr<Grid> _grid);
		std::vector<glm::vec2> BreadthFirst(const glm::vec2& _start, const glm::vec2& _end, std::weak_ptr<Grid> _grid);


protected:
		/* Heuristics */
		int ManhattanDistance(const glm::ivec2& _nodeAIndex, const glm::ivec2& _nodeBIndex);
		
		/* Utils */
		std::vector<glm::vec2> Backtrace(std::weak_ptr<Node> _startNode, std::weak_ptr<Node> _endNode);
		std::vector<glm::vec2> CompressPath(std::vector<glm::vec2>& _path);

protected:
		Diagonal m_diagonal;

		std::weak_ptr<Node> m_startNode;
		std::weak_ptr<Node> m_endNode;
		Heap<std::weak_ptr<Node>, std::vector<std::weak_ptr<Node>>, ComparePriority> m_openSet;
		std::unordered_set<std::weak_ptr<Node>, NodeHasher, ComparePriority> m_closedSet;
};


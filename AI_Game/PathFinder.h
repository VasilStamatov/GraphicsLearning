#pragma once

#include "Grid.h"
#include "Heap.h"

#include <functional>
#include <unordered_set>

class PathFinder
{
public:
		PathFinder();
		~PathFinder();

		std::vector<glm::vec2> AStar						 (const glm::vec2& _start, const glm::vec2& _end, std::weak_ptr<Grid> _grid, const Diagonal& _diagonal);
		std::vector<glm::vec2> BestFirst		 (const glm::vec2& _start, const glm::vec2& _end, std::weak_ptr<Grid> _grid, const Diagonal& _diagonal);
		std::vector<glm::vec2> BreadthFirst(const glm::vec2& _start, const glm::vec2& _end, std::weak_ptr<Grid> _grid, const Diagonal& _diagonal);

private:
		/* Heuristics */
		static int ManhattanDistance(const glm::ivec2& _nodeAIndex, const glm::ivec2& _nodeBIndex);
		static int EuclideanDistance(const glm::ivec2& _nodeAIndex, const glm::ivec2& _nodeBIndex);
		static int OctileDistance			(const glm::ivec2& _nodeAIndex, const glm::ivec2& _nodeBIndex);
		static int ChebyshevDistance(const glm::ivec2& _nodeAIndex, const glm::ivec2& _nodeBIndex);

		/* Utils */
		std::vector<glm::vec2> Backtrace   (std::weak_ptr<Node> _startNode, std::weak_ptr<Node> _endNode);
		std::vector<glm::vec2> Interpolate (const glm::vec2& _startCoord, const glm::vec2& _endCoord);
		std::vector<glm::vec2> ExpandPath  (const std::vector<glm::vec2>& _path);
		std::vector<glm::vec2> SmoothenPath(const std::vector<glm::vec2>& _path, std::weak_ptr<Grid> _grid);
		std::vector<glm::vec2> CompressPath(const std::vector<glm::vec2>& _path);
		//std::vector<glm::vec2> BiBacktrace	(std::weak_ptr<Node> _startNode, std::weak_ptr<Node> _endNode);

private:
		std::function<int(const glm::ivec2& _nodeAIndex, const glm::ivec2& _nodeBIndex)> m_heuristic;

		std::weak_ptr<Node> m_startNode;
		std::weak_ptr<Node> m_endNode;
		Heap<std::weak_ptr<Node>, std::vector<std::weak_ptr<Node>>, ComparePriority> m_openSet;
		std::unordered_set<std::weak_ptr<Node>, NodeHasher, ComparePriority> m_closedSet;
		//std::unordered_set<Node, NodeHasher, ComparePriority> m_visited;
};


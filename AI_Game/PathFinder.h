#pragma once

#include "Grid.h"
#include "Heap.h"

#include <functional>
#include <unordered_set>

/** \brief The pathfinder class to contain the algorithms */
class PathFinder
{
public:
		PathFinder();
		~PathFinder();

		/** \brief All pathfinding algorithms this finder supports.
		 *  \param _start				- start position in world space 
			*  \param _end				 	- end position in world space 
			*  \param _grid				 - the grid to be used 
			*  \param _diagonal - state of diagonal allowing */

		std::vector<glm::vec2> AStar						 (const glm::vec2& _start, const glm::vec2& _end, std::weak_ptr<Grid> _grid, const Diagonal& _diagonal);
		std::vector<glm::vec2> AStarEpsilon(const glm::vec2& _start, const glm::vec2& _end, std::weak_ptr<Grid> _grid, const Diagonal& _diagonal);
		std::vector<glm::vec2> BestFirst		 (const glm::vec2& _start, const glm::vec2& _end, std::weak_ptr<Grid> _grid, const Diagonal& _diagonal);
		std::vector<glm::vec2> BreadthFirst(const glm::vec2& _start, const glm::vec2& _end, std::weak_ptr<Grid> _grid, const Diagonal& _diagonal);
		std::vector<glm::vec2> DepthFirst  (const glm::vec2& _start, const glm::vec2& _end, std::weak_ptr<Grid> _grid, const Diagonal& _diagonal);
		std::vector<glm::vec2> Dijkstra    (const glm::vec2& _start, const glm::vec2& _end, std::weak_ptr<Grid> _grid, const Diagonal& _diagonal);
		std::vector<glm::vec2> GreedyBFirst(const glm::vec2& _start, const glm::vec2& _end, std::weak_ptr<Grid> _grid, const Diagonal& _diagonal);

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
		std::vector<glm::vec2> CompressPath(const std::vector<glm::vec2>& _path);
		//std::vector<glm::vec2> SmoothenPath(const std::vector<glm::vec2>& _path, std::weak_ptr<Grid> _grid);
		//std::vector<glm::vec2> BiBacktrace	(std::weak_ptr<Node> _startNode, std::weak_ptr<Node> _endNode);

private:
		/** \brief function pointer to the heuristic function to be used */
		std::function<int(const glm::ivec2& _nodeAIndex, const glm::ivec2& _nodeBIndex)> m_heuristic; 
};


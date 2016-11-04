#pragma once

#include "Grid.h"

#include <functional>

enum class Heuristic : size_t
{
		MANHATTAN,
		EUCLIDEAN,
		OCTILE,
		CHEBYSHEV
};

class PathFinder
{
public:
		PathFinder();
		virtual ~PathFinder();

		virtual std::vector<glm::vec2> FindPath(const glm::vec2& _start, const glm::vec2& _end) = 0;

		void SetGrid(std::weak_ptr<Grid> _grid) { m_grid = _grid; }

protected:
		/* Heuristics */
		//int Manhattan(float _deltaX, float _deltaY);
		//int Manhattan(const glm::vec2& _deltaVec);
		//int Euclidean(float _deltaX, float _deltaY);
		//int Euclidean(const glm::vec2& _deltaVec);
		//int Octile(float _deltaX, float _deltaY);
		//int Octile(const glm::vec2& _deltaVec);
		//int Chebyshev(float _deltaX, float _deltaY);
		//int Chebyshev(const glm::vec2& _deltaVec);

		/* Utils */
		std::vector<glm::vec2> Backtrace(std::weak_ptr<Node> _startNode, std::weak_ptr<Node> _endNode);
		std::vector<glm::vec2> CompressPath(std::vector<glm::vec2>& _path);
		/*std::vector<glm::vec2> BiBacktrace(Node* _nodeA, Node* _nodeB);
		std::vector<glm::vec2> Interpolate(const glm::vec2& _startCoord, const glm::vec2& _endCoord);
		std::vector<glm::vec2> ExpandPath(std::vector<glm::vec2>& _path);
		std::vector<glm::vec2> SmoothenPath(std::vector<glm::vec2>& _path, Grid* _grid);
		float PathLength(std::vector<glm::vec2>& _path);*/

protected:
		std::weak_ptr<Grid> m_grid;
		Diagonal m_diagonal;
		std::function<void(const glm::vec2& _deltaVec)> m_heuristic;
};


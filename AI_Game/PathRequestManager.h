#pragma once
#include <queue>

#include "PathFinder.h"

/** \brief all supported algorithms */
enum class Algorithm : size_t
{
		BEST_FIRST,
		ASTAR,
		ASTARe,
		BREADTH_FIRST,
		DEPTH_FIRST,
		DIJKSTRA,
		GREEDY_BEST_FIRST
};

/** \brief Path request data */
struct PathRequest
{
		PathRequest() {}
		PathRequest(const glm::vec2& _start, const glm::vec2& _end, const Algorithm& _algo,
				const Diagonal& _diagonal, std::function<void(std::vector<glm::vec2>&, bool)> _callback) :
				m_start(_start), m_end(_end), m_algorithm(_algo), m_diagonal(_diagonal), m_callback(_callback) {}

		glm::vec2 m_start;
		glm::vec2 m_end;
		Algorithm m_algorithm;
		Diagonal m_diagonal;
		std::function<void(std::vector<glm::vec2>&, bool)> m_callback;
};

/** \brief Path request manager which finds 1 path per frame */
class PathRequestManager
{
public:
		PathRequestManager();
		PathRequestManager(std::weak_ptr<Grid> _grid);
		~PathRequestManager();

		/** \brief Emplace a path in the queue */
		void RequestPath(const glm::vec2& _start, const glm::vec2& _end, const Algorithm& _algo,
				const Diagonal& _diagonal, std::function<void(std::vector<glm::vec2>&, bool)> _callback);
		/** \brief Emplace a path in the queue */
		void RequestPath(const PathRequest& _request);
		/** \brief Called every frame to find 1 path on the top of the queue */
		void Update();

private:
		std::queue<PathRequest> m_pathRequestQueue; ///< The queue of all path requests

		std::unique_ptr<PathFinder> m_pathFinder;   ///< The pathfinder class to find the paths

		std::weak_ptr<Grid> m_grid; ///< reference (weak pointer) to the grid of the world
};


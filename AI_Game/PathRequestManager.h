#pragma once
#include <queue>

#include "PathFinder.h"

enum class Algorithm : size_t
{
		BEST_FIRST,
		ASTAR,
		ASTARe,
		BREADTH_FIRST
};

//Path request data
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

class PathRequestManager
{
public:
		PathRequestManager();
		PathRequestManager(std::weak_ptr<Grid> _grid);
		~PathRequestManager();

		//Emplace a path in the queue
		void RequestPath(const glm::vec2& _start, const glm::vec2& _end, const Algorithm& _algo,
				const Diagonal& _diagonal, std::function<void(std::vector<glm::vec2>&, bool)> _callback);
		void RequestPath(const PathRequest& _request);
		//Called every frame to find 1 path on the top of the queue
		void Update();

private:
		//The queue of all path requests
		std::queue<PathRequest> m_pathRequestQueue;

		//The pathfinder class to find the paths!
		std::unique_ptr<PathFinder> m_pathFinder;

		//reference (weak pointer) to the grid of the world
		std::weak_ptr<Grid> m_grid;
};


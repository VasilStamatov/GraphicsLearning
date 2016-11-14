#include "PathRequestManager.h"



PathRequestManager::PathRequestManager()
{
		m_pathFinder = std::make_unique<PathFinder>();
}

PathRequestManager::PathRequestManager(std::weak_ptr<Grid> _grid) : m_grid(_grid)
{
		m_pathFinder = std::make_unique<PathFinder>();
}


PathRequestManager::~PathRequestManager()
{
}

void PathRequestManager::RequestPath(const glm::vec2 & _start, const glm::vec2 & _end, const Algorithm & _algo, const Diagonal& _diagonal, std::function<void(std::vector<glm::vec2>&, bool)> _callback)
{
		std::printf("Player pos(apparently): x: %f, y: %f \n", _end.x, _end.y);
		m_pathRequestQueue.emplace(_start, _end, _algo, _diagonal, _callback);
}

void PathRequestManager::RequestPath(const PathRequest & _request)
{
		m_pathRequestQueue.emplace(_request);
}

void PathRequestManager::Update()
{
		if (!m_pathRequestQueue.empty())
		{
				PathRequest currentPathRequest = m_pathRequestQueue.front();
				m_pathRequestQueue.pop();
				switch (currentPathRequest.m_algorithm)
				{
						case Algorithm::BEST_FIRST:
						{
								std::vector<glm::vec2> pathResult = m_pathFinder->BestFirst(currentPathRequest.m_start, currentPathRequest.m_end, m_grid, currentPathRequest.m_diagonal);
								if (pathResult.empty())
								{
										//Failed to find path
										currentPathRequest.m_callback(pathResult, false);
								}
								else
								{
										//Successfully found path
										currentPathRequest.m_callback(pathResult, true);
								}
								break;
						}
						case Algorithm::ASTAR:
						{
								std::vector<glm::vec2> pathResult = m_pathFinder->AStar(currentPathRequest.m_start, currentPathRequest.m_end, m_grid, currentPathRequest.m_diagonal);
								if (pathResult.empty())
								{
										//Failed to find path
										currentPathRequest.m_callback(pathResult, false);
								}
								else
								{
										//Successfully found path
										currentPathRequest.m_callback(pathResult, true);
								}
								break;
						}
						case Algorithm::DIJKSTRA:
						{
								printf("Algorith not implemented");
								break;
						}
						case Algorithm::BREADTH_FIRST:
						{
								std::vector<glm::vec2> pathResult = m_pathFinder->BreadthFirst(currentPathRequest.m_start, currentPathRequest.m_end, m_grid, currentPathRequest.m_diagonal);
								if (pathResult.empty())
								{
										//Failed to find path
										currentPathRequest.m_callback(pathResult, false);
								}
								else
								{
										//Successfully found path
										currentPathRequest.m_callback(pathResult, true);
								}
								break;
						}
						default:
						{
								printf("Algorith not implemented");
								break;
						}
				}
		}
}

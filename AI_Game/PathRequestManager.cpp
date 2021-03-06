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
				std::vector<glm::vec2> pathResult;
				switch (currentPathRequest.m_algorithm)
				{
						case Algorithm::BEST_FIRST:
						{
								pathResult = m_pathFinder->BestFirst(currentPathRequest.m_start, currentPathRequest.m_end, m_grid, currentPathRequest.m_diagonal);
								break;
						}
						case Algorithm::ASTAR:
						{
								pathResult = m_pathFinder->AStar(currentPathRequest.m_start, currentPathRequest.m_end, m_grid, currentPathRequest.m_diagonal);
								break;
						}
						case Algorithm::ASTARe:
						{
								pathResult = m_pathFinder->AStarEpsilon(currentPathRequest.m_start, currentPathRequest.m_end, m_grid, currentPathRequest.m_diagonal);
								break;
						}
						case Algorithm::BREADTH_FIRST:
						{
								pathResult = m_pathFinder->BreadthFirst(currentPathRequest.m_start, currentPathRequest.m_end, m_grid, currentPathRequest.m_diagonal);
								break;
						}
						case Algorithm::DEPTH_FIRST:
						{
								pathResult = m_pathFinder->DepthFirst(currentPathRequest.m_start, currentPathRequest.m_end, m_grid, currentPathRequest.m_diagonal);
								break;
						}
						case Algorithm::DIJKSTRA:
						{
								pathResult = m_pathFinder->Dijkstra(currentPathRequest.m_start, currentPathRequest.m_end, m_grid, currentPathRequest.m_diagonal);
								break;
						}
						case Algorithm::GREEDY_BEST_FIRST:
						{
								pathResult = m_pathFinder->GreedyBFirst(currentPathRequest.m_start, currentPathRequest.m_end, m_grid, currentPathRequest.m_diagonal);
								break;
						}
						default:
						{
								printf("Algorith not implemented");
								break;
						}
				}
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
		}
}

#include "PathFinder.h"

#define EMPTY_VECTOR std::vector<glm::vec2>()
#define MAX_ITERATIONS 50

PathFinder::PathFinder(const Diagonal& _diagonal/* = Diagonal::IFNOWALLS */) : m_diagonal(_diagonal)
{
}


PathFinder::~PathFinder()
{
		m_openSet.Clear();
		m_closedSet.clear();
}

std::vector<glm::vec2> PathFinder::AStar(const glm::vec2 & _start, const glm::vec2 & _end, std::weak_ptr<Grid> _grid)
{
		unsigned char counter = 0;
		_grid.lock()->CleanGrid();
		m_openSet.Clear();
		m_closedSet.clear();
		m_startNode = _grid.lock()->GetNodeAt(_start);
		m_endNode = _grid.lock()->GetNodeAt(_end);
		if (!m_startNode.lock()->walkable || !m_endNode.lock()->walkable)
		{
				return EMPTY_VECTOR;
		}

		m_startNode.lock()->h = ManhattanDistance(m_startNode.lock()->nodeIndex, m_endNode.lock()->nodeIndex);
		m_startNode.lock()->inOpenSet = true;
		m_openSet.Insert(m_startNode);

		while (!m_openSet.IsEmpty())
		{
				std::weak_ptr<Node> currentNode = m_openSet.PopFront();
				currentNode.lock()->inOpenSet = false;

				currentNode.lock()->inClosedSet = true;
				m_closedSet.insert(currentNode);

				//check if the end was found
				if (currentNode.lock() == m_endNode.lock())
				{
						std::vector<glm::vec2> result = Backtrace(m_startNode, m_endNode);
						//result = CompressPath(result);
						return result;
				}

				//get all the walkable neightbors
				for (auto neighbor : _grid.lock()->GetNeighbors(currentNode, m_diagonal))
				{
						//get the g cost of the neighbor 
						int newG = currentNode.lock()->g + ManhattanDistance(currentNode.lock()->nodeIndex, neighbor.lock()->nodeIndex) + neighbor.lock()->terrainCost;

						if (neighbor.lock()->inOpenSet)
						{
								//node already generated, but not expanded
								if (newG < neighbor.lock()->g)
								{
										//new path is cheaper
										neighbor.lock()->g = newG;
										neighbor.lock()->h = ManhattanDistance(neighbor.lock()->nodeIndex, m_endNode.lock()->nodeIndex);
										neighbor.lock()->parent = currentNode;
										//Re-heapify the open set with the updated costs
										m_openSet.UpdateHeap();
								}
						}
						else if (neighbor.lock()->inClosedSet)
						{
								if (newG < neighbor.lock()->g)
								{
										//new path is cheaper
										neighbor.lock()->g = newG;
										neighbor.lock()->h = ManhattanDistance(neighbor.lock()->nodeIndex, m_endNode.lock()->nodeIndex);
										neighbor.lock()->parent = currentNode;
										neighbor.lock()->inClosedSet = false;
										m_closedSet.erase(neighbor);
										neighbor.lock()->inOpenSet = true;
										m_openSet.Insert(neighbor);
								}
						}
						else
						{
								neighbor.lock()->g = newG;
								neighbor.lock()->h = ManhattanDistance(neighbor.lock()->nodeIndex, m_endNode.lock()->nodeIndex);
								neighbor.lock()->parent = currentNode;
								neighbor.lock()->inOpenSet = true;
								m_openSet.Insert(neighbor);
						}
				}
				counter++;
				if (counter > MAX_ITERATIONS)
				{
						std::vector<glm::vec2> result = Backtrace(m_startNode, currentNode);
						//result = CompressPath(result);
						return result;
				}
		}
		return EMPTY_VECTOR;
}

std::vector<glm::vec2> PathFinder::BestFirst(const glm::vec2 & _start, const glm::vec2 & _end, std::weak_ptr<Grid> _grid)
{
		unsigned char counter = 0;
		_grid.lock()->CleanGrid();
		m_openSet.Clear();
		m_closedSet.clear();
		m_startNode = _grid.lock()->GetNodeAt(_start);
		m_endNode = _grid.lock()->GetNodeAt(_end);
		if (!m_startNode.lock()->walkable || !m_endNode.lock()->walkable)
		{
				return EMPTY_VECTOR;
		}

		m_startNode.lock()->h = ManhattanDistance(m_startNode.lock()->nodeIndex, m_endNode.lock()->nodeIndex);
		m_startNode.lock()->inOpenSet = true;
		m_openSet.Insert(m_startNode);

		while (!m_openSet.IsEmpty())
		{
				std::weak_ptr<Node> currentNode = m_openSet.PopFront();
				currentNode.lock()->inOpenSet = false;

				currentNode.lock()->inClosedSet = true;
				m_closedSet.insert(currentNode);

				//check if the end was found
				if (currentNode.lock() == m_endNode.lock())
				{
						std::vector<glm::vec2> result = Backtrace(m_startNode, m_endNode);
						//result = CompressPath(result);
						return result;
				}

				//get all the walkable neightbors
				for (auto neighbor : _grid.lock()->GetNeighbors(currentNode, m_diagonal))
				{
						//check if the neighbor has already been checked (in closed list)
						if (neighbor.lock()->inClosedSet)
						{
								continue;
						}

						int newG = currentNode.lock()->g + ManhattanDistance(currentNode.lock()->nodeIndex, neighbor.lock()->nodeIndex) + neighbor.lock()->terrainCost;

						if (newG < neighbor.lock()->g || !neighbor.lock()->inOpenSet)
						{
								neighbor.lock()->g = newG;
								neighbor.lock()->h = ManhattanDistance(neighbor.lock()->nodeIndex, m_endNode.lock()->nodeIndex);
								neighbor.lock()->parent = currentNode;

								if (!neighbor.lock()->inOpenSet)
								{
										neighbor.lock()->inOpenSet = true;
										//add and sort the newly added neighbor
										m_openSet.Insert(neighbor);
								}
								else
								{
										//Re-heapify the open set with the updated costs
										m_openSet.UpdateHeap();
								}
						}
				}
				counter++;
				if (counter > MAX_ITERATIONS)
				{
						std::vector<glm::vec2> result = Backtrace(m_startNode, currentNode);
						//result = CompressPath(result);
						return result;
				}
		}
		return EMPTY_VECTOR;
}

std::vector<glm::vec2> PathFinder::BreadthFirst(const glm::vec2 & _start, const glm::vec2 & _end, std::weak_ptr<Grid> _grid)
{
		unsigned char counter = 0;
		_grid.lock()->CleanGrid();
		m_openSet.Clear();
		m_closedSet.clear();
		m_startNode = _grid.lock()->GetNodeAt(_start);
		m_endNode = _grid.lock()->GetNodeAt(_end);
		if (!m_startNode.lock()->walkable || !m_endNode.lock()->walkable)
		{
				return EMPTY_VECTOR;
		}
		m_startNode.lock()->inOpenSet = true;
		m_openSet.Insert(m_startNode);

		while (!m_openSet.IsEmpty())
		{
				std::weak_ptr<Node> currentNode = m_openSet.PopFront();
				currentNode.lock()->inOpenSet = false;

				currentNode.lock()->inClosedSet = true;
				m_closedSet.insert(currentNode);
				//check if the end was found
				if (currentNode.lock() == m_endNode.lock())
				{
						std::vector<glm::vec2> result = Backtrace(m_startNode, m_endNode);
						//result = CompressPath(result);
						return result;
				}

				//get all the walkable neightbors
				for (auto neighbor : _grid.lock()->GetNeighbors(currentNode, m_diagonal))
				{
						//check if the neighbor has already been inspected 
						if (neighbor.lock()->inClosedSet || neighbor.lock()->inOpenSet)
						{
								continue;
						}
						neighbor.lock()->inOpenSet = true;
						neighbor.lock()->parent = currentNode;
						m_openSet.Insert(neighbor);
				}
				counter++;
				if (counter > MAX_ITERATIONS)
				{
						std::vector<glm::vec2> result = Backtrace(m_startNode, currentNode);
						//result = CompressPath(result);
						return result;
				}
		}
		return EMPTY_VECTOR;
}

int PathFinder::ManhattanDistance(const glm::ivec2& _nodeAIndex, const glm::ivec2& _nodeBIndex)
{
		int distanceX = abs(_nodeAIndex.x - _nodeBIndex.x);
		int distanceY = abs(_nodeAIndex.y - _nodeBIndex.y);

		if (distanceX > distanceY)
		{
				return 14 * distanceY + 10 * (distanceX - distanceY);
		}
		return 14 * distanceX + 10 * (distanceY - distanceX);
}

std::vector<glm::vec2> PathFinder::Backtrace(std::weak_ptr<Node> _startNode, std::weak_ptr<Node> _endNode)
{
		//create the vector of glm::vec2 coordinates and push the current (last) coordinate
		std::vector<glm::vec2> path;
		std::weak_ptr<Node> currentNode = _endNode;
		while (currentNode.lock() != _startNode.lock())
		{
				path.push_back(currentNode.lock()->worldPos);
				currentNode = currentNode.lock()->parent;
		}
		std::reverse(path.begin(), path.end());
		return path;
}

std::vector<glm::vec2> PathFinder::CompressPath(std::vector<glm::vec2>& _path)
{
		//check if there is anything to compress
		if (_path.size() < 3)
		{
				return _path;
		}

		std::vector<glm::vec2> waypoints(_path.size());
		glm::vec2 directionOld{ 0.0f, 0.0f };

		for (size_t i = 1; i < _path.size(); i++)
		{
				glm::vec2 directionNew{ _path.at(i - 1).x - _path.at(i).x, _path.at(i - 1).y - _path.at(i).y };
				if (directionNew != directionOld)
				{
						waypoints.push_back(_path.at(i));
				}
				directionOld = directionNew;
		}

		return waypoints;
}
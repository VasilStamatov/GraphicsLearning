#include "PathFinder.h"

#define EMPTY_VECTOR std::vector<glm::vec2>()

//Set the max iterations based on compile mode
#ifdef _DEBUG
#define MAX_ITERATIONS 100u
#else
#define MAX_ITERATIONS 450u
#endif

#define EPSILON 1.3f

PathFinder::PathFinder() { }

PathFinder::~PathFinder() { }

std::vector<glm::vec2> PathFinder::AStar(const glm::vec2 & _start, const glm::vec2 & _end, std::weak_ptr<Grid> _grid, const Diagonal& _diagonal)
{
		if (_diagonal == Diagonal::NEVER)
		{
				m_heuristic = ManhattanDistance;
		}
		else
		{
				m_heuristic = OctileDistance;
		}
		unsigned char counter = 0;
		_grid.lock()->CleanGrid();

		std::weak_ptr<Node> m_startNode;
		std::weak_ptr<Node> m_endNode;
		Heap<std::weak_ptr<Node>, std::vector<std::weak_ptr<Node>>, ComparePriority> m_openSet;
		std::unordered_set<std::weak_ptr<Node>, NodeHasher, HashComparator> m_closedSet;

		m_startNode = _grid.lock()->GetNodeAt(_start);
		m_endNode = _grid.lock()->GetNodeAt(_end);
		if (!m_startNode.lock()->walkable || !m_endNode.lock()->walkable)
		{
				return EMPTY_VECTOR;
		}

		m_startNode.lock()->h = m_heuristic(m_startNode.lock()->nodeIndex, m_endNode.lock()->nodeIndex);
		m_startNode.lock()->inOpenSet = true;
		m_openSet.Push(m_startNode);

		while (!m_openSet.IsEmpty())
		{
				std::weak_ptr<Node> currentNode = m_openSet.Front();
				currentNode.lock()->inOpenSet = false;
				m_openSet.Pop();

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
				for (auto neighbor : _grid.lock()->GetNeighbors(currentNode, _diagonal))
				{
						//get the g cost of the neighbor 
						int newG = currentNode.lock()->g + m_heuristic(currentNode.lock()->nodeIndex, neighbor.lock()->nodeIndex) + neighbor.lock()->terrainCost;

						if (neighbor.lock()->inOpenSet)
						{
								//node already generated, but not expanded
								if (newG < neighbor.lock()->g)
								{
										//new path is cheaper
										neighbor.lock()->g = newG;
										neighbor.lock()->h = m_heuristic(neighbor.lock()->nodeIndex, m_endNode.lock()->nodeIndex);
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
										neighbor.lock()->h = m_heuristic(neighbor.lock()->nodeIndex, m_endNode.lock()->nodeIndex);
										neighbor.lock()->parent = currentNode;
										neighbor.lock()->inClosedSet = false;
										m_closedSet.erase(neighbor);
										neighbor.lock()->inOpenSet = true;
										m_openSet.Push(neighbor);
								}
						}
						else
						{
								neighbor.lock()->g = newG;
								neighbor.lock()->h = m_heuristic(neighbor.lock()->nodeIndex, m_endNode.lock()->nodeIndex);
								neighbor.lock()->parent = currentNode;
								neighbor.lock()->inOpenSet = true;
								m_openSet.Push(neighbor);
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

std::vector<glm::vec2> PathFinder::AStarEpsilon(const glm::vec2 & _start, const glm::vec2 & _end, std::weak_ptr<Grid> _grid, const Diagonal & _diagonal)
{
		if (_diagonal == Diagonal::NEVER)
		{
				m_heuristic = ManhattanDistance;
		}
		else
		{
				m_heuristic = OctileDistance;
		}
		unsigned char counter = 0;
		_grid.lock()->CleanGrid();

		std::weak_ptr<Node> m_startNode;
		std::weak_ptr<Node> m_endNode;

		m_startNode = _grid.lock()->GetNodeAt(_start);
		m_endNode = _grid.lock()->GetNodeAt(_end);
		if (!m_startNode.lock()->walkable || !m_endNode.lock()->walkable)
		{
				return EMPTY_VECTOR;
		}

		Heap<std::weak_ptr<Node>, std::vector<std::weak_ptr<Node>>, ComparePriority> m_openSet;
		std::unordered_set<std::weak_ptr<Node>, NodeHasher, HashComparator> m_closedSet;
		Heap<std::weak_ptr<Node>, std::vector<std::weak_ptr<Node>>, SecondaryComparator> m_focal;

		m_startNode.lock()->h = m_heuristic(m_startNode.lock()->nodeIndex, m_endNode.lock()->nodeIndex);
		m_startNode.lock()->inOpenSet = true;
		m_openSet.Push(m_startNode);

		while (!m_openSet.IsEmpty())
		{
				m_focal.Clear();
				for (size_t i = 0; i < m_openSet.Size(); i++)
				{
						//focal = set{currentnode.f() <= (1 + epsilon) * smallest f()}
						if (m_openSet.At(i).lock()->f() <= (EPSILON) * m_openSet.Front().lock()->f())
						{
								m_focal.Push(m_openSet.At(i));
						}
						else
						{
								break;
						}
				}
				//get the best node from the secondary heuristic
				std::weak_ptr<Node> currentNode = m_focal.Front();
				currentNode.lock()->inOpenSet = false;
				
				//pop the current node from the open set
				for (size_t i = 0; i < m_openSet.Size(); i++)
				{
						//use the operator overloaded function which takes a weak ptr as rhs
						if (*currentNode.lock().get() == m_openSet.At(i))
						{
								m_openSet.Remove(i);
								break;
						}
				}

				//set the node to the closed set
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
				for (auto neighbor : _grid.lock()->GetNeighbors(currentNode, _diagonal))
				{
						//get the g cost of the neighbor 
						int newG = currentNode.lock()->g + m_heuristic(currentNode.lock()->nodeIndex, neighbor.lock()->nodeIndex) + neighbor.lock()->terrainCost;

						if (neighbor.lock()->inOpenSet)
						{
								//node already generated, but not expanded
								if (newG < neighbor.lock()->g)
								{
										//new path is cheaper
										neighbor.lock()->g = newG;
										neighbor.lock()->h = m_heuristic(neighbor.lock()->nodeIndex, m_endNode.lock()->nodeIndex);
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
										neighbor.lock()->h = m_heuristic(neighbor.lock()->nodeIndex, m_endNode.lock()->nodeIndex);
										neighbor.lock()->parent = currentNode;
										neighbor.lock()->inClosedSet = false;
										m_closedSet.erase(neighbor);
										neighbor.lock()->inOpenSet = true;
										m_openSet.Push(neighbor);
								}
						}
						else
						{
								neighbor.lock()->g = newG;
								neighbor.lock()->h = m_heuristic(neighbor.lock()->nodeIndex, m_endNode.lock()->nodeIndex);
								neighbor.lock()->parent = currentNode;
								neighbor.lock()->inOpenSet = true;
								m_openSet.Push(neighbor);
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

std::vector<glm::vec2> PathFinder::BestFirst(const glm::vec2 & _start, const glm::vec2 & _end, std::weak_ptr<Grid> _grid, const Diagonal& _diagonal)
{
		if (_diagonal == Diagonal::NEVER)
		{
				m_heuristic = ManhattanDistance;
		}
		else
		{
				m_heuristic = OctileDistance;
		}
		unsigned char counter = 0;
		_grid.lock()->CleanGrid();

		std::weak_ptr<Node> m_startNode;
		std::weak_ptr<Node> m_endNode;
		Heap<std::weak_ptr<Node>, std::vector<std::weak_ptr<Node>>, ComparePriority> m_openSet;
		std::vector<std::weak_ptr<Node>> m_closedSet;

		m_startNode = _grid.lock()->GetNodeAt(_start);
		m_endNode = _grid.lock()->GetNodeAt(_end);
		if (!m_startNode.lock()->walkable || !m_endNode.lock()->walkable)
		{
				return EMPTY_VECTOR;
		}

		m_startNode.lock()->h = m_heuristic(m_startNode.lock()->nodeIndex, m_endNode.lock()->nodeIndex);
		m_startNode.lock()->inOpenSet = true;
		m_openSet.Push(m_startNode);

		while (!m_openSet.IsEmpty())
		{
				std::weak_ptr<Node> currentNode = m_openSet.Front();
				currentNode.lock()->inOpenSet = false;
				m_openSet.Pop();

				currentNode.lock()->inClosedSet = true;
				m_closedSet.push_back(currentNode);

				//check if the end was found
				if (currentNode.lock() == m_endNode.lock())
				{
						std::vector<glm::vec2> result = Backtrace(m_startNode, m_endNode);
						//result = CompressPath(result);
						return result;
				}

				//get all the walkable neightbors
				for (auto neighbor : _grid.lock()->GetNeighbors(currentNode, _diagonal))
				{
						//check if the neighbor has already been checked (in closed list)
						if (neighbor.lock()->inClosedSet || neighbor.lock()->inOpenSet)
						{
								continue;
						}
						int newG = currentNode.lock()->g + m_heuristic(currentNode.lock()->nodeIndex, neighbor.lock()->nodeIndex) + neighbor.lock()->terrainCost;
						neighbor.lock()->g = newG;
						neighbor.lock()->h = m_heuristic(neighbor.lock()->nodeIndex, m_endNode.lock()->nodeIndex);
						neighbor.lock()->parent = currentNode;
						neighbor.lock()->inOpenSet = true;
						m_openSet.Push(neighbor);
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

std::vector<glm::vec2> PathFinder::BreadthFirst(const glm::vec2 & _start, const glm::vec2 & _end, std::weak_ptr<Grid> _grid, const Diagonal& _diagonal)
{
		unsigned char counter = 0;
		_grid.lock()->CleanGrid();

		std::weak_ptr<Node> m_startNode;
		std::weak_ptr<Node> m_endNode;

		m_startNode = _grid.lock()->GetNodeAt(_start);
		m_endNode			= _grid.lock()->GetNodeAt(_end);
		if (!m_startNode.lock()->walkable || !m_endNode.lock()->walkable)
		{
				return EMPTY_VECTOR;
		}
		
		std::queue<std::weak_ptr<Node>> m_openSet;
		std::vector<std::weak_ptr<Node>> m_closedSet;
		m_startNode.lock()->inOpenSet = true;
		m_openSet.push(m_startNode);

		while (!m_openSet.empty())
		{
				std::weak_ptr<Node> currentNode = m_openSet.front();
				m_openSet.pop();
				currentNode.lock()->inOpenSet = false;

				currentNode.lock()->inClosedSet = true;
				m_closedSet.push_back(currentNode);
				//check if the end was found
				if (currentNode.lock() == m_endNode.lock())
				{
						std::vector<glm::vec2> result = Backtrace(m_startNode, m_endNode);
						//result = CompressPath(result);
						return result;
				}

				//get all the walkable neightbors
				for (auto neighbor : _grid.lock()->GetNeighbors(currentNode, _diagonal))
				{
						//check if the neighbor has already been inspected 
						if (neighbor.lock()->inClosedSet || neighbor.lock()->inOpenSet)
						{
								continue;
						}
						neighbor.lock()->inOpenSet = true;
						neighbor.lock()->parent = currentNode;
						m_openSet.push(neighbor);
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

std::vector<glm::vec2> PathFinder::DepthFirst(const glm::vec2 & _start, const glm::vec2 & _end, std::weak_ptr<Grid> _grid, const Diagonal & _diagonal)
{
		unsigned char counter = 0;
		_grid.lock()->CleanGrid();

		std::weak_ptr<Node> m_startNode;
		std::weak_ptr<Node> m_endNode;

		m_startNode = _grid.lock()->GetNodeAt(_start);
		m_endNode = _grid.lock()->GetNodeAt(_end);
		if (!m_startNode.lock()->walkable || !m_endNode.lock()->walkable)
		{
				return EMPTY_VECTOR;
		}

		std::vector<std::weak_ptr<Node>> m_openSet;
		std::vector<std::weak_ptr<Node>> m_closedSet;
		m_startNode.lock()->inOpenSet = true;
		m_openSet.push_back(m_startNode);

		while (!m_openSet.empty())
		{
				std::weak_ptr<Node> currentNode = m_openSet.back();
				m_openSet.pop_back();
				currentNode.lock()->inOpenSet = false;

				currentNode.lock()->inClosedSet = true;
				m_closedSet.push_back(currentNode);
				//check if the end was found
				if (currentNode.lock() == m_endNode.lock())
				{
						std::vector<glm::vec2> result = Backtrace(m_startNode, m_endNode);
						//result = CompressPath(result);
						return result;
				}

				//get all the walkable neightbors
				for (auto neighbor : _grid.lock()->GetNeighbors(currentNode, _diagonal))
				{
						//check if the neighbor has already been inspected 
						if (neighbor.lock()->inClosedSet || neighbor.lock()->inOpenSet)
						{
								continue;
						}
						neighbor.lock()->inOpenSet = true;
						neighbor.lock()->parent = currentNode;
						m_openSet.push_back(neighbor);
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

std::vector<glm::vec2> PathFinder::Dijkstra(const glm::vec2 & _start, const glm::vec2 & _end, std::weak_ptr<Grid> _grid, const Diagonal & _diagonal)
{
		if (_diagonal == Diagonal::NEVER)
		{
				m_heuristic = ManhattanDistance;
		}
		else
		{
				m_heuristic = OctileDistance;
		}
		unsigned char counter = 0;
		_grid.lock()->CleanGrid();

		std::weak_ptr<Node> m_startNode;
		std::weak_ptr<Node> m_endNode;
		Heap<std::weak_ptr<Node>, std::vector<std::weak_ptr<Node>>, ComparePriority> m_openSet;
		std::unordered_set<std::weak_ptr<Node>, NodeHasher, HashComparator> m_closedSet;

		m_startNode = _grid.lock()->GetNodeAt(_start);
		m_endNode = _grid.lock()->GetNodeAt(_end);
		if (!m_startNode.lock()->walkable || !m_endNode.lock()->walkable)
		{
				return EMPTY_VECTOR;
		}

		m_startNode.lock()->inOpenSet = true;
		m_openSet.Push(m_startNode);

		while (!m_openSet.IsEmpty())
		{
				std::weak_ptr<Node> currentNode = m_openSet.Front();
				currentNode.lock()->inOpenSet = false;
				m_openSet.Pop();

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
				for (auto neighbor : _grid.lock()->GetNeighbors(currentNode, _diagonal))
				{
						//get the g cost of the neighbor 
						int newG = currentNode.lock()->g + m_heuristic(currentNode.lock()->nodeIndex, neighbor.lock()->nodeIndex) + neighbor.lock()->terrainCost;

						if (neighbor.lock()->inOpenSet)
						{
								//node already generated, but not expanded
								if (newG < neighbor.lock()->g)
								{
										//new path is cheaper
										neighbor.lock()->g = newG;
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
										neighbor.lock()->parent = currentNode;
										neighbor.lock()->inClosedSet = false;
										m_closedSet.erase(neighbor);
										neighbor.lock()->inOpenSet = true;
										m_openSet.Push(neighbor);
								}
						}
						else
						{
								neighbor.lock()->g = newG;
								neighbor.lock()->parent = currentNode;
								neighbor.lock()->inOpenSet = true;
								m_openSet.Push(neighbor);
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

//Greedy Best-First Search
std::vector<glm::vec2> PathFinder::GreedyBFirst(const glm::vec2 & _start, const glm::vec2 & _end, std::weak_ptr<Grid> _grid, const Diagonal & _diagonal)
{
		if (_diagonal == Diagonal::NEVER)
		{
				m_heuristic = ManhattanDistance;
		}
		else
		{
				m_heuristic = OctileDistance;
		}
		unsigned char counter = 0;
		_grid.lock()->CleanGrid();

		std::weak_ptr<Node> m_startNode;
		std::weak_ptr<Node> m_endNode;
		Heap<std::weak_ptr<Node>, std::vector<std::weak_ptr<Node>>, ComparePriority> m_openSet;
		std::vector<std::weak_ptr<Node>> m_closedSet;

		m_startNode = _grid.lock()->GetNodeAt(_start);
		m_endNode = _grid.lock()->GetNodeAt(_end);
		if (!m_startNode.lock()->walkable || !m_endNode.lock()->walkable)
		{
				return EMPTY_VECTOR;
		}

		m_startNode.lock()->h = m_heuristic(m_startNode.lock()->nodeIndex, m_endNode.lock()->nodeIndex);
		m_startNode.lock()->inOpenSet = true;
		m_openSet.Push(m_startNode);

		while (!m_openSet.IsEmpty())
		{
				std::weak_ptr<Node> currentNode = m_openSet.Front();
				currentNode.lock()->inOpenSet = false;
				m_openSet.Pop();

				currentNode.lock()->inClosedSet = true;
				m_closedSet.push_back(currentNode);

				//check if the end was found
				if (currentNode.lock() == m_endNode.lock())
				{
						std::vector<glm::vec2> result = Backtrace(m_startNode, m_endNode);
						//result = CompressPath(result);
						return result;
				}

				//get all the walkable neightbors
				for (auto neighbor : _grid.lock()->GetNeighbors(currentNode, _diagonal))
				{
						//check if the neighbor has already been checked (in closed list)
						if (neighbor.lock()->inClosedSet || neighbor.lock()->inOpenSet)
						{
								continue;
						}
						neighbor.lock()->h = m_heuristic(neighbor.lock()->nodeIndex, m_endNode.lock()->nodeIndex);
						neighbor.lock()->parent = currentNode;
						neighbor.lock()->inOpenSet = true;
						m_openSet.Push(neighbor);
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

		return 10 * (distanceX + distanceY);
}

int PathFinder::EuclideanDistance(const glm::ivec2 & _nodeAIndex, const glm::ivec2 & _nodeBIndex)
{
		int distanceX = abs(_nodeAIndex.x - _nodeBIndex.x);
		int distanceY = abs(_nodeAIndex.y - _nodeBIndex.y);
		return 10 * (int)(std::sqrtf(distanceX * distanceX + distanceY * distanceY));
}

int PathFinder::OctileDistance(const glm::ivec2 & _nodeAIndex, const glm::ivec2 & _nodeBIndex)
{
		int distanceX = abs(_nodeAIndex.x - _nodeBIndex.x);
		int distanceY = abs(_nodeAIndex.y - _nodeBIndex.y);

		if (distanceX > distanceY)
		{
				return 14 * distanceY + 10 * (distanceX - distanceY);
		}
		return 14 * distanceX + 10 * (distanceY - distanceX);
}

int PathFinder::ChebyshevDistance(const glm::ivec2 & _nodeAIndex, const glm::ivec2 & _nodeBIndex)
{
		int distanceX = abs(_nodeAIndex.x - _nodeBIndex.x);
		int distanceY = abs(_nodeAIndex.y - _nodeBIndex.y);
		return 10 * (distanceX + distanceY) + (10 - 2 * 10) * std::min(distanceX, distanceY);
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
		//std::reverse(path.begin(), path.end());
		return path;
}

std::vector<glm::vec2> PathFinder::Interpolate(const glm::vec2 & _startCoord, const glm::vec2 & _endCoord)
{
		std::vector<glm::vec2> result;
		glm::vec2 distance{ 0.0f, 0.0f };
		glm::vec2 startCoord{ _startCoord }; //current start coordinate
		glm::vec2 progressCoord{ 0.0f, 0.0f };
		float deltaError{ 0.0f };
		float delta2Error{ deltaError * 2.0f };

		distance = glm::vec2(fabsf(_endCoord.x - _startCoord.x), fabsf(_endCoord.y - _startCoord.y));
		progressCoord.x = (_startCoord.x < _endCoord.x) ? 1.0f : -1.0f;
		progressCoord.y = (_startCoord.y < _endCoord.y) ? 1.0f : -1.0f;

		deltaError = distance.x - distance.y;

		while (true)
		{
				result.emplace_back(startCoord.x, startCoord.y);

				if (_startCoord.x == _endCoord.x && _startCoord.y == _endCoord.y)
				{
						break;
				}

				delta2Error = deltaError * 2.0f;
				if (delta2Error > -distance.y)
				{
						deltaError -= distance.y;
						startCoord.x += progressCoord.x;
				}
				if (delta2Error < distance.x)
				{
						deltaError += distance.x;
						startCoord.y += progressCoord.y;
				}
		}
		return result;
}

std::vector<glm::vec2> PathFinder::ExpandPath(const std::vector<glm::vec2>& _path)
{
		std::vector<glm::vec2> expanded;
		std::vector<glm::vec2> interpolated;
		glm::vec2 coord0{ 0.0f, 0.0f }, coord1{ 0.0f, 0.0f };

		if (_path.size() < 2)
		{
				return expanded;
		}

		for (size_t pathIndex = 0; pathIndex < _path.size() - 1; pathIndex++)
		{
				coord0 = _path.at(pathIndex);
				coord1 = _path.at(pathIndex + 1);

				std::vector<glm::vec2> interpolate = Interpolate(coord0, coord1);

				interpolated.insert(interpolated.end(), interpolate.begin(), interpolate.end());

				for (size_t j = 0; j < interpolated.size() - 1; j++)
				{
						expanded.push_back(interpolated.at(j));
				}
		}

		expanded.push_back(_path.at(_path.size() - 1));

		return expanded;
}

std::vector<glm::vec2> PathFinder::CompressPath(const std::vector<glm::vec2>& _path)
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
				glm::vec2 directionNew = glm::normalize(glm::vec2(_path.at(i - 1).x - _path.at(i).x, _path.at(i - 1).y - _path.at(i).y));
				if (directionNew != directionOld)
				{
						waypoints.push_back(_path.at(i));
				}
				directionOld = directionNew;
		}

		return waypoints;
}
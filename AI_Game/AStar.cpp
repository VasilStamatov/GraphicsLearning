#include "AStar.h"
#include "Heap.h"

#include <algorithm>

//test
#include <iostream>

AStar::AStar()
{
}

AStar::AStar(std::weak_ptr<Grid> _grid, const Diagonal& _diagonal/* = Diagonal::IFNOWALLS */)
{
		m_grid = _grid;
		m_diagonal = _diagonal;
}


AStar::~AStar()
{
}

std::vector<glm::vec2> AStar::FindPath(const glm::vec2 & _start, const glm::vec2 & _end)
{
		try
		{
				m_grid.lock()->CleanGrid();
				std::weak_ptr<Node> startNode = m_grid.lock()->GetNodeAt(_start);
				std::weak_ptr<Node> endNode = m_grid.lock()->GetNodeAt(_end);
				if (!startNode.lock()->walkable || !endNode.lock()->walkable)
				{
						return std::vector<glm::vec2>();
				}
				Heap<std::weak_ptr<Node>, std::vector<std::weak_ptr<Node>>, ComparePriority> openSet;

				std::vector<std::weak_ptr<Node>> closedSet;

				startNode.lock()->inOpenSet = true;
				openSet.Insert(startNode);

				while (!openSet.IsEmpty())
				{
						std::weak_ptr<Node> currentNode = openSet.Front();
						currentNode.lock()->inOpenSet = false;
						openSet.PopFront();
						
						currentNode.lock()->inClosedSet = true;
						closedSet.push_back(currentNode);

						//check if the end was found
						if (currentNode.lock() == endNode.lock())
						{
								std::vector<glm::vec2> result = Backtrace(startNode, endNode);
								//result = CompressPath(result);
								return result;
						}

						//get all the walkable neightbors
						for (auto neighbor : m_grid.lock()->GetNeighbors(currentNode, m_diagonal))
						{
								//check if the neighbor has already been checked (in closed list)
								if (neighbor.lock()->inClosedSet)
								{
										continue;
								}

								int newG = currentNode.lock()->g + ManhattanDistance(currentNode.lock()->nodeIndex, neighbor.lock()->nodeIndex);

								if (newG < neighbor.lock()->g || !neighbor.lock()->inOpenSet)
								{
										neighbor.lock()->g = newG;
										neighbor.lock()->h = ManhattanDistance(neighbor.lock()->nodeIndex, endNode.lock()->nodeIndex);
										neighbor.lock()->parent = currentNode;

										if (!neighbor.lock()->inOpenSet)
										{
												neighbor.lock()->inOpenSet = true;
												//add and sort the newly added neighbor
												openSet.Insert(neighbor);
										}
										else
										{
												//Re-heapify the open set with the updated costs
												openSet.UpdateHeap();
										}
								}
						}
				}
		}
		catch (const std::runtime_error& _e)
		{
				std::cout << _e.what() << std::endl;
				return std::vector<glm::vec2>();
		}
		return std::vector<glm::vec2>();
}

int AStar::ManhattanDistance(const glm::ivec2 & _nodeAIndex, const glm::ivec2 & _nodeBIndex)
{
		int distanceX = abs(_nodeAIndex.x - _nodeBIndex.x);
		int distanceY = abs(_nodeAIndex.y - _nodeBIndex.y);

		if (distanceX > distanceY)
		{
				return 14 * distanceY + 10 * (distanceX - distanceY);
		}
		return 14 * distanceX + 10 * (distanceY - distanceX);
}

#include "Grid.h"

Grid::Grid(const Grid & _obj)
{
		m_nodeMap							=	_obj.m_nodeMap;
		m_gridWorldSize = _obj.m_gridWorldSize; 
		m_nodeDiameter  = _obj.m_nodeDiameter;
		m_numXNodes					= _obj.m_numXNodes;
		m_numYNodes					= _obj.m_numYNodes;
}

Grid::Grid()
{
}


Grid::~Grid()
{
		m_nodeMap.clear();
}

Grid::Grid(const glm::vec2& _gridWorldSize, float _nodeDiameter, std::vector<bool>& _walkableMatrix) :
		m_gridWorldSize(_gridWorldSize), m_nodeDiameter(_nodeDiameter)
{
		m_numXNodes = (int)ceil((m_gridWorldSize.x / _nodeDiameter));
		m_numYNodes = (int)ceil((m_gridWorldSize.y / _nodeDiameter));
		CreateGrid(_walkableMatrix);
		m_debugRenderer.Init();
}

Grid::Grid(const glm::vec2& _gridWorldSize, float _nodeDiameter) :
		m_gridWorldSize(_gridWorldSize), m_nodeDiameter(_nodeDiameter)
{
		m_numXNodes = (int)ceil((m_gridWorldSize.x / _nodeDiameter));
		m_numYNodes = (int)ceil((m_gridWorldSize.y / _nodeDiameter));
		CreateGrid();
		m_debugRenderer.Init();
}

Grid::Grid(size_t _numXNodes, size_t _numYNodes, float _nodeDiameter, std::vector<bool>& _walkableMatrix) :
		m_numXNodes(_numXNodes), m_numYNodes(_numYNodes), m_nodeDiameter(_nodeDiameter)
{
		m_gridWorldSize.x = float(m_numXNodes * m_nodeDiameter);
		m_gridWorldSize.y = float(m_numYNodes * m_nodeDiameter);
		CreateGrid(_walkableMatrix);
		m_debugRenderer.Init();
}

Grid::Grid(size_t _numXNodes, size_t _numYNodes, float _nodeDiameter) :
		m_numXNodes(_numXNodes), m_numYNodes(_numYNodes), m_nodeDiameter(_nodeDiameter)
{
		m_gridWorldSize.x = float(m_numXNodes * m_nodeDiameter);
		m_gridWorldSize.y = float(m_numYNodes * m_nodeDiameter);
		CreateGrid();
		m_debugRenderer.Init();
}

std::weak_ptr<Node> Grid::GetNodeAt(const glm::vec2& _worldPos)
{
		//the position on the x coordinate (0 to m_numXNodes)
		float posX = (_worldPos.x / m_nodeDiameter);
		//the position on the y coordinate (0 to m_numYNodes)
		float posY = (_worldPos.y / m_nodeDiameter);

		//Check if the indices are inside the node vector
		if (!IsPosInside(glm::vec2(posX, posY)))
		{
				throw std::runtime_error("Requested node is out of range");
		}

		//cast the indices for the node vector to ints to get the Node
		return GetNodeAt(glm::ivec2(int(posX), int(posY)));
}

std::weak_ptr<Node> Grid::GetNodeAt(const glm::ivec2 & _index)
{
		return m_nodeMap[int(_index.y) * m_numXNodes + int(_index.x)];
}

bool Grid::IsWalkableAt(const glm::vec2& _worldPos)
{
		std::weak_ptr<Node> nodeToCheck = GetNodeAt(_worldPos);
		//make sure the node exists in the node vector
		if (nodeToCheck.expired())
		{
				return false;
		}
		return nodeToCheck.lock()->walkable;
}

bool Grid::IsWalkableAt(const glm::ivec2 & _index)
{
		if (!IsPosInside(_index))
		{
				return false;
		}
		return GetNodeAt(_index).lock()->walkable;
}

bool Grid::IsPosInside(const glm::vec2& _index)
{
		if (_index.x < 0.0f || _index.x >= m_numXNodes)
		{
				return false;
		}
		if (_index.y < 0.0f || _index.y >= m_numYNodes)
		{
				return false;
		}
		return true;
}

void Grid::SetWalkableAt(const glm::vec2& _worldPos, bool _walkable)
{
		GetNodeAt(_worldPos).lock()->walkable = _walkable;
}
void Grid::SetWalkableAt(const glm::ivec2 & _index, bool _walkable)
{
		GetNodeAt(_index).lock()->walkable = _walkable;
}
void Grid::SetTerrainCost(const glm::vec2 & _worldPos, int _cost)
{
		GetNodeAt(_worldPos).lock()->terrainCost = _cost;
}
void Grid::SetTerrainCost(const glm::ivec2 & _index, int _cost)
{
		GetNodeAt(_index).lock()->terrainCost = _cost;
}
std::vector<std::weak_ptr<Node>> Grid::GetNeighbors(std::weak_ptr<Node> _node, const Diagonal & _diagonal)
{
		/**
		* Get the neighbors of the given node.
		*
		*     sides											diagonals:
		*  +---+---+---+    +---+---+---+
		*  |   | 0 |   |    | 0 |   | 1 |
		*  +---+---+---+    +---+---+---+
		*  | 3 |   | 1 |    |   |   |   |
		*  +---+---+---+    +---+---+---+
		*  |   | 2 |   |    | 3 |   | 2 |
		*  +---+---+---+    +---+---+---+
		*/
		std::vector<std::weak_ptr<Node>> neighbors;
		bool side0 = false;
		bool side1 = false;
		bool side2 = false;
		bool side3 = false;

		bool diagonal0 = false;
		bool diagonal1 = false;
		bool diagonal2 = false;
		bool diagonal3 = false;

		//check if the node above is walkable
		if (IsWalkableAt(glm::ivec2(_node.lock()->nodeIndex.x, _node.lock()->nodeIndex.y + 1)))
		{
				//if the node ^above^ is walkable then push it to the neighbors
				neighbors.push_back(GetNodeAt(glm::ivec2(_node.lock()->nodeIndex.x, _node.lock()->nodeIndex.y + 1)));
				side0 = true;
		}
		if (IsWalkableAt(glm::ivec2(_node.lock()->nodeIndex.x + 1, _node.lock()->nodeIndex.y)))
		{
				//if the node >To the right> is walkable then push it to the neighbors
				neighbors.push_back(GetNodeAt(glm::ivec2(_node.lock()->nodeIndex.x + 1, _node.lock()->nodeIndex.y)));
				side1 = true;
		}
		if (IsWalkableAt(glm::ivec2(_node.lock()->nodeIndex.x, _node.lock()->nodeIndex.y - 1)))
		{
				//if the node \/below\/ is walkable then push it to the neighbors
				neighbors.push_back(GetNodeAt(glm::ivec2(_node.lock()->nodeIndex.x, _node.lock()->nodeIndex.y - 1)));
				side2 = true;
		}
		if (IsWalkableAt(glm::ivec2(_node.lock()->nodeIndex.x - 1, _node.lock()->nodeIndex.y)))
		{
				//if the node <to the left< is walkable then push it to the neighbors
				neighbors.push_back(GetNodeAt(glm::ivec2(_node.lock()->nodeIndex.x - 1, _node.lock()->nodeIndex.y)));
				side3 = true;
		}

		//Check the diagonal movement state
		switch (_diagonal)
		{
		case Diagonal::ALWAYS:
		{
				//always allow diagonals
				diagonal0 = true;
				diagonal1 = true;
				diagonal2 = true;
				diagonal3 = true;
				break;
		}
		case Diagonal::NEVER:
		{
				//if it is just return the current walkable neighbors
				return neighbors;
		}
		case Diagonal::IFNOWALLS:
		{
				//diagonals are available if the 2 tiles adjacent to it are walkable
				diagonal0 = side0 && side3;
				diagonal1 = side0 && side1;
				diagonal2 = side1 && side2;
				diagonal3 = side2 && side3;
				break;
		}
		case Diagonal::IFLESSTHANTWOWALLS:
		{
				//diagonals are available if at least 1 of the 2 adjacent tiles are free
				diagonal0 = side0 || side3;
				diagonal1 = side0 || side1;
				diagonal2 = side1 || side2;
				diagonal3 = side2 || side3;
				break;
		}
		default:
				//error
				break;
		}

		//add all the walkable diagonals

		if (diagonal0 && IsWalkableAt(glm::ivec2(_node.lock()->nodeIndex.x - 1, _node.lock()->nodeIndex.y + 1)))
		{
				neighbors.push_back(GetNodeAt(glm::ivec2(_node.lock()->nodeIndex.x - 1, _node.lock()->nodeIndex.y + 1)));
		}
		if (diagonal1 && IsWalkableAt(glm::ivec2(_node.lock()->nodeIndex.x + 1, _node.lock()->nodeIndex.y + 1)))
		{
				neighbors.push_back(GetNodeAt(glm::ivec2(_node.lock()->nodeIndex.x + 1, _node.lock()->nodeIndex.y + 1)));
		}
		if (diagonal2 && IsWalkableAt(glm::ivec2(_node.lock()->nodeIndex.x + 1, _node.lock()->nodeIndex.y - 1)))
		{
				neighbors.push_back(GetNodeAt(glm::ivec2(_node.lock()->nodeIndex.x + 1, _node.lock()->nodeIndex.y - 1)));
		}
		if (diagonal3 && IsWalkableAt(glm::ivec2(_node.lock()->nodeIndex.x - 1, _node.lock()->nodeIndex.y - 1)))
		{
				neighbors.push_back(GetNodeAt(glm::ivec2(_node.lock()->nodeIndex.x - 1, _node.lock()->nodeIndex.y - 1)));
		}

		return neighbors;
}

std::vector<Node> Grid::GetNeighbors(const Node & _node, const Diagonal & _diagonal)
{
		/**
		* Get the neighbors of the given node.
		*
		*     sides											diagonals:
		*  +---+---+---+    +---+---+---+
		*  |   | 0 |   |    | 0 |   | 1 |
		*  +---+---+---+    +---+---+---+
		*  | 3 |   | 1 |    |   |   |   |
		*  +---+---+---+    +---+---+---+
		*  |   | 2 |   |    | 3 |   | 2 |
		*  +---+---+---+    +---+---+---+
		*/
		std::vector<Node> neighbors;
		bool side0 = false;
		bool side1 = false;
		bool side2 = false;
		bool side3 = false;

		bool diagonal0 = false;
		bool diagonal1 = false;
		bool diagonal2 = false;
		bool diagonal3 = false;

		//check if the node above is walkable
		if (IsWalkableAt(glm::ivec2(_node.nodeIndex.x, _node.nodeIndex.y + 1)))
		{
				//if the node ^above^ is walkable then push it to the neighbors
				Node neighbor = *GetNodeAt(glm::ivec2(_node.nodeIndex.x, _node.nodeIndex.y + 1)).lock();
				neighbors.push_back(neighbor);
				side0 = true;
		}
		if (IsWalkableAt(glm::ivec2(_node.nodeIndex.x + 1, _node.nodeIndex.y)))
		{
				//if the node >To the right> is walkable then push it to the neighbors
				Node neighbor = *GetNodeAt(glm::ivec2(_node.nodeIndex.x + 1, _node.nodeIndex.y)).lock();
				neighbors.push_back(neighbor);
				side1 = true;
		}
		if (IsWalkableAt(glm::ivec2(_node.nodeIndex.x, _node.nodeIndex.y - 1)))
		{
				//if the node \/below\/ is walkable then push it to the neighbors
				Node neighbor = *GetNodeAt(glm::ivec2(_node.nodeIndex.x, _node.nodeIndex.y - 1)).lock();
				neighbors.push_back(neighbor);
				side2 = true;
		}
		if (IsWalkableAt(glm::ivec2(_node.nodeIndex.x - 1, _node.nodeIndex.y)))
		{
				//if the node <to the left< is walkable then push it to the neighbors
				Node neighbor = *GetNodeAt(glm::ivec2(_node.nodeIndex.x - 1, _node.nodeIndex.y)).lock();
				neighbors.push_back(neighbor);
				side3 = true;
		}

		//Check the diagonal movement state
		switch (_diagonal)
		{
		case Diagonal::ALWAYS:
		{
				//always allow diagonals
				diagonal0 = true;
				diagonal1 = true;
				diagonal2 = true;
				diagonal3 = true;
				break;
		}
		case Diagonal::NEVER:
		{
				//if it is just return the current walkable neighbors
				return neighbors;
		}
		case Diagonal::IFNOWALLS:
		{
				//diagonals are available if the 2 tiles adjacent to it are walkable
				diagonal0 = side0 && side3;
				diagonal1 = side0 && side1;
				diagonal2 = side1 && side2;
				diagonal3 = side2 && side3;
				break;
		}
		case Diagonal::IFLESSTHANTWOWALLS:
		{
				//diagonals are available if at least 1 of the 2 adjacent tiles are free
				diagonal0 = side0 || side3;
				diagonal1 = side0 || side1;
				diagonal2 = side1 || side2;
				diagonal3 = side2 || side3;
				break;
		}
		default:
				//error
				break;
		}

		//add all the walkable diagonals

		if (diagonal0 && IsWalkableAt(glm::ivec2(_node.nodeIndex.x - 1, _node.nodeIndex.y + 1)))
		{
				Node neighbor = *GetNodeAt(glm::ivec2(_node.nodeIndex.x - 1, _node.nodeIndex.y + 1)).lock();
				neighbors.push_back(neighbor);
		}
		if (diagonal1 && IsWalkableAt(glm::ivec2(_node.nodeIndex.x + 1, _node.nodeIndex.y + 1)))
		{
				Node neighbor = *GetNodeAt(glm::ivec2(_node.nodeIndex.x + 1, _node.nodeIndex.y + 1)).lock();
				neighbors.push_back(neighbor);
		}
		if (diagonal2 && IsWalkableAt(glm::ivec2(_node.nodeIndex.x + 1, _node.nodeIndex.y - 1)))
		{
				Node neighbor = *GetNodeAt(glm::ivec2(_node.nodeIndex.x + 1, _node.nodeIndex.y - 1)).lock();
				neighbors.push_back(neighbor);
		}
		if (diagonal3 && IsWalkableAt(glm::ivec2(_node.nodeIndex.x - 1, _node.nodeIndex.y - 1)))
		{
				Node neighbor = *GetNodeAt(glm::ivec2(_node.nodeIndex.x - 1, _node.nodeIndex.y - 1)).lock();
				neighbors.push_back(neighbor);
		}

		return neighbors;
}

void Grid::DrawGrid(const glm::mat4& _projection)
{
		for (size_t i = 0; i < m_nodeMap.size(); i++)
		{
				std::weak_ptr<Node> currentNode = m_nodeMap.at(i);

				float radius = m_nodeDiameter / 2.0f;

				//set the position minus the radius, because the world space position of the node is at the center, while we want to render it from the bottom left
				glm::vec4 destRect(currentNode.lock()->worldPos.x - radius, currentNode.lock()->worldPos.y - radius, m_nodeDiameter - 1.0f, m_nodeDiameter - 1.0f);

				GameEngine::ColorRGBA8 color;

				//draw the node with red color if it's collidable and green if it's not
				if (!currentNode.lock()->walkable)
				{
						color.r = 255;
						color.g = 0;
						color.b = 0;
						color.a = 255;
				}
				else
				{
						color.r = 0;
						color.g = 255;
						color.b = 0;
						color.a = 255;
				}
				m_debugRenderer.DrawBox(destRect, color, 0.0f);
		}
		m_debugRenderer.End();
		m_debugRenderer.Render(_projection, 1.0f);
}

void Grid::DrawPath(const std::vector<glm::vec2>& _path, const glm::mat4& _projection)
{
		for (size_t i = 0; i < _path.size(); i++)
		{
				std::weak_ptr<Node> currentNode = GetNodeAt(_path.at(i));

				float radius = m_nodeDiameter / 2.0f;

				//set the position minus the radius, because the world space position of the node is at the center, while we want to render it from the bottom left
				glm::vec4 destRect(currentNode.lock()->worldPos.x - radius, currentNode.lock()->worldPos.y - radius, m_nodeDiameter - 1.0f, m_nodeDiameter - 1.0f);

				GameEngine::ColorRGBA8 color;

				color.r = 0;
				color.g = 0;
				color.b = 0;
				color.a = 255;

				m_debugRenderer.DrawBox(destRect, color, 0.0f);
		}
		m_debugRenderer.End();
		m_debugRenderer.Render(_projection, 1.0f);
}

void Grid::CleanGrid()
{
		for (auto& node : m_nodeMap)
		{
				node->g = 0;
				node->h = 0;
				node->inClosedSet = false;
				node->inOpenSet = false;
				node->parent.reset();
		}
}

int Grid::GetNumNodes()
{
		return m_numXNodes * m_numYNodes;
}

void Grid::CreateGrid(std::vector<bool>& _walkableMatrix)
{
		size_t nodeMapSize = m_numXNodes * m_numYNodes;
		m_nodeMap.reserve(nodeMapSize);

		glm::vec2 worldBottomLeft{ 0.0f, 0.0f };

		for (size_t i = 0; i < nodeMapSize; i++)
		{
				int x = i % m_numXNodes;
				int y = i / m_numXNodes;

				float radius = m_nodeDiameter / 2.0f;

				/*x or y * diameter so that it can correctly offset in each axis (node at index 0 will be from 0 to node diameter,
				index 2 will start from 2 * diameter which means it will start at the end of the previous node and the added radius
				in both axis is so that its world space position is at the center of the node and not at the bottom left (feels better) */
				glm::vec2 worldPoint = worldBottomLeft + glm::vec2(x * m_nodeDiameter + radius, y * m_nodeDiameter + radius);

				std::shared_ptr<Node> node = std::make_shared<Node>(worldPoint, glm::ivec2(x, y), _walkableMatrix.at(i));

				m_nodeMap.push_back(std::move(node));
		}
}
void Grid::CreateGrid()
{
		size_t nodeMapSize = m_numXNodes * m_numYNodes;
		m_nodeMap.reserve(nodeMapSize);

		glm::vec2 worldBottomLeft{ 0.0f, 0.0f };

		for (size_t i = 0; i < nodeMapSize; i++)
		{
				int x = i % m_numXNodes;
				int y = i / m_numXNodes;

				float radius = m_nodeDiameter / 2.0f;

				/*x or y * diameter so that it can correctly offset in each axis (node at index 0 will be from 0 to node diameter,
				index 2 will start from 2 * diameter which means it will start at the end of the previous node and the added radius
				in both axis is so that its world space position is at the center of the node and not at the bottom left (feels better) */
				glm::vec2 worldPoint = worldBottomLeft + glm::vec2(x * m_nodeDiameter + radius, y * m_nodeDiameter + radius);

				std::shared_ptr<Node> node = std::make_shared<Node>(worldPoint, glm::ivec2(x, y), true);

				m_nodeMap.push_back(std::move(node));
		}
}
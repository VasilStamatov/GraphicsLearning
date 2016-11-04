#pragma once

#include <glm/vec2.hpp>

//test
#include <GameEngine\Vertex.h>

enum class Diagonal : size_t
{
		// The Diagonal movement is always allowed
		ALWAYS = 1,
		// Diagonal movement is never allowed
		NEVER = 2,
		//DIagonal movement is allowed if there are no walls in the direction of the diagonal
		IFNOWALLS = 3,
		//Diagonal movement is allowed if there is 1 or less walls in the direction of the diagonal
		IFLESSTHANTWOWALLS = 4
};

struct Node
{
		Node() {}
		Node(const glm::vec2& _worldPos, const glm::ivec2& _index,  bool _walkable) :
				worldPos(_worldPos), nodeIndex(_index) , walkable(_walkable), inOpenSet(false), inClosedSet(false) {}
		~Node() { }

		bool operator== (Node& _rhs)
		{
				if (this->nodeIndex == _rhs.nodeIndex)
				{
						return true;
				}
				return false;
		}
		//return true if lhs has lower priority than rhs
		bool operator< (Node& _rhs)
		{
				if (f() == _rhs.f())
				{
						return !(h < _rhs.h);
				}
				else
				{
						return !(f() < _rhs.f());
				}
		}
		//return true if lhs has higher priority than rhs
		bool operator> (Node& _rhs)
		{
				if (f() == _rhs.f())
				{
						return !(h > _rhs.h);
				}
				else
				{
						return !(f() > _rhs.f());
				}
		}
		bool operator== (Node* _rhs)
		{
				if (this->nodeIndex == _rhs->nodeIndex)
				{
						return true;
				}
				return false;
		}
		bool operator== (std::weak_ptr<Node> _rhs)
		{
				if (this->nodeIndex == _rhs.lock()->nodeIndex)
				{
						return true;
				}
				return false;
		}

		//return true if lhs has lower priority than rhs
		bool operator< (Node* _rhs)
		{
				if (f() == _rhs->f())
				{
						return !(h < _rhs->h);
				}
				else
				{
						return !(f() < _rhs->f());
				}
		}
		//return true if lhs has higher priority than rhs
		bool operator> (Node* _rhs)
		{
				if (f() == _rhs->f())
				{
						return !(h > _rhs->h);
				}
				else
				{
						return !(f() > _rhs->f());
				}
		}

		glm::vec2 worldPos{ 0.0f, 0.0f }; ///< world coordinate (center of node)
		glm::ivec2 nodeIndex{ -1 }; ///< the index of the node in the nodemap vector for the grid

		int g{ 0 }; ///< distance from start to current node
		int h{ 0 }; ///< distance from end to current node
		int f() { return g + h; }; ///< g + h

		bool walkable{ false }; ///flag whether you can walk through this node
		bool inOpenSet{ false }; ///Flag for a log(1) check if in open set
		bool inClosedSet{ false }; ///Flag for a log(1) check if in closed set

		std::weak_ptr<Node> parent;

		GameEngine::ColorRGBA8 color;
};

struct ComparePriority
{
		bool operator()(Node & _lhs, Node & _rhs)
		{
				// return "true" if "_lhs" is ordered before "_rhs" (_lhs has less priority)
				if (_lhs.f() == _rhs.f())
				{
						return (_lhs.h > _rhs.h);
				}
				else
				{
						return (_lhs.f() > _rhs.f());
				}
		}

		bool operator()(std::weak_ptr<Node> _lhs, std::weak_ptr<Node> _rhs)
		{
				// return "true" if "_lhs" is ordered before "_rhs" (_lhs has less priority)
				if (_lhs.lock()->f() == _rhs.lock()->f())
				{
						return (_lhs.lock()->h > _rhs.lock()->h);
				}
				else
				{
						return (_lhs.lock()->f() > _rhs.lock()->f());
				}
		}

		bool operator()(Node * _lhs, Node * _rhs)
		{
				// return "true" if "_lhs" is ordered before "_rhs" (_lhs has less priority)
				if (_lhs->f() == _rhs->f())
				{
						return (_lhs->h > _rhs->h);
				}
				else
				{
						return (_lhs->f() > _rhs->f());
				}
		}
};
#pragma once

#include <glm/vec2.hpp>

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
		Node(const glm::vec2& _worldPos, const glm::ivec2& _index, bool _walkable) :
				worldPos(_worldPos), nodeIndex(_index), walkable(_walkable), inOpenSet(false), inClosedSet(false) {}
		~Node() { }

		bool operator== (const Node& _rhs) const
		{
				if (nodeIndex == _rhs.nodeIndex)
				{
						return true;
				}
				return false;
		}
		bool operator!= (const Node& _rhs) const
		{
				if (nodeIndex == _rhs.nodeIndex)
				{
						return false;
				}
				return true;
		}
		bool operator>  (const Node& _rhs) const
		{
				if (f() == _rhs.f())
				{
						return (h < _rhs.h);
				}
				else
				{
						return (f() < _rhs.f());
				}
		}
		bool operator<  (const Node& _rhs) const
		{
				if (f() == _rhs.f())
				{
						return (h > _rhs.h);
				}
				else
				{
						return (f() > _rhs.f());
				}
		}
		bool operator== (const Node* _rhs) const
		{
				if (nodeIndex == _rhs->nodeIndex)
				{
						return true;
				}
				return false;
		}
		bool operator!= (const Node* _rhs) const
		{
				if (nodeIndex == _rhs->nodeIndex)
				{
						return false;
				}
				return true;
		}
		bool operator<  (const Node* _rhs) const
		{
				if (f() == _rhs->f())
				{
						return (h > _rhs->h);
				}
				else
				{
						return (f() > _rhs->f());
				}
		}
		bool operator>  (const Node* _rhs) const
		{
				if (f() == _rhs->f())
				{
						return (h < _rhs->h);
				}
				else
				{
						return (f() < _rhs->f());
				}
		}
		bool operator== (const std::weak_ptr<Node> _rhs) const
		{
				if (nodeIndex == _rhs.lock()->nodeIndex)
				{
						return true;
				}
				return false;
		}
		bool operator!= (const std::weak_ptr<Node> _rhs) const
		{
				if (nodeIndex == _rhs.lock()->nodeIndex)
				{
						return false;
				}
				return true;
		}
		bool operator>  (const std::weak_ptr<Node> _rhs) const
		{
				if (f() == _rhs.lock()->f())
				{
						return (h < _rhs.lock()->h);
				}
				else
				{
						return (f() < _rhs.lock()->f());
				}
		}
		bool operator<  (const std::weak_ptr<Node> _rhs) const
		{
				if (f() == _rhs.lock()->f())
				{
						return (h > _rhs.lock()->h);
				}
				else
				{
						return (f() > _rhs.lock()->f());
				}
		}

		glm::vec2 worldPos{ 0.0f, 0.0f }; ///< world coordinate (center of node)
		glm::ivec2 nodeIndex{ -1 }; ///< the index of the node in the nodemap vector for the grid

		int terrainCost{ 0 };
		int g{ 0 }; ///< distance from start to current node
		int h{ 0 }; ///< distance from end to current node
		int f() const noexcept { return g + h; }; ///< g + h

		bool walkable{ false }; ///flag whether you can walk through this node
		bool inOpenSet{ false }; ///Flag for a log(1) check if in open set
		bool inClosedSet{ false }; ///Flag for a log(1) check if in closed set

		std::weak_ptr<Node> parent;
};

//Comparator for priority queues
struct ComparePriority
{
		bool operator()(const std::weak_ptr<Node> _lhs, const std::weak_ptr<Node> _rhs) const noexcept
		{
				// return "true" if "_lhs" is ordered before "_rhs" (_lhs has less priority)
				if (_lhs.lock()->f() == _rhs.lock()->f())
				{
						//have a tie breaker just in case
						return (_lhs.lock()->h > _rhs.lock()->h);
				}
				else
				{
						//have a tie breaker just in case
						return (_lhs.lock()->f() > _rhs.lock()->f());
				}
		}

		bool operator()(const Node & _lhs, const Node & _rhs) const noexcept
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

		bool operator()(const Node * _lhs, const Node * _rhs) const noexcept
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

struct SecondaryComparator
{
		bool operator()(const std::weak_ptr<Node> _lhs, const std::weak_ptr<Node> _rhs) const noexcept
		{
				// return "true" if "_lhs" is ordered before "_rhs" (_lhs has less priority(depending on g))
				if (_lhs.lock()->g == _rhs.lock()->g)
				{
						//have a tie breaker just in case
						return (_lhs.lock()->h > _rhs.lock()->h);
				}
				else
				{
						return (_lhs.lock()->g > _rhs.lock()->g);
				}
		}

		bool operator()(const Node & _lhs, const Node & _rhs) const noexcept
		{
				// return "true" if "_lhs" is ordered before "_rhs" (_lhs has less priority(depending on g))
				if (_lhs.g == _rhs.g)
				{
						//have a tie breaker just in case
						return (_lhs.h > _rhs.h);
				}
				else
				{
						return (_lhs.g > _rhs.g);
				}
		}

		bool operator()(const Node * _lhs, const Node * _rhs) const noexcept
		{
				// return "true" if "_lhs" is ordered before "_rhs" (_lhs has less priority(depending on g))
				if (_lhs->g == _rhs->g)
				{
						//have a tie breaker just in case
						return (_lhs->h > _rhs->h);
				}
				else
				{
						return (_lhs->g > _rhs->g);
				}
		}
};

// custom hash can be a standalone function object:
// http://en.cppreference.com/w/cpp/utility/hash
struct NodeHasher
{
		std::size_t operator()(const Node & _node) const noexcept
		{
				//hash the world pos and node index (both are different for every node)
				std::size_t hash0 = std::hash<int>{}(_node.nodeIndex.x);
				std::size_t hash1 = std::hash<int>{}(_node.nodeIndex.y);

				std::size_t hash2 = std::hash<float>{}(_node.worldPos.x);
				std::size_t hash3 = std::hash<float>{}(_node.worldPos.y);

				return (hash0 ^ hash1) ^ ((hash2 ^ hash3) << 1);
		}
		std::size_t operator()(std::weak_ptr<Node> _node) const noexcept
		{
				//hash the world pos and node index (both are different for every node)
				std::size_t hash0 = std::hash<int>{}(_node.lock()->nodeIndex.x);
				std::size_t hash1 = std::hash<int>{}(_node.lock()->nodeIndex.y);

				std::size_t hash2 = std::hash<float>{}(_node.lock()->worldPos.x);
				std::size_t hash3 = std::hash<float>{}(_node.lock()->worldPos.y);

				return (hash0 ^ hash1) ^ ((hash2 ^ hash3) << 1);
		}
		std::size_t operator()(Node* _node) const noexcept
		{
				//hash the world pos and node index (both are different for every node)
				std::size_t hash0 = std::hash<int>{}(_node->nodeIndex.x);
				std::size_t hash1 = std::hash<int>{}(_node->nodeIndex.y);

				std::size_t hash2 = std::hash<float>{}(_node->worldPos.x);
				std::size_t hash3 = std::hash<float>{}(_node->worldPos.y);

				return (hash0 ^ hash1) ^ ((hash2 ^ hash3) << 1);
		}
};

//The comparator to be used for the unordered set, it must return true of var a == var b
struct HashComparator
{
		bool operator()(const std::weak_ptr<Node> _lhs, const std::weak_ptr<Node> _rhs) const noexcept
		{
				//check if their index in the nodemap is the same, if it is then it means they are the same node
				if (_lhs.lock()->nodeIndex == _rhs.lock()->nodeIndex)
				{
						return true;
				}
				return false;
		}

		bool operator()(const Node & _lhs, const Node & _rhs) const noexcept
		{
				//check if their index in the nodemap is the same, if it is then it means they are the same node
				if (_lhs.nodeIndex == _rhs.nodeIndex)
				{
						return true;
				}
				return false;
		}

		bool operator()(const Node * _lhs, const Node * _rhs) const noexcept
		{
				//check if their index in the nodemap is the same, if it is then it means they are the same node
				if (_lhs->nodeIndex == _rhs->nodeIndex)
				{
						return true;
				}
				return false;
		}
};
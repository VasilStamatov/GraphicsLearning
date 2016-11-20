#pragma once
#include <vector>
#include <memory>
#include <GameEngine\DebugRenderer.h>

#include "Node.h"

class Grid
{
public:
		Grid(const Grid& _obj);
		Grid();
		~Grid();

		/** \brief Create the grid (2d matrix of nodes)
		* \param _gridWorldSize - the width and height of the grid in pixels
		* \param _nodeDiameter - the diameter of each node
		The number of nodes is calculated by using the width and height division by the diameter
		m_numXNodes = (int)ceil((_width / _nodeDiameter));
		m_numYNodes = (int)ceil((_height / _nodeDiameter));
		* \param _walkable matrix - a 2d vector array of booleans depicting which node is walkable (needs to be the same size as the grid)
		*/
		Grid(const glm::vec2& _gridWorldSize, float _nodeDiameter, std::vector<bool>& _walkableMatrix);
		Grid(const glm::vec2& _gridWorldSize, float _nodeDiameter);

		/** \brief Create the grid (2d matrix of nodes)
		* \param _numXNodes - the number of nodes in the x axis
		* \param _numYNodes - the number of nodes in the Y axis
		* \param _nodeDiameter - the diameter of each node
		By passing how many nodes we want in the x and y axis we can
		manually calculate the size of the grid map in pixels by
		_width = float(_numXNodes * _nodeDiameter);
		_height = float(_numYNodes * _nodeDiameter);
		* \param _walkable matrix - a 2d vector array of booleans depicting which node is walkable (needs to be the same size as the grid)
		*/
		Grid(size_t _numXNodes, size_t _numYNodes, float _nodeDiameter, std::vector<bool>& _walkableMatrix);
		Grid(size_t _numXNodes, size_t _numYNodes, float _nodeDiameter);

		/** \brief Gets node based on world coordinate
		* \param _worldPos - the x,y coordinate in world space
		* the coordinate is to be converted into the indices in the node vector
		* eg. world coordinate of 0,0 to diameter, diameter (32,32 for example)
		* will be converted to 0, 0 and 0 + diameter, 0 + diameter to diameter * 2, diameter * 2 (diagonal top-right of index 0, 0)
		* will be converted to 1, 1
		* \return Node* -  a pointer to the node at this coordinate
		*/
		std::weak_ptr<Node> GetNodeAt(const glm::vec2& _worldPos);

		/** \brief Gets node based on its index in the node vector
		* \param _index - the x,y index in the vector
		* index 0,0 return the bottom left node, while index m_numXNodes, m_numyNodes will return the top right node
		* \return Node* -  a pointer to the node at this index
		*/
		std::weak_ptr<Node> GetNodeAt(const glm::ivec2& _index);

		/** \brief Check if the node at a certain world coordinate can be walked on
		* \param _worldPos - the world coordinate of the node
		* \return bool					-	true if walkable, false if coordinate isn't in the node map or node isn't walkable
		*/
		bool IsWalkableAt(const glm::vec2& _worldPos);

		/** \brief Check if the node at a certain index in the node vector can be walked on
		* \param _index - the x,y index in the vector
		* \return bool  - true if walkable, false if coordinate isn't in the node map or node isn't walkable
		*/
		bool IsWalkableAt(const glm::ivec2& _index);

		/** \brief Check if this index exists in the vector
		* \param _x - the x index (node on the x axis
		* \param _y - the y index (node on the y axis)
		* This fucntion will check if a certain index exists in the node vector
		* the index should not have coordinates below 0 or above the specified m_numXNodes and m_numyNodes of the map
		* \return bool -  true if the coordinate exists
		*/
		bool IsPosInside(const glm::vec2& _index);

		/** \brief Sets the node at set world coordinate to _walkable
		* \param _worldPos - the world coordinate of the node
		* \param _walkable - the flag whether to set it to walkable or not
		*/
		void SetWalkableAt(const glm::vec2& _worldPos, bool _walkable);

		/** \brief Sets the node at set index to _walkable
		* \param _index - the index of the node in the vector
		* \param _walkable - the flag whether to set it to walkable or not
		*/
		void SetWalkableAt(const glm::ivec2& _index, bool _walkable);
		
		/** \brief Sets the terrain cost of node at set world coordinate to _cost
		* \param _worldPos - the world coordinate of the node
		* \param _cost - the terrain cost
		*/
		void SetTerrainCost(const glm::vec2& _worldPos, int _cost);

		/** \brief Sets the terrain cost of node at set index to _cost
		* \param _index - the index of the node in the vector
		* \param _cost - the terrain cost
		*/
		void SetTerrainCost(const glm::ivec2& _index, int _cost);

		/** \brief Gets all the available neighbors of the certain node
		* \param _node - pointer to the node to be checked
		* \param _diagonal - flag for diagonal movement
		* \return std::vector<Node*> - a vector of all available neighbors
		*/
		std::vector<std::weak_ptr<Node>> GetNeighbors(std::weak_ptr<Node> _node, const Diagonal& _diagonal);
		std::vector<Node> GetNeighbors(const Node& _node, const Diagonal& _diagonal);

		std::vector<std::shared_ptr<Node>> GetNodemap() const noexcept { return m_nodeMap; }

		/** \brief Render the outlines of the nodes as rectangles for debugging
		* \param _projection - the projection matrix to be used in the shader
		*/
		void DrawGrid(const glm::mat4& _projection);
		void DrawPath(const std::vector<glm::vec2>& _path, const glm::mat4& _projection);

		/** \brief Cleans the values of the nodes in the grid (except for terrain cost) */
		void CleanGrid();

		/** \brief Gets the total number of nodes in the node map */
		int GetNumNodes();

private:
		void CreateGrid(std::vector<bool>& _walkableMatrix); ///< create the grid with preset collidable flags
		void CreateGrid(); ///< create the grid with all nodes set to collidable

private:
		std::vector<std::shared_ptr<Node>> m_nodeMap; ///< a 1D vector representing a 2D vector for better performance
		glm::vec2 m_gridWorldSize{ 0.0f, 0.0f }; ///< the size of the grid world (width * node diameter and height * nodeDiameter) 
		float m_nodeDiameter; /// the diameter of each node
		int m_numXNodes; ///< the number of nodes on the x axis (width) 
		int m_numYNodes; ///< the number of nodes on the y axis (height)
		GameEngine::DebugRenderer m_debugRenderer; ///< a debug renderer to render the nodes for debugging
};
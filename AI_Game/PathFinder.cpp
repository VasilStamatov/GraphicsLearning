#include "PathFinder.h"



PathFinder::PathFinder()
{
}


PathFinder::~PathFinder()
{ }

//
//int PathFinder::Manhattan(float _deltaX, float _deltaY)
//{
//		return _deltaX + _deltaY;
//}
//
//int PathFinder::Manhattan(const glm::vec2 & _deltaVec)
//{
//		return _deltaVec.x + _deltaVec.y;
//}
//
//int PathFinder::Euclidean(float _deltaX, float _deltaY)
//{
//		glm::vec2 deltaVec{ _deltaX, _deltaY };
//		return int(glm::length(deltaVec));
//}
//
//int PathFinder::Euclidean(const glm::vec2 & _deltaVec)
//{
//		glm::vec2 deltaVec{ _deltaVec };
//		return int(glm::length(deltaVec));
//}
//
//int PathFinder::Octile(float _deltaX, float _deltaY)
//{
//		return 0;
//}
//
//int PathFinder::Octile(const glm::vec2 & _deltaVec)
//{
//		return 0;
//}
//
//int PathFinder::Chebyshev(float _deltaX, float _deltaY)
//{
//		return (_deltaX > _deltaY) ? _deltaX : _deltaY;
//}
//
//int PathFinder::Chebyshev(const glm::vec2 & _deltaVec)
//{
//		return (_deltaVec.x > _deltaVec.y) ? _deltaVec.x : _deltaVec.y;
//}

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

//std::vector<glm::vec2> PathFinder::BiBacktrace(Node * _nodeA, Node * _nodeB)
//{
//		std::vector<glm::vec2> pathA = Backtrace(_nodeA);
//		std::vector<glm::vec2> pathB = Backtrace(_nodeB);
//		std::reverse(pathB.begin(), pathB.end());
//		pathA.insert(pathA.end(), pathB.begin(), pathB.end());
//		return pathA;
//}
//
//std::vector<glm::vec2> PathFinder::Interpolate(const glm::vec2 & _startCoord, const glm::vec2 & _endCoord)
//{
//		std::vector<glm::vec2> result;
//		int deltax{ 0 }, deltay{ 0 };
//		glm::vec2 startCoord{ _startCoord }; //current start coordinate
//		glm::vec2 progressCoord{ 0,0 };
//		int deltaError{ 0 };
//		int delta2Error{ deltaError * 2 };
//
//		deltax = (int)(fabsf(_endCoord.x - _startCoord.x));
//		deltay = (int)(fabsf(_endCoord.y - _startCoord.y));
//		progressCoord.x = (_startCoord.x < _endCoord.x) ? 1 : -1;
//		progressCoord.y = (_startCoord.y < _endCoord.y) ? 1 : -1;
//
//		deltaError = deltax - deltay;
//
//		while (true)
//		{
//				result.emplace_back(startCoord.x, startCoord.y);
//
//				if (_startCoord.x == _endCoord.x && _startCoord.y == _endCoord.y)
//				{
//						break;
//				}
//
//				delta2Error = deltaError * 2;
//				if (delta2Error > -deltay)
//				{
//						deltaError -= deltay;
//						startCoord.x += progressCoord.x;
//				}
//				if (delta2Error < deltax)
//				{
//						deltaError += deltax;
//						startCoord.y += progressCoord.y;
//				}
//		}
//		return result;
//}
//
//std::vector<glm::vec2> PathFinder::ExpandPath(std::vector<glm::vec2>& _path)
//{
//		std::vector<glm::vec2> expanded;
//		std::vector<glm::vec2> interpolated;
//		glm::vec2 coord0{ 0, 0 }, coord1{ 0, 0 };
//
//		if (_path.size() < 2)
//		{
//				return expanded;
//		}
//
//		for (size_t pathIndex = 0; pathIndex < _path.size() - 1; pathIndex++)
//		{
//				coord0 = _path.at(pathIndex);
//				coord1 = _path.at(pathIndex + 1);
//
//				std::vector<glm::vec2> interpolate = Interpolate(coord0, coord1);
//
//				interpolated.insert(interpolated.end(), interpolate.begin(), interpolate.end());
//
//				for (size_t j = 0; j < interpolated.size() - 1; j++)
//				{
//						expanded.push_back(interpolated.at(j));
//				}
//		}
//
//		expanded.push_back(_path.at(_path.size() - 1));
//
//		return expanded;
//}
//
//std::vector<glm::vec2> PathFinder::SmoothenPath(std::vector<glm::vec2>& _path, Grid * _grid)
//{
//		//glm::vec2 end{ _path.at(_path.size() - 1) }; //last coord
//		//glm::vec2 startCoord{ _path.at(0) }; //current start coordinate
//		//glm::vec2 endCoord{ 0 , 0 }; //current end coordinate
//		//bool blocked{ false };
//
//		//std::vector<glm::vec2> newPath;
//
//		////push the first coord
//		//newPath.push_back(startCoord);
//
//		////push all coords inbetween start and end
//		//for (size_t pathIndex = 1; pathIndex < _path.size(); pathIndex++)
//		//{
//		//		glm::vec2 coord{ _path.at(pathIndex) };
//		//		endCoord = coord;
//		//		std::vector<glm::vec2> line = Interpolate(startCoord, endCoord);
//
//		//		blocked = false;
//		//		for (size_t lineIndex = 0; lineIndex < line.size(); lineIndex++)
//		//		{
//		//				if (!_grid->IsWalkableAt(line.at(lineIndex).x, line.at(lineIndex).y))
//		//				{
//		//						blocked = true;
//		//						break;
//		//				}
//		//		}
//		//		if (blocked)
//		//		{
//		//				glm::vec2 lastValidCoord = _path.at(pathIndex - 1);
//		//				newPath.push_back(lastValidCoord);
//		//				startCoord = lastValidCoord;
//		//		}
//		//}
//
//		////push the last coord
//		//newPath.push_back(end);
//
//		//return newPath;
//		return std::vector<glm::vec2>();
//}
//
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
//
//float PathFinder::PathLength(std::vector<glm::vec2>& _path)
//{
//		float sum{ 0 };
//		for (size_t i = 1; i < _path.size(); i++)
//		{
//				glm::vec2 currentCoord = _path.at(i);
//				glm::vec2 previousCoord = _path.at(i - 1);
//
//				glm::vec2 direction{ previousCoord.x - currentCoord.x, previousCoord.y - currentCoord.y };
//				sum += glm::length(direction);
//		}
//		return sum;
//}

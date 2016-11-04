#include "AABB.h"
#include <string>


AABB::AABB()
{
}


AABB::~AABB()
{
}

AABB::AABB(const glm::vec2 & _min, const glm::vec2 & _max)
{
		m_min = glm::vec3(_min, 0.0f);
		m_max = glm::vec3(_max, 0.0f);
}

AABB::AABB(const glm::vec3 & _min, const glm::vec3 & _max)
{
		m_min = _min;
		m_max = _max;
}

AABB::AABB(float _minX, float _minY, float _width, float _height)
{
		m_min = glm::vec3(_minX, _minY, 0.0f);
		m_max = glm::vec3(_minX + _width, _minY + _height, 0.0f);
}

AABB::AABB(float _minX, float _minY, float _minZ, float _width, float _height, float _depth)
{
		m_min = glm::vec3(_minX, _minY, _minZ);
		m_max = glm::vec3(_minX + _width, _minY + _height, _minZ + _depth);
}

bool AABB::Intersects(const AABB & _other) const
{
		return m_min.x < _other.GetMax().x && m_max.x > _other.GetMax().x &&
								 m_min.y < _other.GetMax().y && m_max.y > _other.GetMin().y &&
								 m_min.z < _other.GetMax().z && m_max.z > _other.GetMin().z;
}

bool AABB::Contains(const AABB & _other) const
{
		return m_min.x <= _other.GetMin().x && _other.GetMax().x <= m_max.x &&
								 m_min.y <= _other.GetMin().y && _other.GetMax().y <= m_max.y &&
								 m_min.z <= _other.GetMin().z && _other.GetMax().z <= m_max.z;
}

bool AABB::Contains(const glm::vec2 & _point) const
{
		return _point.x > m_min.x && _point.y > m_min.y &&
							 	_point.x < m_max.x && _point.y < m_max.y;
}

bool AABB::Contains(const glm::vec3 & _point) const
{
		return _point.x > m_min.x && _point.y > m_min.y && _point.z > m_min.z &&
								 _point.x < m_max.x && _point.y < m_max.y && _point.z < m_max.z;
}

glm::vec3 AABB::Center() const
{
		return glm::vec3((m_min + m_max) * 0.5f);
}

bool AABB::operator==(const AABB & _other) const
{
		return (m_min == _other.GetMin()) && (m_max == _other.GetMax());
}

bool AABB::operator!=(const AABB & _other) const
{
		return !(*this == _other);
}

bool AABB::operator<(const AABB & _other) const
{
		return (m_max.x < _other.GetMin().x) && (m_max.y < _other.GetMin().y) && (m_max.z < _other.GetMin().z);
}

bool AABB::operator>(const AABB & _other) const
{
		return (m_min.x < _other.GetMax().x) && (m_min.y < _other.GetMax().y) && (m_min.z < _other.GetMax().z);
}

std::ostream & operator<<(std::ostream & _stream, const AABB & _aabb)
{
				_stream << "(minX: " + std::to_string(_aabb.GetMin().x) + ", maxX: " + std::to_string(_aabb.GetMax().x) + "), " +
														 "(minY: " + std::to_string(_aabb.GetMin().y) + ", maxY: " + std::to_string(_aabb.GetMax().y) + "), " +
														 "(minZ: " + std::to_string(_aabb.GetMin().z) + ", maxZ: " + std::to_string(_aabb.GetMax().z) + ")";
		return _stream;
}

#pragma once
#include <glm\glm.hpp>
#include <ostream>

//Axis aligned bounding box for 3D shapes
//Can be used as a 2D shape as well with depth value set to 0.0
class AABB
{
public:
		AABB();
		~AABB();

		/** \brief Init AABB with vec2 min and max value
	  *  \param _min - min value of the bounding box (bottom left for a rectangle for example)
		 *  \param _max - max value of the bounding box (top right for a rectangle for example)
		 */
		AABB(const glm::vec2& _min, const glm::vec2& _max);

		/** \brief Init AABB with vec3 min and max value
		*  \param _min - min value of the bounding box (bottom left for a rectangle for example)
		*  \param _max - max value of the bounding box (top right for a rectangle for example)
		*/
		AABB(const glm::vec3& _min, const glm::vec3& _max);

		/** \brief Init AABB with individual values for a 2D shape
		*  \param _minX - min value on the x axis
		*  \param _minY - max value on the y axis
		*  \param _width - width of the bounding box on the x axis (makes max.x = minX + width)
		*  \param _height - height of the bounding box on the y axis (makes max.y = minY + height)
		*/
		AABB(float _minX, float _minY, float _width, float _height);

		/** \brief Init AABB with individual values for a 3D shape
		*  \param _minX - min value on the x axis
		*  \param _minY - max value on the y axis
		*  \param _width - width of the bounding box on the x axis (makes max.x = minX + width)
		*  \param _height - height of the bounding box on the y axis (makes max.y = minY + height)
		*  \param _depth - depth of the bounding box on the z axis (makes max.z = minZ + depth)
		*/
		AABB(float _minX, float _minY, float _minZ, float _width, float _height, float _depth);

		/** \brief Check for intersection with another AABB
		*  \param _other - the other AABB to be checked with
		*/
		bool Intersects(const AABB& _other) const;

		/** \brief Check if another AABB is completely inside this one
		*  \param _other - the other AABB to be checked with
		*/
		bool Contains(const AABB& _other) const;

		/** \brief Check if a 2D point is completely inside this AABB (depth = 0.0)
		*  \param _other - the 2D point to be checked with
		*/
		bool Contains(const glm::vec2& _point) const;

		/** \brief Check if a 3D point is completely inside this AABB
		*  \param _other - the 3D point to be checked with
		*/
		bool Contains(const glm::vec3& _point) const;

		/** \return the center vec3 of the AABB*/
		glm::vec3 Center() const;

		/** Operator overloaders */
		bool operator==(const AABB& _other) const;
		bool operator!=(const AABB& _other) const;

		bool operator<(const AABB& _other) const;
		bool operator>(const AABB& _other) const;

		friend std::ostream& operator<<(std::ostream& _stream, const AABB& _aabb);

		/** Simple Getters */
		inline const glm::vec3& GetSize() const { return glm::vec3(abs(m_max.x - m_min.x), abs(m_max.y - m_min.y), abs(m_max.z - m_min.z)); }
		inline const glm::vec3& GetMin()  const { return m_min; }
		inline const glm::vec3& GetMax()  const { return m_max; }
private:
		glm::vec3 m_min{ 0.0f, 0.0f, 0.0f }; ///< min coord
		glm::vec3 m_max{ 0.0f, 0.0f, 0.0f }; ///< max coord (min + size on axis)
};


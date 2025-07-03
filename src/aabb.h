#pragma once

#include <glm/vec3.hpp>

namespace Eon
{
	struct AABB
	{
		AABB(const glm::dvec3& dim)
			: dimensions(dim), position(0, 0, 0)
		{
		}

		void Update(const glm::dvec3& location)
		{
			position = location;
		}

		glm::dvec3 GetVN(const glm::dvec3& normal) const
		{
			glm::dvec3 res = position;

			if (normal.x < 0)
			{
				res.x += dimensions.x;
			}
			if (normal.y < 0)
			{
				res.y += dimensions.y;
			}
			if (normal.z < 0)
			{
				res.z += dimensions.z;
			}

			return res;
		}

		glm::dvec3 GetVP(const glm::dvec3& normal) const
		{
			glm::dvec3 res = position;

			if (normal.x > 0)
			{
				res.x += dimensions.x;
			}
			if (normal.y > 0)
			{
				res.y += dimensions.y;
			}
			if (normal.z > 0)
			{
				res.z += dimensions.z;
			}

			return res;
		}

		glm::dvec3 position;
		const glm::dvec3 dimensions;
	};
}
#pragma once

#include <glm/vec3.hpp>

namespace Eon
{
	struct AABB
	{
		AABB(const glm::vec3& dim)
			: dimensions(dim), position(0, 0, 0)
		{
		}

		void Update(const glm::vec3& location)
		{
			position = location;
		}

		glm::vec3 GetVN(const glm::vec3& normal) const
		{
			glm::vec3 res = position;

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

		glm::vec3 GetVP(const glm::vec3& normal) const
		{
			glm::vec3 res = position;

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

		glm::vec3 position;
		const glm::vec3 dimensions;
	};
}
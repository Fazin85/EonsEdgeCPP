#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Eon
{
	struct BoundingBox
	{
		BoundingBox(glm::vec3 min, glm::vec3 max);
		bool InBounds3(glm::vec3 position) const;
		bool InBounds2(glm::vec2 position) const;
		BoundingBox Expand(glm::vec3 amount) const;
		float ClipCollideX(BoundingBox& bb, float xv) const;
		float ClipCollideY(BoundingBox& bb, float yv) const;
		float ClipCollideZ(BoundingBox& bb, float zv) const;
		void Move(glm::vec3 amount);

		glm::vec3 min;
		glm::vec3 max;
	};
}
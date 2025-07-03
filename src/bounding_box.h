#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Eon
{
	struct BoundingBox
	{
		BoundingBox(glm::dvec3 min, glm::dvec3 max);
		bool InBounds3(glm::dvec3 position) const;
		bool InBounds2(glm::dvec2 position) const;
		BoundingBox Expand(glm::dvec3 amount) const;
		double ClipCollideX(BoundingBox& bb, double xv) const;
		double ClipCollideY(BoundingBox& bb, double yv) const;
		double ClipCollideZ(BoundingBox& bb, double zv) const;
		void Move(glm::dvec3 amount);

		glm::dvec3 min;
		glm::dvec3 max;
	};
}
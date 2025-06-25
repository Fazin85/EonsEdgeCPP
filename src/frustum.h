#pragma once

#include <array>

#include <glm/glm.hpp>

namespace Eon
{
	struct AABB;

	struct Plane
	{
		float DistanceToPoint(const glm::vec3& point) const;

		float distance_to_origin;
		glm::vec3 normal;
	};

	class Frustum
	{
	public:
		void Update(const glm::mat4& projViewMatrix) noexcept;

		bool BoxInFrustum(const AABB& box) const noexcept;

		std::array<Plane, 6> planes;
	};
}
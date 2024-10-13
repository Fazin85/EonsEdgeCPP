#include "Frustum.h"

#include "aabb.h"
#include <glm/glm.hpp>

namespace Eon
{
	enum Planes
	{
		Near,
		Far,
		Left,
		Right,
		Top,
		Bottom,
	};

	float Plane::DistanceToPoint(const glm::vec3& point) const
	{
		return glm::dot(point, normal) + distance_to_origin;
	}

	void Frustum::Update(const glm::mat4& mat) noexcept
	{
		planes[Planes::Left].normal.x = mat[0][3] + mat[0][0];
		planes[Planes::Left].normal.y = mat[1][3] + mat[1][0];
		planes[Planes::Left].normal.z = mat[2][3] + mat[2][0];
		planes[Planes::Left].distance_to_origin = mat[3][3] + mat[3][0];

		planes[Planes::Right].normal.x = mat[0][3] - mat[0][0];
		planes[Planes::Right].normal.y = mat[1][3] - mat[1][0];
		planes[Planes::Right].normal.z = mat[2][3] - mat[2][0];
		planes[Planes::Right].distance_to_origin = mat[3][3] - mat[3][0];

		planes[Planes::Bottom].normal.x = mat[0][3] + mat[0][1];
		planes[Planes::Bottom].normal.y = mat[1][3] + mat[1][1];
		planes[Planes::Bottom].normal.z = mat[2][3] + mat[2][1];
		planes[Planes::Bottom].distance_to_origin = mat[3][3] + mat[3][1];

		planes[Planes::Top].normal.x = mat[0][3] - mat[0][1];
		planes[Planes::Top].normal.y = mat[1][3] - mat[1][1];
		planes[Planes::Top].normal.z = mat[2][3] - mat[2][1];
		planes[Planes::Top].distance_to_origin = mat[3][3] - mat[3][1];

		planes[Planes::Near].normal.x = mat[0][3] + mat[0][2];
		planes[Planes::Near].normal.y = mat[1][3] + mat[1][2];
		planes[Planes::Near].normal.z = mat[2][3] + mat[2][2];
		planes[Planes::Near].distance_to_origin = mat[3][3] + mat[3][2];

		planes[Planes::Far].normal.x = mat[0][3] - mat[0][2];
		planes[Planes::Far].normal.y = mat[1][3] - mat[1][2];
		planes[Planes::Far].normal.z = mat[2][3] - mat[2][2];
		planes[Planes::Far].distance_to_origin = mat[3][3] - mat[3][2];

		for (auto& plane : planes)
		{
			float length = glm::length(plane.normal);
			plane.normal /= length;
			plane.distance_to_origin /= length;
		}
	}

	bool Frustum::BoxInFrustum(const AABB& box) const noexcept
	{
		bool result = true;
		for (auto& plane : planes)
		{
			if (plane.DistanceToPoint(box.GetVP(plane.normal)) < 0)
			{
				return false;
			}
			else if (plane.DistanceToPoint(box.GetVN(plane.normal)) < 0)
			{
				result = true;
			}
		}
		return result;
	}
}
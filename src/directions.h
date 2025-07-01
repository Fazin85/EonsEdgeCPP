#pragma once

#include <glm/glm.hpp>

namespace Eon
{
	enum class Directions : unsigned char
	{
		Front,
		Back,
		Left,
		Right,
		Top,
		Bottom
	};

	constexpr glm::vec3 GetNormalFromDirection(Directions dir)
	{
		switch (dir)
		{
		case Directions::Front:  return glm::vec3(0, 0, 1);
		case Directions::Back:   return glm::vec3(0, 0, -1);
		case Directions::Left:   return glm::vec3(-1, 0, 0);
		case Directions::Right:  return glm::vec3(1, 0, 0);
		case Directions::Top:    return glm::vec3(0, 1, 0);
		case Directions::Bottom: return glm::vec3(0, -1, 0);
		}
		return glm::vec3(0, 0, 0);
	}
}
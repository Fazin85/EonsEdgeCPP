#pragma once

#include <map>
#include <glm/vec2.hpp>

#include "directions.h"
#include "block.h"

namespace Eon
{
	class BlockTexture
	{
	public:
		static std::map<const Directions, const glm::vec2> GetUVCoordsFromBlockID(BlockType type);
	};
}
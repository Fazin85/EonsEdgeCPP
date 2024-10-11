#pragma once

#include <vector>
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include "num.h"

namespace Eon
{
	struct ChunkMeshData
	{
	public:
		std::vector<glm::ivec3> vertexPositions;
		std::vector<glm::vec2> uvs;
		std::vector<u32> indices;
		std::vector<u8> directions;
		std::vector<u8> light;
		u32 index_count;
	};
}
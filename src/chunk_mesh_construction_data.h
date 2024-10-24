#pragma once

#include <vector>
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"

namespace Eon
{
	struct ChunkMeshConstructionData
	{
	public:
		std::vector<glm::ivec3> vertexPositions;
		std::vector<glm::ivec2> uvs;
		std::vector<unsigned int> indices;
		std::vector<unsigned char> directions;
		std::vector<unsigned char> light;
		unsigned int index_count;
	};
}
#pragma once

#include <vector>
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"

#include "array_pool.h"

namespace Eon
{
	struct ChunkMeshConstructionData
	{
	public:
		ChunkMeshConstructionData() :
			vertexPositions(*PoolAllocators::GetInstance().ivec3_allocator),
			uvs(*PoolAllocators::GetInstance().ivec2_allocator),
			indices(*PoolAllocators::GetInstance().u32_allocator),
			directions(*PoolAllocators::GetInstance().u8_allocator),
			light(*PoolAllocators::GetInstance().u8_allocator),
			index_count(0),
			face_count(0)
		{
		}

		std::vector<glm::ivec3, PoolAllocator<glm::ivec3>> vertexPositions;
		std::vector<glm::ivec2, PoolAllocator<glm::ivec2>> uvs;
		std::vector<unsigned int, PoolAllocator<unsigned int>> indices;
		std::vector<unsigned char, PoolAllocator<unsigned char>> directions;
		std::vector<unsigned char, PoolAllocator<unsigned char>> light;
		unsigned int index_count;
		unsigned int face_count;
	};
}
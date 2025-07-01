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
			vertexPositions(*PoolAllocators::GetInstance().vec3_allocator),
			uvs(*PoolAllocators::GetInstance().vec2_allocator),
			colors(*PoolAllocators::GetInstance().vec3_allocator),
			normals(*PoolAllocators::GetInstance().vec3_allocator)
		{
		}

		std::vector<glm::vec3, PoolAllocator<glm::vec3>> vertexPositions;
		std::vector<glm::vec2, PoolAllocator<glm::vec2>> uvs;
		std::vector<glm::vec3, PoolAllocator<glm::vec3>> colors;
		std::vector<glm::vec3, PoolAllocator<glm::vec3>> normals;
	};
}
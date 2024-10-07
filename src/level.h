#pragma once

#include <array>

#include "block.h"
#include "chunk.h"
#include "chunk_position.h"
#include "num.h"
#include <glm/vec3.hpp>

namespace Eon
{
	class Level
	{
	public:
		Chunk* GetChunk(ChunkPosition position);
		Block* GetBlock(u16 x, u16 y, u16 z);
		Block* GetBlock(glm::ivec3 position);

	private:
		std::array<Chunk*, 1024> chunks;
		u32 IndexFromPosition(u16 x, u16 z);
	};
} // namespace Eon
#pragma once

#include <array>

#include "block.h"
#include "chunk.h"
#include "chunk_position.h"
#include "num.h"
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <memory>

#define LEVEL_WIDTH_CHUNKS 32

namespace Eon
{
	class Level
	{
	public:
		Level();
		Chunk* GetChunk(ChunkPosition position);
		Chunk* GetChunkUnsafe(ChunkPosition position);
		Block* GetBlock(i16 x, i16 y, i16 z);
		Block* GetBlock(glm::ivec3 position);
		glm::vec4& SkyColor();

	private:
		std::array<std::unique_ptr<Chunk>, LEVEL_WIDTH_CHUNKS* LEVEL_WIDTH_CHUNKS> chunks;
		u32 IndexFromPosition(i16 x, i16 z);
		glm::vec4 sky_color;
	};
} // namespace Eon
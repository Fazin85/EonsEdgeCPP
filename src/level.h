#pragma once

#include "block.h"
#include "bounding_box.h"
#include "chunk.h"
#include "chunk_position.h"
#include "num.h"
#include <array>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <memory>

#define LEVEL_WIDTH_CHUNKS 64

namespace Eon
{
	class Level
	{
	public:
		Level();
		Chunk* GetChunk(ChunkPosition position);
		Chunk* GetChunkUnsafe(ChunkPosition position);
		Chunk* GetChunkFromIndex(size_t index);
		Block* GetBlock(i16 x, i16 y, i16 z);
		Block* GetBlock(glm::ivec3 position);
		glm::vec4& SkyColor();
		std::vector<BoundingBox> GetBlockAABBs(const BoundingBox& bb);
		u32 IndexFromPosition(i16 x, i16 z);

	private:
		std::array<std::unique_ptr<Chunk>, LEVEL_WIDTH_CHUNKS* LEVEL_WIDTH_CHUNKS> chunks;
		glm::vec4 sky_color;
	};
} // namespace Eon
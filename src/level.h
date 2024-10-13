#pragma once

#include <array>

#include "block.h"
#include "chunk.h"
#include "chunk_position.h"
#include "num.h"
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Eon
{
	class Level
	{
	public:
		Level();
		Chunk* GetChunk(ChunkPosition position);
		std::optional<Block> GetBlock(i16 x, i16 y, i16 z);
		std::optional<Block> GetBlock(glm::ivec3 position);
		glm::vec4& SkyColor();

	private:
		std::array<Chunk*, 1024> chunks;
		u32 IndexFromPosition(i16 x, i16 z);
		glm::vec4 sky_color;
	};
} // namespace Eon
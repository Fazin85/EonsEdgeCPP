#pragma once

#include "block.h"
#include "chunk_position.h"
#include <optional>

namespace Eon
{
	class Chunk
	{
	public:
		Chunk(ChunkPosition chunkPosition);
		std::optional<Block*> GetBlock(u8 x, u16 y, u8 z);
		ChunkPosition Position() const;

	private:
		u32 IndexFromPosition(u8 x, u16 y, u8 z);
		ChunkPosition position;
		Block blocks[131072];
	};
}  // namespace Eon
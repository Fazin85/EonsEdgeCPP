#pragma once

#include "block.h"
#include "chunk_position.h"
#include <optional>

#define CHUNK_WIDTH 32
#define CHUNK_HEIGHT 512

namespace Eon
{
	class Chunk
	{
	public:
		Chunk(ChunkPosition chunkPosition);
		Block* GetBlock(u8 x, u16 y, u8 z);
		ChunkPosition Position() const;

	private:
		u32 IndexFromPosition(u8 x, u16 y, u8 z);
		ChunkPosition position;
		Block blocks[CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_WIDTH];
	};
}  // namespace Eon
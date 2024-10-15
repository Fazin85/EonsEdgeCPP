#include "chunk.h"

namespace Eon
{
	Chunk::Chunk(ChunkPosition chunkPosition) : blocks{}, position(chunkPosition)
	{
	}

	Block* Chunk::GetBlock(u8 x, u16 y, u8 z)
	{
		if (x >= CHUNK_WIDTH || z >= CHUNK_WIDTH || y >= CHUNK_HEIGHT || x < 0 || z < 0 || y < 0)
		{
			return nullptr;
		};

		return &blocks[IndexFromPosition(x, y, z)];
	}

	ChunkPosition Chunk::Position() const
	{
		return position;
	}

	u32 Chunk::IndexFromPosition(u8 x, u16 y, u8 z)
	{
		return x + CHUNK_WIDTH * (y + CHUNK_HEIGHT * z);
	}
} // namespace Eon

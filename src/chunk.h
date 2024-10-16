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
		~Chunk();
		Block* GetBlock(u8 x, u16 y, u8 z);
		ChunkPosition Position() const;
		void Compress();
		void Decompress();

	private:
		u32 IndexFromPosition(u8 x, u16 y, u8 z);
		ChunkPosition position;
		Block* blocks;
		char* compressed_blocks;
		size_t compressed_blocks_size;
		bool compressed;
	};
}  // namespace Eon
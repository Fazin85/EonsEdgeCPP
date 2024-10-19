#pragma once

#include "block.h"
#include "chunk_position.h"

#define CHUNK_WIDTH 64
#define CHUNK_HEIGHT 256
#define CHUNK_BITSHIFT_AMOUNT 6
#define CHUNK_MAX_LOD 8

namespace Eon
{
	class Chunk
	{
	public:
		Chunk(ChunkPosition chunkPosition);
		~Chunk();
		Block* GetBlock(u8 x, i16 y, u8 z);
		ChunkPosition Position() const;
		void Compress();
		void Decompress();

	private:
		ChunkPosition position;
		Block* blocks;
		char* compressed_blocks;
		size_t compressed_blocks_size;
		bool compressed;
	};
}  // namespace Eon
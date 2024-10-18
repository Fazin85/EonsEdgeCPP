#pragma once

#include "block.h"
#include "chunk_position.h"

#define CHUNK_WIDTH 32
#define CHUNK_HEIGHT 256

namespace Eon
{
	class Chunk
	{
	public:
		Chunk(ChunkPosition chunkPosition);
		~Chunk();
		Block* GetBlock(i8 x, i16 y, i8 z);
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
#pragma once

#include "block.h"
#include "chunk_position.h"

#define CHUNK_WIDTH 128
#define CHUNK_HEIGHT 256
#define CHUNK_BLOCK_COUNT CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_WIDTH
#define CHUNK_BITSHIFT_AMOUNT 7
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
		void DeleteBlocks();
		Block* GetBlocks();
		i16* GetHeightestBlockY(u8 x, u8 z);
		std::vector<char> CompressToBuffer() const;

	private:
		ChunkPosition position;
		Block* blocks;
		i16 highest_blocks[CHUNK_WIDTH * CHUNK_WIDTH];
		char* compressed_blocks;
		size_t compressed_blocks_size;
		bool compressed;
	};
}  // namespace Eon
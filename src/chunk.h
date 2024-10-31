#pragma once

#include "block.h"
#include "chunk_position.h"

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 256
#define CHUNK_BLOCK_COUNT CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_WIDTH
#define CHUNK_BITSHIFT_AMOUNT 4
#define CHUNK_MAX_LOD 8

namespace Eon
{
	class Chunk
	{
	public:
		Chunk(ChunkPosition chunkPosition);
		~Chunk();
		Block* GetBlock(unsigned char x, short y, unsigned char z);
		ChunkPosition Position() const;
		void Compress();
		void Decompress();
		void DeleteBlocks();
		Block* GetBlocks();
		short* GetHeightestBlockY(unsigned char x, unsigned char z);
		std::vector<char> CompressToBuffer() const;

	private:
		ChunkPosition position;
		Block* blocks;
		short highest_blocks[CHUNK_WIDTH * CHUNK_WIDTH];
		char* compressed_blocks;
		size_t compressed_blocks_size;
		bool compressed;
	};
}  // namespace Eon